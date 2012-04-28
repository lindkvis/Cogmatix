/* OpenSceneGraph example, osgcamera.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include "stdafx.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <iostream>
#include <osg/TexGen>
#include <osg/Texture2D>
#include <osgWidget/Util>
#include <osgWidget/WindowManager>
#include <osg/Material>
#include <osgGA/StateSetManipulator>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>

#include "OsgPlugins.h"
#include "LibCogmatix/Factory.h"
#include "LibCogmatix/Clock.h"
#include "LibCogmatix/Light.h"
#include "LibCogmatix/Node.h"
#include "LibCogmatix/GearPart.h"


#include "EventHandler.h"

using namespace LibCogmatix;
using namespace Cogmatix;

bool loadShaderSource(osg::Shader* obj, const std::string& fileName )
{
	std::string fqFileName = osgDB::findDataFile(fileName);
	if( fileName.length() == 0 )
	{
		std::cout << "File \"" << fileName << "\" not found." << std::endl;
		return false;
	}
	bool success = obj->loadShaderSourceFromFile( fileName.c_str());
	if ( !success  )
	{
		std::cout << "Couldn't load file: " << fileName << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}

osg::Node* createBase(const osg::Vec3& center,float radius)
{
#define numTilesX 10
#define numTilesZ 10
    
    float width = 2*radius;
    float height = 2*radius;
    
    osg::Vec3 v000(center - osg::Vec3(width*0.5f,0.0f, height*0.5f));
    osg::Vec3 dx(osg::Vec3(width/((float)numTilesX),0.0,0.0f));
    osg::Vec3 dz(osg::Vec3(0.0f,0.0f, height/((float)numTilesZ)));
    
    osg::Vec2 tdx = osg::Vec2(1.f, 0.f);
    osg::Vec2 tdy = osg::Vec2(0.f, 1.f);
    
    // fill in vertices for grid, note numTilesX+1 * numTilesY+1...
    osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
    int iz;
    for(iz=0;iz<=numTilesZ;++iz)
    {
        for(int ix=0;ix<=numTilesX;++ix)
        {
            coords->push_back(v000+dx*(float)ix+dz*(float)iz);
            texcoords->push_back(tdx*(float)ix+tdy*(float)iz);
        }
    }
    
    //Just two colours - black and white.
    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,1.0f,1.0f)); // white
    colors->push_back(osg::Vec4(1.f,1.f,1.f,1.0f)); // grey
    int numColors=colors->size();
    
    
    int numIndicesPerRow=numTilesX+1;
    int numIndicesPerColumn=numTilesZ+1;
  
    osg::UIntArray* coordIndices = new osg::UIntArray; // assumes we are using less than 256 points...
    osg::UIntArray* colorIndices = new osg::UIntArray;
    osg::ref_ptr<osg::Vec2Array> texpoints = new osg::Vec2Array;
    for(iz=0;iz<numTilesZ;++iz)
    {
        for(int ix=0;ix<numTilesX;++ix)
        {
            // four vertices per quad.
            coordIndices->push_back(ix+iz*numIndicesPerRow);
            osg::Vec2 v = (*texcoords)[coordIndices->back()];
            texpoints->push_back(v);
            
            coordIndices->push_back((ix+1)+iz*numIndicesPerRow);
            v = (*texcoords)[coordIndices->back()];
            texpoints->push_back((*texcoords)[coordIndices->back()]);
            
            coordIndices->push_back((ix+1)+(iz+1)*numIndicesPerRow);
            v = (*texcoords)[coordIndices->back()];
            texpoints->push_back((*texcoords)[coordIndices->back()]);
            
            coordIndices->push_back(ix+(iz+1)*numIndicesPerRow);
            v = (*texcoords)[coordIndices->back()];
            texpoints->push_back((*texcoords)[coordIndices->back()]);
            
            // one color per quad
            colorIndices->push_back((ix+iz)%numColors);
        }
    }
    
    
    // set up a single normal
    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
    
    
    osg::Geometry* geom = new osg::Geometry;
    geom->setVertexArray(coords);
    geom->setVertexIndices(coordIndices);
    geom->setTexCoordArray(0, texpoints);
    geom->setColorArray(colors);
    geom->setColorIndices(colorIndices);
    geom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
    
    geom->setNormalArray(normals);
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,coordIndices->size()));
    
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geom);
    
    return geode;
}


const unsigned int MASK_2D = 0xF0000000;

int main( int argc, char **argv )
{
    const int ReceivesShadowTraversalMask = 0x1;
    
    const int CastsShadowTraversalMask = 0x2;
    
	osgViewer::Viewer viewer;
    osgDB::setLibraryFilePathList("."); 
    
	osgWidget::WindowManager* wm = new osgWidget::WindowManager(
		&viewer,
		1024.0f,
		768.0f,
		MASK_2D,
		0 //osgWidget::WindowManager::WM_PICK_DEBUG
		);
	osg::Camera* camera = wm->createParentOrthoCamera();
	viewer.getCamera()->setClearColor(osg::Vec4(0.0,0.0,0.0,1.0));
	Machine::Ptr machine = Factory::get()->CreateMachine("TestMachine");

	ParametricSpurGearPart::Ptr gear = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(0., 0., 0.), 40, 1.5, 1.0, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear2 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(7.5, 0., 0.),10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear3 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 0.), 10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear4 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 5.1), 24, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0, 12.5), 20, 1.0, 0.5, 0.3, 0.);

	gear2->snapTo(gear);
	gear3->snapTo(gear2);
	gear4->snapTo(gear3);
    gear5->snapTo(gear4);
    
    gear->setNodeMask(CastsShadowTraversalMask);
    gear2->setNodeMask(CastsShadowTraversalMask);
    gear3->setNodeMask(CastsShadowTraversalMask);
    gear4->setNodeMask(CastsShadowTraversalMask);
    gear5->setNodeMask(CastsShadowTraversalMask);

	//ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, -2., 5.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
	//ParametricSpurGearPart::Ptr gear6 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 0., 1.), Vec(10.5, -6., 10.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
	bool bFastPaths = gear->gear()->areFastPathsUsed();
	BoxMotor::Ptr motor = Factory::get()->CreateBoxMotor(20, Vec(0., 1., 0.), Vec(0., 5., 0.), Vec(5., 5., 5.), 1.0, 7.5);
	machine->addChild(motor);
    machine->addChild(gear);
	machine->addChild(gear2);
	machine->addChild(gear3);
	machine->addChild(gear4);
	machine->addChild(gear5);
    
    osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints;
    hints->setDetailRatio(2.0f);

    osg::ref_ptr<osg::Node> plane_geode = createBase(Vec(0., 8., 0.), 100);
	plane_geode->setNodeMask(ReceivesShadowTraversalMask);
    osg::StateSet* planeState = plane_geode->getOrCreateStateSet();
    
    osg::ref_ptr<osg::Texture2D> planeTex = new osg::Texture2D(osgDB::readImageFile("paper_tile.jpg"));
    //    planeTex->setFilter(osg::Texture::MIN_FILTER,osg::Texture::NEAREST_MIPMAP_NEAREST);
    //    planeTex->setFilter(osg::Texture::MAG_FILTER,osg::Texture::NEAREST_MIPMAP_NEAREST);
    planeTex->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);    
    planeTex->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);    
    planeState->setTextureAttributeAndModes( 0, planeTex , osg::StateAttribute::ON);    
    //machine->addChild(gear6);
	Clock::get()->add(motor);

	osg::StateSet* gearState = machine->getOrCreateStateSet();

	// add a reflection map to the teapot.     
	/* osg::Image* image = osgDB::readImageFile("skymap.jpg");
	if (image)
	{
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);

		osg::TexGen* texgen = new osg::TexGen;
		texgen->setMode(osg::TexGen::SPHERE_MAP);

		gearState->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
		gearState->setTextureAttributeAndModes(0,texgen,osg::StateAttribute::ON);

		//geode->setStateSet(stateset);
	}*/

    motor->start();


