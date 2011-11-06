#include "StdAfx.h"
#include "Machine.h"

namespace LibCogmatics
{
	Machine::Machine(NodeID ID, CoString name) : NamedMachineNode(ID, name)
	{
	}

	Machine::~Machine(void)
	{
	}
}