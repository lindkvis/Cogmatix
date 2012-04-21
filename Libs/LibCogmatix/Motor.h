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
	class Motor : public RotaryAxis
	{
	public:
		typedef osg::ref_ptr<Motor> Ptr;
		typedef osg::ref_ptr<const Motor> CPtr;

		void start() { _isRunning=true; }
		void stop() { _isRunning=false; }
		bool isRunning() const { return _isRunning; }
		bool isBlocked() const { return _blocked; }
        // Valid actions
		virtual Actions validActions() const;
        virtual ActionResult perform(CoString action, const ActionArgs& args); 
        
	factory_protected:
		Motor(NodeID ID, double RPM, const Vec& axisVector, const Vec& origin, double axisDiameter, double axisLength);
        Motor(const Motor& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) 
        : RotaryAxis(copyFrom, copyop), _blocked(copyFrom._blocked), _RPS(copyFrom._RPS)
        {
        }

		~Motor();
	protected:
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
        BoxMotor(NodeID ID, double RPM, const Vec& axisVector, const Vec& origin, Vec boxWidths, double axisDiameter, double axisLength) : Motor (ID, RPM, axisVector, origin, axisDiameter, axisLength)
        {
            osg::Box* box = new osg::Box(Vec(0., 0., 0.), boxWidths[0], boxWidths[1], boxWidths[2]);
            osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
            _geode->addDrawable(drawable);	            
        }
        ~BoxMotor() {}
        
    };
}
