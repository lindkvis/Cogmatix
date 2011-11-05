// TestCogmatics.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "LibCogmatics/Factory.h"

using namespace LibCogmatics;

int _tmain(int argc, _TCHAR* argv[])
{
	Machine::Ptr machine = Factory::Get()->CreateMachine(L"TestMachine");
	LinearAxis::Ptr axis = Factory::Get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	bool worked = machine->addChild(axis);
	std::wcout << machine->Name() << std::endl;
	return 0;
}

