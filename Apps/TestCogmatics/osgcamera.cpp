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


#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "LibCogmatics/Factory.h"
#include "LibCogmatics/Clock.h"

using namespace LibCogmatics;

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

int main( int argc, char **argv )
{
	
	osgViewer::Viewer viewer;
	viewer.getCamera()->setClearColor(osg::Vec4(0.7,0.7,0.8,0.5));

	Machine::Ptr machine = Factory::get()->CreateMachine("TestMachine");
	LinearAxis::Ptr axisLinear = Factory::get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	RotaryAxis::Ptr axisRotary = Factory::get()->CreateRotaryAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., -10000000., 10000000.);
	RotaryAxis::Ptr axisRotary2 = Factory::get()->CreateRotaryAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., -10000000., 10000000.);
	CompositePart::Ptr part = Factory::get()->CreateCompositePart("Test part", "D:\\Cogmotion\\3rdParty\\OpenSceneGraph\\data\\dumptruck.osg");
	ParametricSpurGearPart::Ptr gear = Factory::get()->CreateParametricSpurGearPart("TestGear", 100, Length(1.5*meters), Length(0.5*meters), Length(0.075*meters), Angle(0.1*si::radians)); 
	ParametricSpurGearPart::Ptr gear2 = Factory::get()->CreateParametricSpurGearPart("TestGear", 5, Length(2.*meters), Length(0.5*meters), Length(1*meters), Angle(0.*si::radians)); 
	Motor::Ptr motor = Factory::get()->CreateMotor(20);
	Motor::Ptr motor2 = Factory::get()->CreateMotor(-50);
	machine->addChild(motor);
	motor->addChild(axisRotary);
	motor2->addChild(axisRotary2);
	axisLinear->addChild(motor2);
	axisRotary->addChild(gear);
	axisRotary2->addChild(gear2);
	axisLinear->moveTo(50.);
	machine->addChild(axisLinear);
	//Clock::get()->add(motor);
	//Clock::get()->add(motor2);
	//axisRotary->moveTo(3.);

	/* osg::StateSet* state = machine->getOrCreateStateSet();
	osg::PolygonMode *polyModeObj;

	polyModeObj = dynamic_cast<osg::PolygonMode*>
		(state->getAttribute(osg::StateAttribute::POLYGONMODE));
	if ( !polyModeObj ) 
	{
		polyModeObj = new osg::PolygonMode;
		state->setAttribute(polyModeObj); 
	}
	polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	*/

	Light::Ptr lightBlue = Factory::get()->CreateLight(machine.get(), Vec(20., -20., 10.), Vec4(0.5, 0.5, 1., 1.));
	machine->addChild(lightBlue);
	Light::Ptr lightRed = Factory::get()->CreateLight(machine.get(), Vec(-50., -10., 30.), Vec4(1., 0.5, 0.5, 1.));
	machine->addChild(lightRed);
	
	osg::StateSet* gearState = machine->getOrCreateStateSet();

	// add a reflection map to the teapot.     
    osg::Image* image = osgDB::readImageFile("D:/Cogmotion/3rdParty/OpenSceneGraph/data/Images/skymap.jpg");
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



/*	osg::Program* brickProgramObject = new osg::Program;
	osg::Shader* brickVertexObject = 
		new osg::Shader( osg::Shader::VERTEX );
	osg::Shader* brickFragmentObject = 
		new osg::Shader( osg::Shader::FRAGMENT );
	brickProgramObject->addShader( brickFragmentObject );
	brickProgramObject->addShader( brickVertexObject );
	loadShaderSource( brickVertexObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.vert" );
	loadShaderSource( brickFragmentObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.frag" );
	
	gearState->setAttributeAndModes(brickProgramObject, osg::StateAttribute::ON);
	*/ 
	{
		boost::archive::xml_oarchive oa(std::cout);
		oa << make_nvp("my_machine", *machine);
		oa << make_nvp("Linear_axis", *axisLinear);
		oa << make_nvp("Rotary_axis", *axisRotary);
		oa << make_nvp("part", *gear);
	}
	viewer.setSceneData(machine.get());
	viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    viewer.realize();
	Clock::get()->start();
	
	while (!viewer.done())
    {
		Clock::get()->tick();
		viewer.frame(Clock::get()->elapsed());
	}


    return 0;
}