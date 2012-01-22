#pragma once

#include <osg/Group>
#include "LibCogmatix.h"
#include "Axis.h"
#include "Node.h"

namespace LibCogmatix
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
		bool isRunning() const { return _isRunning; }
		bool isBlocked() const { return _blocked; }
		
	factory_protected:
		Motor(NodeID ID, double RPM);
		~Motor();
	protected:
		bool _blocked; // is it being blocked by a conflict in the gear chain?
		double _RPS; // rotations per second
		friend class Clock;
		void tick(double secs);
		bool _isRunning;
	};
}
