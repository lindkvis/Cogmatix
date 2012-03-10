#pragma once

#include <osg/Group>
#include <osg/Timer>

#include "LibCogmatix.h"
#include "Axis.h"
#include "Node.h"


namespace LibCogmatix
{
	class Motor : public TMachineNode<osg::Group>
	{
	public:
		typedef osg::ref_ptr<Motor> Ptr;
		typedef osg::ref_ptr<const Motor> CPtr;
		void setAxis (Axis::Ptr axis)
		{
           setChild(0, axis.get());
		}
		const Axis* getAxis () const
		{
            return findChildOfType<Axis> (this);
		}
		Axis* getAxis ()
		{
            return findChildOfType<Axis> (this);
		}

		void start() { _isRunning=true; }
		void stop() { _isRunning=false; }
		bool isRunning() const { return _isRunning; }
		bool isBlocked() const { return _blocked; }
		
	factory_protected:
		Motor(NodeID ID, double RPM);
		~Motor();
	protected:
		bool _blocked; // is it being blocked by a conflict in the gear chain?
		double _RPS; // rotations per second
		friend class Clock;
		void tick(double dsecs);
		bool _isRunning;
 	};
}
