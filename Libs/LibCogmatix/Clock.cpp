#include "StdAfx.h"
#include "Clock.h"

namespace LibCogmatix
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
		foreach(osg::observer_ptr<Motor> motorLocked, _motors)
		{
            Motor::Ptr motor = motorLocked.get();
            if (motor)
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
		foreach(osg::observer_ptr<Motor> motorLocked, _motors)
		{
            Motor::Ptr motor = motorLocked.get();
			if (motor && machine->containsNode(motor))
				erase(motor);
		}
	}

	void Clock::start()
	{
		_isRunning = true;
		_startTime = osg::Timer::instance()->tick();
	}

	double Clock::elapsed() const
	{
		return osg::Timer::instance()->time_s();
	}

}
