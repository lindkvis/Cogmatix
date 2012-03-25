#pragma once

#include <boost/units/cmath.hpp>
#include <boost/units/io.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/codata/physico-chemical_constants.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/systems/si/prefixes.hpp>

using namespace boost::units;
using namespace boost::units::si;

namespace LibCogmatix
{
	// The minimum float possible.
	static const double EPSILON = 1.0e-9;
	static const double AXIS_TOLERANCE = 1.0e-5;
	static const double PITCH_TOLERANCE = 1.0e-1;
    static const double SNAP_TOLERANCE = 1.5;

	static const double PI = 3.14159265358979323846;
}
