#pragma once

#include <osg/Geometry>

#include "LibCogmatics.h"
#include "Units.h"

namespace LibCogmatics
{
	class ParametricSpurGear : public osg::Geometry
	{
	public:
		typedef osg::ref_ptr<ParametricSpurGear> Ptr;
		typedef osg::ref_ptr<ParametricSpurGear> CPtr;

		ParametricSpurGear(short numberOfTeeth, Length depth, Length axisDiameter, Length module);
		~ParametricSpurGear(void);

	protected:
		// Input parameters
		short  _numberOfTeeth;
		Length _depth;
		Length _module; /// <Inverse of what is often referred to as diametral pitch. Also the same as the addendum and denendum>
		Length _axisDiameter;
		// Derived parameters
		Length _rootDiameter;
		Length _outsideDiameter;
		Length _pitchDiameter;
		Length _toothThicknessBottom;
		Length _toothThicknessTop;
	};
}