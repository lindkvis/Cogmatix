#include "StdAfx.h"
#include "Part.h"

namespace LibCogmatics
{
	Part::Part(NodeID ID, CoString name) : NamedMachineNode(ID, name)
	{
	}


	Part::~Part(void)
	{
	}

	ParametricSpurGearPart::ParametricSpurGearPart(NodeID ID, CoString name, short numberOfTeeth, Length depth, Length axisDiameter, Length module)
		: Part (ID, name)
	{
		// Create the child
		ParametricSpurGear::Ptr pChild = ParametricSpurGear::Ptr(new ParametricSpurGear(numberOfTeeth, depth, axisDiameter, module));
		addDrawable(pChild);
	}


	ParametricSpurGearPart::~ParametricSpurGearPart()
	{

	}
}