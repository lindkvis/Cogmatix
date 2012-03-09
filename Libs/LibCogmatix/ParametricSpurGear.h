#pragma once

#include <osg/Geometry>

#include "LibCogmatix.h"
#include "Units.h"

namespace LibCogmatix
{
struct dereference_less
{
	template<class T, class U>
	inline bool operator()(const T& lhs, const U& rhs) const
	{
		return *lhs < *rhs;
	}
};
struct derefence_equals
{
	template<class T, class U>
	inline bool operator()(const T& lhs, const U& rhs) const
	{
		return *lhs < *rhs;
	}
};
class ParametricSpurGear: public osg::Geometry
{
public:
	// Input parameters
	struct GearParameters
	{
		short _numberOfTeeth;
		double _depth;
		double _module; /// <Inverse of what is often referred to as diametral pitch. Also the same as the addendum and denendum>
		double _axisDiameter;
		double _helix; /// <Helix offset angle. 0 means normal spur gear>
		double _pitch_angle; /// <Angle between pitch surface and axis. If not 90 degrees (PI/2), the gear will be conical.
		GearParameters(short numberOfTeeth, double depth, double axisDiameter, double module, double helix, double pitch_angle) :
				_numberOfTeeth(numberOfTeeth), _depth(depth), _module(module),
				_axisDiameter(axisDiameter), _helix(helix), _pitch_angle(pitch_angle)
		{
		}
		bool operator==(const GearParameters& rhs) const
		{
			return _numberOfTeeth == rhs._numberOfTeeth && _depth == rhs._depth
					&& _module == rhs._module
					&& _axisDiameter == rhs._axisDiameter
					&& _helix == rhs._helix
					&& _pitch_angle==rhs._pitch_angle;
		}
		bool operator<(const GearParameters& rhs) const
		{
			return (_module < rhs._module || _helix < rhs._helix || _pitch_angle < rhs._pitch_angle
					|| _numberOfTeeth < rhs._numberOfTeeth
					|| _depth < rhs._depth || _axisDiameter < rhs._axisDiameter);
		}

	};
	typedef osg::ref_ptr<ParametricSpurGear> Ptr;
	typedef osg::ref_ptr<ParametricSpurGear> CPtr;

	typedef std::map<GearParameters, ParametricSpurGear::Ptr> GearMap;

	double module() const
	{
		return params._module;
	}
	double helixAnglePerUnitDepth() const
	{
		return (params._helix / params._depth);
	}
	double pitchAngle() const
	{
		return params._pitch_angle;
	}
	double pitchRadius() const
	{
		return _pitchDiameter / 2.;
	}
	double depth() const
	{
		return params._depth;
	}
	static GearMap s_Gears; // Set of gears that can be reused
	static ParametricSpurGear::Ptr Create(short numberOfTeeth, double depth,
			double axisDiameter, double module, double helix, double pitch_angle);
	ParametricSpurGear(const GearParameters& params);
	~ParametricSpurGear(void);
	// Given a vector of orientation. How much tooth is at that position 
    // (1 is right centered on the tooth, 0 is right centered on a gap).
	double toothRatio (const Vec& v) const;
    // Given a tooth ratio to match and our own orientation vector, how much
    // do we need to rotate by to match the ratio.
    double angleFromRatio (double ratioOther, const Vec& vOwn) const;
protected:
	// Input parameters
	GearParameters params;
	// Derived parameters
	double _rootDiameter;
	double _outsideDiameter;
	double _pitchDiameter;
	double _toothThicknessBottom;
	double _toothThicknessTop;
};
}
