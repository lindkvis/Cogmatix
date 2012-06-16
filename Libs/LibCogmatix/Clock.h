#pragma once

#include <osg/Timer>
#include <osg/observer_ptr>

#include "Machine.h"
#include "Motor.h"

namespace LibCogmatix
{
	typedef std::set<osg::observer_ptr<Motor> > Motors;
	class Clock
	{
	public:
		/**	 
		 * Static singleton method. Will create clock if necessary
		 */
		static Clock* get();
		void tick();
		void add(Motor* motor) { _motors.insert(motor); }
		void add(Machine* machine);
		void clear() { _motors.clear(); }
		void erase(Motor* motor) { _motors.erase(motor); }
		void erase(Machine* machine);
		void start();
		void stop() { _isRunning = false; }
		bool isRunning() const { return _isRunning; }
		double elapsed() const;
	private:
		Clock() {}
		~Clock();
		// Private copy constructor and assigned operator. Never to be used.
		Clock(Clock const&) {}	
		Clock& operator=(Clock const&) { return *m_pInstance; };

		static Clock* m_pInstance;
		// The list of motors to be operated by the clock.
		Motors _motors;
		osg::Timer_t _startTime;
		bool _isRunning;
	};
}
