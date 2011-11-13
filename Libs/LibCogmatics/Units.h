// Boost.Units - A C++ library for zero-overhead dimensional analysis and 
// unit/quantity manipulation and conversion
//
// Copyright (C) 2003-2008 Matthias Christian Schabel
// Copyright (C) 2008 Steven Watanabe
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <boost/units/cmath.hpp>
#include <boost/units/io.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/codata/physico-chemical_constants.hpp>
#include <boost/units/systems/si/io.hpp>
#include <boost/units/systems/si/prefixes.hpp>

#include "LibCogmatics.h"

using namespace boost::units;
using namespace boost::units::si;

namespace LibCogmatics
{
	typedef quantity<length> Length;
	typedef quantity<plane_angle> Angle;

	// The minimum float possible.
	static const Length epsilon (1.0 * nano * meter);

	static const double pi = 3.14159265358979323846;
}
