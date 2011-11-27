#include "StdAfx.h"
#include "Clock.h"

namespace LibCogmatics
{
		// Initialise the static pointer.
	Clock* Clock::m_pInstance = nullptr;

	Clock* Clock::get()
	{
		if (nullptr==m_pInstance)
			m_pInstance = new Clock();
		return m_pInstance;
	}

	Clock::~Clock()
	{
	}

	void Clock::tick ()
	{
		static osg::Timer_t last = osg::Timer::instance()->tick();
		osg::Timer_t current = osg::Timer::instance()->tick();
		double elapsed = osg::Timer::instance()->delta_s(last, current);
		foreach(Motor* motor, _motors)
		{
			motor->tick(elapsed);
		}
		last = current;
	}

	void Clock::add (Machine* machine)
	{
		for (unsigned int i=0; i < machine->getNumChildren(); ++i)
		{
			Motor* motor = dynamic_cast<Motor*>(machine->getChild(i));
			if (motor)
				add(motor);
		}
	}

	void Clock::erase(Machine* machine)
	{
		foreach(Motor* motor, _motors)
		{
			if (machine->containsNode(motor))
				erase(motor);
		}
	}

	void Clock::start()
	{
		_isRunning = true;
		_startTime = osg::Timer::instance()->getStartTick();
	}

	double Clock::elapsed() const
	{
		osg::Timer_t current = osg::Timer::instance()->tick();
		return osg::Timer::instance()->delta_s(_startTime, current);
	}

}