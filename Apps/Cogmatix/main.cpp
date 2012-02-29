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
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osg/PolygonMode>
#include <iostream>
#include <osg/TexGen>
#include <osg/Texture2D>
#include <osgWidget/Util>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/WindowManager>

#include "LibCogmatix/Factory.h"
#include "LibCogmatix/Clock.h"

#include "PickHandler.h"

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

const unsigned int MASK_2D = 0xF0000000;

int main( int argc, char **argv )
{

	osgViewer::Viewer viewer;

	osgWidget::WindowManager* wm = new osgWidget::WindowManager(
		&viewer,
		1280.0f,
		1024.0f,
		MASK_2D,
		0 //osgWidget::WindowManager::WM_PICK_DEBUG
		);
	osg::Camera* camera = wm->createParentOrthoCamera();
	viewer.getCamera()->setClearColor(osg::Vec4(0.7,0.7,0.8,0.5));

	Machine::Ptr machine = Factory::get()->CreateMachine("TestMachine");
	LinearAxis::Ptr axisLinear = Factory::get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	LinearAxis::Ptr axisLinear2 = Factory::get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	LinearAxis::Ptr axisLinear3 = Factory::get()->CreateLinearAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., 0., 100.);
	//RotaryAxis::Ptr axisRotary = Factory::get()->CreateRotaryAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., -10000000., 10000000.);
	//RotaryAxis::Ptr axisRotary2 = Factory::get()->CreateRotaryAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., -10000000., 10000000.);
	//CompositePart::Ptr part = Factory::get()->CreateCompositePart("Test part", "D:\\Cogmotion\\3rdParty\\OpenSceneGraph\\data\\dumptruck.osg");
	ParametricSpurGearPart::Ptr gear = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(0., 0., 0.), 40, 1.5, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear2 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(7.5, 0., 0.),10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear3 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 0.), 10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear4 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 5.1), 24, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0, 12.5), 20, 1.0, 0.5, 0.3, 0.);

//	gear2->snapTo(gear);
//	gear3->snapTo(gear2);
	gear4->snapTo(gear3);
    gear5->snapTo(gear4);
	//ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, -2., 5.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
	//ParametricSpurGearPart::Ptr gear6 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 0., 1.), Vec(10.5, -6., 10.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
	bool bFastPaths = gear->gear()->areFastPathsUsed();
	Motor::Ptr motor = Factory::get()->CreateMotor(20);
	machine->addChild(motor);
	motor->addChild(gear);
	machine->addChild(gear2);
	machine->addChild(gear3);
	machine->addChild(gear4);
	machine->addChild(gear5);
	//machine->addChild(gear6);

	Clock::get()->add(motor);

	Light::Ptr lightBlue = Factory::get()->CreateLight(machine.get(), Vec(20., -20., 10.), Vec4(0.5, 0.5, 1., 1.));
	machine->addChild(lightBlue);
	Light::Ptr lightRed = Factory::get()->CreateLight(machine.get(), Vec(-50., -10., 30.), Vec4(1., 0.5, 0.5, 1.));
	machine->addChild(lightRed);

	osg::StateSet* gearState = machine->getOrCreateStateSet();

	// add a reflection map to the teapot.     
	osg::Image* image = osgDB::readImageFile("reflect.rgb");
	if (image)
	{
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);

		osg::TexGen* texgen = new osg::TexGen;
		texgen->setMode(osg::TexGen::SPHERE_MAP);

		gearState->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
		gearState->setTextureAttributeAndModes(0,texgen,osg::StateAttribute::ON);

		//geode->setStateSet(stateset);
	}
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
	osg::Group* world = new osg::Group();
	world->addChild(machine);
	world->addChild(camera);
	viewer.setSceneData(world);

	Vec vecGear2 = gear2->worldPosition();
	Vec vecGear1 = gear->worldPosition();

	double distance = (vecGear1 - vecGear2).length();
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
	unsigned int clearMask = viewer.getCamera()->getClearMask();
	viewer.getCamera()->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
	viewer.getCamera()->setClearStencil(0);
	// add the handler for doing the picking
	viewer.addEventHandler(new PickHandler(wm, machine));

	viewer.addEventHandler(new osgWidget::MouseHandler(wm));
	viewer.addEventHandler(new osgWidget::KeyboardHandler(wm));
	viewer.addEventHandler(new osgWidget::ResizeHandler(wm, camera));
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

	while (!viewer.done())
	{
		Clock::get()->tick();
		viewer.frame(Clock::get()->elapsed());
	}


	return 0;
}
