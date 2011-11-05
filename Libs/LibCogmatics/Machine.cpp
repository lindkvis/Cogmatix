#include "StdAfx.h"
#include "Machine.h"

namespace LibCogmatics
{
	Machine::Machine(NodeID ID, const String& name) : NamedMachineNode(ID, name)
	{
	}

	Machine::~Machine(void)
	{
	}
}