#pragma once

#include <osg/Group>
#include <osg/Timer>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Shape>

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
        // Valid actions
		virtual Actions validActions() const;
        virtual ActionResult perform(CoString action, const ActionArgs& args); 
        
	factory_protected:
		Motor(NodeID ID, double RPM);
        Motor(const Motor& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) 
        : TMachineNode<osg::Group>(copyFrom, copyop), _blocked(copyFrom._blocked), _RPS(copyFrom._RPS)
        {
        }

		~Motor();
	protected:
        Axis* axis;
		bool _blocked; // is it being blocked by a conflict in the gear chain?
		double _RPS; // rotations per second
		friend class Clock;
		void tick(double dsecs);
		bool _isRunning;
 	};
    
    class BoxMotor : public Motor
    {
    public:
        typedef osg::ref_ptr<BoxMotor> Ptr;
        typedef osg::ref_ptr<const BoxMotor> CPtr;
    factory_protected:
        BoxMotor(NodeID ID, double RPM, Vec boxCenter, Vec boxWidths) : Motor (ID, RPM)
        {
            osg::Geode* geode = new osg::Geode();
            osg::Box* box = new osg::Box(boxCenter, boxWidths[0], boxWidths[1], boxWidths[2]);
            osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
            geode->addDrawable(drawable);	
            
            addChild(geode);
        }
        ~BoxMotor() {}
        
    };
}
