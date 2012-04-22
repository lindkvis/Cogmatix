#pragma once

#include <bitset>
#include <osg/PositionAttitudeTransform>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include "Node.h"

namespace LibCogmatix
{
	/**
	* Basic Abstract Axis class
	* Inherits osg::MatrixTransform, so that all children will receive any movement applied to this axis.
	* @author Gaute Lindkvist <lindkvis@gmail.com>
	*/
	class Axis : public TMachineNode<osg::PositionAttitudeTransform>
	{
	public:
		enum DiagnosticEnum
		{	
			Ok=0,
			Failed,
			LimitsExceeded,
			DiagnosticSize
		};
		typedef std::bitset<DiagnosticSize> Diagnostics;
	protected:
		Vec _origin;									   ///<Origin of the axis>
		Vec _axisVector;								   ///<Axis vector>
		float _value;									   ///<Current axis value>	
		float _valueInitial;							   ///<Initial axis value>
		float _min;                                        ///<Lower axis limit>
		float _max;                                        ///<Upper axis limit>

		static Diagnostics s_diagnostics;   ///<Diagnostic values. Will only be set if an axis function returns false>

		Axis(NodeID ID, const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max) 
			: TMachineNode(ID), _origin(origin), _axisVector (axisVector), _valueInitial (valueInitial), _min (min), _max (max)
		{
			reset();
			// TODO: throw exception if we're already outside axis limits
		}
        Axis() : TMachineNode<osg::PositionAttitudeTransform>() {}
        
        Axis(const Axis& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
            : TMachineNode<osg::PositionAttitudeTransform>(copyFrom, copyop)
            , _origin (copyFrom.origin()), _axisVector(copyFrom.vector()), _value (copyFrom.value()), _valueInitial(copyFrom.valueInitial())
            , _min (copyFrom.min()), _max (copyFrom.max()) {}

	public:
		typedef osg::ref_ptr<Axis> Ptr;
		typedef osg::ref_ptr<const Axis> CPtr;

		static const Diagnostics& GetDiagnostics() { return s_diagnostics; }
        
		virtual ~Axis() {};

		void reset ()
		{
			_attitude.makeRotate (Vec(0., 0., 1.), _axisVector);
			_position = _origin;
            dirtyBound();
		}
		virtual bool moveTo (float newValue)=0;
		virtual bool move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked)=0;
  		// Get methods
		virtual Vec origin() const { return _origin; }
		virtual void setOrigin (Vec origin) { _origin = origin; reset(); }
		const Vec& vector() const { return _axisVector; }
		float value() const { return _value; }
        float valueInitial() const { return _valueInitial; }
        float min() const { return _min; }
        float max() const { return _max; }
        // Get the world axis
        Vec worldAxis() const;
        // Get the world position
        Vec worldPosition() const;
        // Get the world matrix for the spur gear
        osg::Matrixd worldMatrix() const;
        // Get the world bounding sphere of the spur gear
        osg::BoundingSphere worldBound() const;
        
        virtual MachineNode::Compatibility isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave) const;
        bool moveOthers (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked);
	private:
	};

	class RotaryAxis : public Axis
	{
	public:
		typedef osg::ref_ptr<RotaryAxis> Ptr;
		typedef osg::ref_ptr<const RotaryAxis> CPtr;
		virtual bool moveTo(float newValue);
		virtual bool move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked);
        
        virtual MachineNode::Compatibility isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave) const;

        double diameter() const { return _axisDiameter; }
        double length() const { return _axisLength; }
        virtual bool snapTo (const MachineNode* master);
        
	factory_protected:
		RotaryAxis(NodeID ID, const Vec& axisVector, const Vec& origin, double axisDiameter, double axisLength, float valueInitial, float min, float max) 
			: Axis (ID, axisVector, origin, valueInitial, min, max), _axisDiameter(axisDiameter), _axisLength(axisLength)
        {
            _geode = new osg::Geode();
            addChild(_geode);
        
            osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder(Vec(0., 0., 0.), _axisDiameter/2., _axisLength);
            osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(cylinder);
            _geode->addDrawable(drawable);
        }
		virtual ~RotaryAxis() {}
        RotaryAxis() : Axis() {}
        RotaryAxis(const RotaryAxis& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
        : Axis(copyFrom, copyop) {}
        
    protected:
        osg::ref_ptr<osg::Geode> _geode;
        double _axisDiameter;
        double _axisLength;
	private:
	};

	class LinearAxis : public Axis
	{
	public:
		typedef osg::ref_ptr<LinearAxis> Ptr;
		typedef osg::ref_ptr<const LinearAxis> CPtr;
		virtual bool moveTo(float newValue);
		virtual bool move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked);
	factory_protected:
		LinearAxis(NodeID ID, const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max) 
			: Axis (ID, axisVector, origin, valueInitial, min, max) {}
		virtual ~LinearAxis() {}
        LinearAxis() : Axis() {}
	private:
	};

    struct AxisDistance
    {
        Axis* axis;
        double distance;
        AxisDistance(Axis* ax, Axis* master) : axis(ax)
        {
            distance = (master && ax) ? (axis->worldPosition() - master->worldPosition()).length() : 0.;
        }
        bool operator< (const AxisDistance& axd) const { return distance < axd.distance; }
    };
}
