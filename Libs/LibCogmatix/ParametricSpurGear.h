#pragma once

#include <osg/Geometry>

#include "LibCogmatix.h"
#include "Units.h"

namespace LibCogmatix
{
	class ParametricSpurGear : public osg::Geometry
	{
	public:
		typedef osg::ref_ptr<ParametricSpurGear> Ptr;
		typedef osg::ref_ptr<ParametricSpurGear> CPtr;
		double module() const { return _module; }
		double angle_per_unit_depth() const { return (_helix / _depth);  }
		double pitch() const { return _pitchDiameter; }
	protected:
		// Input parameters
		short  _numberOfTeeth;
		double _depth;
		double _module; /// <Inverse of what is often referred to as diametral pitch. Also the same as the addendum and denendum>
		double _axisDiameter;
		// Derived parameters
		double _rootDiameter;
		double _outsideDiameter;
		double _pitchDiameter;
		double _toothThicknessBottom;
		double _toothThicknessTop;
		double _helix; /// <Helix offset angle. 0 means normal spur gear>
	protected:
		friend class ParametricSpurGearPart; // The only class allowed to create Parametric Spur Gears.
		ParametricSpurGear(short numberOfTeeth, double depth, double axisDiameter, double module, double helix);
		~ParametricSpurGear(void);

	};
}