// TestCogmatics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <boost/archive/xml_wiarchive.hpp>
#include <boost/archive/xml_woarchive.hpp>

#include "LibCogmatics/Factory.h"

using namespace LibCogmatics;


int _tmain(int argc, _TCHAR* argv[])
{
	Machine::Ptr machine = Factory::Get()->CreateMachine("TestMachine");
	LinearAxis::Ptr axis = Factory::Get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	CompositePart::Ptr part = Factory::Get()->CreatePart("Test part", "D:\\Cogmotion\\3rdParty\\OpenSceneGraph\\data\\dumptruck.osg");
	bool worked = machine->addChild(axis);
	axis->addChild(part);
	std::cout << machine->Name() << std::endl;

	{
		//std::wofstream file ("First.xml");
		//boost::archive::xml_woarchive oxml (std::wcout);
		//oxml << make_nvp("Machine 1", *machine);
	}
	machine->SetName("New name");
	{
		//std::wofstream file ("Second.xml");
		//boost::archive::xml_woarchive oxml (std::wcout);
		//oxml << make_nvp("Machine 2", *machine);		
	}

	/* {
		Machine::Ptr newMachine = Factory::Get()->CreateMachine(L"Test");
		std::wifstream file ("First.xml");
		boost::archive::xml_wiarchive ixml (file);
		ixml >> make_nvp("Machine 1", *newMachine;
		std::wcout << newMachine->Name() << std::endl;
	}*/
	return 0;
}

