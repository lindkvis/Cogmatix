#pragma once
/** 
* Common definitions for LibCogmatics library
* @author Gaute Lindkvist <lindkvis@gmail.com>
*/

#include <stdint.h>

#include <osg/Vec3f>
#include <osg/Vec3d>
#include <osg/Matrixf>

namespace LibCogmatics
{
	// Standard typedefs
	typedef osg::Vec3f Vec;
	typedef osg::Matrixf Matrix;

	// Unicode string type
	typedef std::wstring String;

	// Node ID
	typedef uint64_t NodeID;
}