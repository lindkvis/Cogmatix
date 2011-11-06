#pragma once
/** 
* Common definitions for LibCogmatics library
* @author Gaute Lindkvist <lindkvis@gmail.com>
*/

#include <stdint.h>

#include <osg/Vec3f>
#include <osg/Vec3d>
#include <osg/Matrixf>
#include <Boost/foreach.hpp>

namespace LibCogmatics
{
	// Standard typedefs
	typedef osg::Vec3f Vec;
	typedef osg::Matrixf Matrix;

	// Unicode string type
	typedef std::string String;
	typedef const String& CoString;

	// Node ID
	typedef uint64_t NodeID;
	
#define foreach BOOST_FOREACH
	//#define allof (container) container.begin(), container.end()
}
