#include "StdAfx.h"
#include "Motor.h"

namespace LibCogmatix
{
	Motor::Motor(NodeID ID, double RPM) : MachineNode(ID), _RPS(RPM/60.)
	{
	}


	Motor::~Motor(void)
	{
	}

	void Motor::tick(double secs)
	{
		if (getAxis())
			getAxis()->move(secs * _RPS);
	}
}