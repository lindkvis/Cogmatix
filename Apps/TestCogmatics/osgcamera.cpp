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

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "LibCogmatics/Factory.h"

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

	Machine::Ptr machine = Factory::Get()->CreateMachine("TestMachine");
	LinearAxis::Ptr axisLinear = Factory::Get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	RotaryAxis::Ptr axisRotary = Factory::Get()->CreateRotaryAxis(Vec(0., 1., 0.), Vec(0., 0., 0.), 0., -100., 1000.);
	//CompositePart::Ptr part = Factory::Get()->CreateCompositePart("Test part", "D:\\Cogmotion\\3rdParty\\OpenSceneGraph\\data\\dumptruck.osg");
	Part::Ptr part2 = Factory::Get()->CreatePart("Test cog");
	ParametricSpurGear::Ptr gear = Factory::Get()->CreateParametricSpurGear(30, Length(50.*meters), Length(20.*meters), Length(10.*meters)); 
	part2->addDrawable(gear);
	machine->addChild(axisRotary);
	axisRotary->addChild(axisLinear);
	
	axisLinear->addChild(part2);
	//axisRotary->addChild(part);
	axisLinear->moveTo(99.);
	axisRotary->moveTo(3.);

	osg::StateSet* state = part2->getOrCreateStateSet();
	osg::PolygonMode *polyModeObj;

	polyModeObj = dynamic_cast<osg::PolygonMode*>
		(state->getAttribute(osg::StateAttribute::POLYGONMODE));
	if ( !polyModeObj ) 
	{
		polyModeObj = new osg::PolygonMode;
		state->setAttribute(polyModeObj); 
	}
	//polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);

	 /* osg::Program* brickProgramObject = new osg::Program;
	 osg::Shader* brickVertexObject = 
		 new osg::Shader( osg::Shader::VERTEX );
	 osg::Shader* brickFragmentObject = 
		 new osg::Shader( osg::Shader::FRAGMENT );
	 brickProgramObject->addShader( brickFragmentObject );
	 brickProgramObject->addShader( brickVertexObject );
	 loadShaderSource( brickVertexObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.vert" );
	 loadShaderSource( brickFragmentObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.frag" );

	 brickState->setAttributeAndModes(brickProgramObject, osg::StateAttribute::ON);
	 */
	{
		boost::archive::xml_oarchive oa(std::cout);
		oa << make_nvp("my_machine", *machine);
		oa << make_nvp("Linear_axis", *axisLinear);
		oa << make_nvp("Rotary_axis", *axisRotary);
		oa << make_nvp("part", *part2);
	}
	viewer.setSceneData(machine.get());
    viewer.run();
    return 0;
}