/*	osg::Program* brickProgramObject = new osg::Program;
	osg::Shader* brickVertexObject = 
		new osg::Shader( osg::Shader::VERTEX );
	osg::Shader* brickFragmentObject = 
		new osg::Shader( osg::Shader::FRAGMENT );
	brickProgramObject->addShader( brickFragmentObject );
	brickProgramObject->addShader( brickVertexObject );
	loadShaderSource( brickVertexObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.vert" );
	loadShaderSource( brickFragmentObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.frag" );
	*/
    osg::ref_ptr<osgShadow::ShadowedScene> world = new osgShadow::ShadowedScene;
    world->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
    world->setCastsShadowTraversalMask(CastsShadowTraversalMask);
    
    osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
    world->setShadowTechnique(sm.get());    
    int mapresx = 1024, mapresy=768;
    sm->setTextureSize(osg::Vec2s(mapresx,mapresy));
    sm->setPolygonOffset(osg::Vec2(-2, -2));
    
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild(world);
	world->addChild(machine);
	root->addChild(camera);
    world->addChild(plane_geode);
    
    Light::Ptr lightBlue = Factory::get()->CreateLight(machine.get(), Vec(30., -200., 30.), Vec4(0.95, 0.95, 1., 1.));
	world->addChild(lightBlue);
    //	Light::Ptr lightRed = Factory::get()->CreateLight(machine.get(), Vec(-50., -10., 30.), Vec4(1., 0.6, 0.6, 1.));
    //    machine->addChild(lightRed);
    
	viewer.setSceneData(root);

	Vec vecGear2 = gear2->worldPosition();
	Vec vecGear1 = gear->worldPosition();

	double distance = (vecGear1 - vecGear2).length();
	unsigned int clearMask = viewer.getCamera()->getClearMask();
	viewer.getCamera()->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
	viewer.getCamera()->setClearStencil(0);
    osg::ref_ptr<EventHandler> handler = new EventHandler(&viewer, wm, machine);
	// add the handler for doing the picking
	viewer.addEventHandler(handler);
	viewer.addEventHandler(new osgWidget::KeyboardHandler(wm));
    //	viewer.addEventHandler(new osgWidget::ResizeHandler(wm, camera));
	viewer.addEventHandler(new osgWidget::CameraSwitchHandler(wm, camera));
	viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgViewer::WindowSizeHandler());
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

    
    viewer.setUpViewInWindow(
        50,
        50,
        static_cast<int>(wm->getWidth()),
        static_cast<int>(wm->getHeight())
    );
	wm->resizeAllWindows();

	viewer.realize();
	//Clock::get()->start();

    // testing
    bool addedNewGear=false;
    bool bStopped=false;
    bool bStarted=false;
	while (!viewer.done())
	{
		Clock::get()->tick();
        viewer.frame(Clock::get()->elapsed());
        handler->snapToLimit();
        /*
        if (Clock::get()->elapsed() > 10.)
            gear4->perform("remove", ActionArgs());
        if (Clock::get()->elapsed() > 5.2 && !addedNewGear)
        {
            gear5->perform("copy", ActionArgs());
            addedNewGear=true;
        }        
        if (Clock::get()->elapsed() > 2.5 && !bStopped)
        {
            motor->perform("stop", ActionArgs());
            bStopped=true;
        }
        if (Clock::get()->elapsed() > 7.5 && !bStarted)
        {
            motor->perform("start", ActionArgs());
            bStarted=true;
        }*/
            
	}


	return 0;
}
