#include "StdAfx.h"
#include "Machine.h"

namespace LibCogmatix
{
	Machine::Machine(NodeID ID, CoString name) : MachineNode(ID)
	{
        setName(name);
	}

	Machine::~Machine(void)
	{
	}
}