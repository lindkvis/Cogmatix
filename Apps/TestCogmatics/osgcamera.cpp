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
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <iostream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "LibCogmatics/Factory.h"

using namespace LibCogmatics;

int main( int argc, char **argv )
{
	
	osgViewer::Viewer viewer;

	Machine::Ptr machine = Factory::Get()->CreateMachine("TestMachine");
	LinearAxis::Ptr axis = Factory::Get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	Part::Ptr part = Factory::Get()->CreatePart("Test part", "D:\\Cogmotion\\3rdParty\\OpenSceneGraph\\data\\dumptruck.osg");
	bool worked = machine->addChild(axis);
	axis->addChild(part);
	{
		boost::archive::xml_oarchive oa(std::cout);
		oa << make_nvp("my_machine", *machine);
		oa << make_nvp("axis", *axis);
		oa << make_nvp("part", *part);
	}
	viewer.setSceneData(machine.get());
    viewer.run();
    return 0;
}