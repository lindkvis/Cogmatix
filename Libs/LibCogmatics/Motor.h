#pragma once

#include <osg/Group>
#include "LibCogmatics.h"
#include "Axis.h"
#include "Node.h"

namespace LibCogmatics
{
	class Motor : public MachineNode, public osg::Group
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
			return (getNumChildren() > 0) ? dynamic_cast<const Axis*>(getChild(0)) : nullptr;
		}
		Axis* getAxis ()
		{
			return (getNumChildren() > 0) ? dynamic_cast<Axis*>(getChild(0)) : nullptr;
		}

		void start() { _isRunning=true; }
		void stop() { _isRunning=false; }
		bool isRunning() { return _isRunning; }
		
	factory_protected:
		Motor(NodeID ID, double RPM);
		~Motor();
	protected:
		double _RPS; // rotations per second
		friend class Clock;
		void tick(double secs);
		bool _isRunning;
	};
}
