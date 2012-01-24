#pragma once

#include <osg/Geometry>

#include "LibCogmatix.h"
#include "Units.h"

namespace LibCogmatix
{
	struct dereference_less
	{
		template<class T, class U>
		inline bool operator() (const T& lhs,const U& rhs) const
		{
			return *lhs < *rhs;
		}
	};
	struct derefence_equals
	{
		template<class T, class U>
		inline bool operator() (const T& lhs,const U& rhs) const
		{
			return *lhs < *rhs;
		}
	};
	class ParametricSpurGear : public osg::Geometry
	{
	public:
		// Input parameters
		struct GearParameters
		{
			short  _numberOfTeeth;
			double _depth;
			double _module; /// <Inverse of what is often referred to as diametral pitch. Also the same as the addendum and denendum>
			double _axisDiameter;
			double _helix; /// <Helix offset angle. 0 means normal spur gear>
			GearParameters(short numberOfTeeth, double depth, double axisDiameter, double module, double helix)
			: _numberOfTeeth(numberOfTeeth), _depth(depth), _module(module), _axisDiameter(axisDiameter), _helix(helix) {}
			bool operator==(const GearParameters& rhs) const
						{
				return _numberOfTeeth==rhs._numberOfTeeth && _depth==rhs._depth && _module==rhs._module && _axisDiameter==rhs._axisDiameter && _helix==rhs._helix;
						}
			bool operator<(const GearParameters& rhs) const
			{
				return (_module<rhs._module || _helix<rhs._helix || _numberOfTeeth<rhs._numberOfTeeth || _depth<rhs._depth || _axisDiameter<rhs._axisDiameter);
			}

		};
		typedef osg::ref_ptr<ParametricSpurGear> Ptr;
		typedef osg::ref_ptr<ParametricSpurGear> CPtr;

		typedef std::map<GearParameters, ParametricSpurGear::Ptr> GearMap;

		double module() const { return params._module; }
		double angle_per_unit_depth() const { return (params._helix / params._depth);  }
		double pitchRadius() const { return _pitchDiameter/2.; }
		double depth() const { return params._depth; }
		static GearMap s_Gears; // Set of gears that can be reused
		static ParametricSpurGear::Ptr Create (short numberOfTeeth, double depth, double axisDiameter, double module, double helix);
		ParametricSpurGear(const GearParameters& params);
		~ParametricSpurGear(void);
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
