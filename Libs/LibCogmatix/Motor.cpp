#include "StdAfx.h"
#include "Motor.h"

namespace LibCogmatix
{
	Motor::Motor(NodeID ID, double RPM) : MachineNode(ID), _RPS(RPM/60.), _blocked(false), _isRunning(false)
	{
	}


	Motor::~Motor(void)
	{
	}

	void Motor::tick(double secs)
	{
		static double blockTimer=0.;
		static double RPS = 0.25*_RPS;
		if (!_blocked)
			RPS = _RPS; // Make sure it is always forwards at full speed if we're not blocked
		if (isRunning() && getAxis())
		{
			std::set<const MachineNode*> chain;
			if (!getAxis()->move(secs * RPS, chain, this))
			{
				_blocked=true;
				if (blockTimer > 0.25)
				{
					blockTimer = 0.;
					RPS *= -1;
				}
				else
					blockTimer += secs;
			}
		}
	}
}
