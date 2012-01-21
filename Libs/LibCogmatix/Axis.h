#pragma once

#include <bitset>

#include "Node.h"

namespace LibCogmatix
{
	/**
	* Basic Abstract Axis class
	* Inherits osg::MatrixTransform, so that all children will receive any movement applied to this axis.
	* @author Gaute Lindkvist <lindkvis@gmail.com>
	*/
	class Axis : public MachineNode, public osg::MatrixTransform
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
			: MachineNode(ID), _origin(origin), _axisVector (axisVector), _valueInitial (valueInitial), _min (min), _max (max)
		{
			reset();
			// TODO: throw exception if we're already outside axis limits
		}
	public:
		typedef osg::ref_ptr<Axis> Ptr;
		typedef osg::ref_ptr<const Axis> CPtr;

		static const Diagnostics& GetDiagnostics() { return s_diagnostics; }
	
		virtual ~Axis() {};

		void reset () { _matrix = Matrix::translate(_origin); _value = _valueInitial; }
		virtual bool moveTo (float newValue)=0;
		virtual bool move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master)=0;

		// Get methods
		const Vec& origin() { return _origin; }
		const Vec& vector() { return _axisVector; }
		const float& value() { return _value; }
	private:
	};

	class RotaryAxis : public Axis
	{
	public:
		typedef osg::ref_ptr<RotaryAxis> Ptr;
		typedef osg::ref_ptr<const RotaryAxis> CPtr;
		virtual bool moveTo(float newValue);
		virtual bool move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master);
	factory_protected:
		RotaryAxis(NodeID ID, const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max) 
			: Axis (ID, axisVector, origin, valueInitial, min, max) {}
		virtual ~RotaryAxis() {}
	private:
	};

	class LinearAxis : public Axis
	{
	public:
		typedef osg::ref_ptr<LinearAxis> Ptr;
		typedef osg::ref_ptr<const LinearAxis> CPtr;
		virtual bool moveTo(float newValue);
		virtual bool move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master);
	factory_protected:
		LinearAxis(NodeID ID, const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max) 
			: Axis (ID, axisVector, origin, valueInitial, min, max) {}
		virtual ~LinearAxis() {}
	private:
	};
}
