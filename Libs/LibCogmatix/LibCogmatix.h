#pragma once
/** 
* Common definitions for LibCogmatix library
* @author Gaute Lindkvist <lindkvis@gmail.com>
*/

#include <stdint.h>

#include <osg/Vec3f>
#include <osg/Vec3d>
#include <osg/Matrixf>
#include <Boost/foreach.hpp>

namespace LibCogmatix
{
	// Standard typedefs
	typedef osg::Vec3f Vec;
	typedef osg::Vec4f Vec4;
	typedef osg::Matrixf Matrix;

	// Unicode string type
	typedef std::string String;
	typedef const String& CoString;

	// Node ID
	typedef uint64_t NodeID;
#define foreach BOOST_FOREACH
#define factory_protected friend class Factory;protected
#define allof(container) container.begin(), container.end()

#define PCAST(type, object) osg::dynamic_pointer_cast<type>(object)
#define PNULL(type) osg::ref_ptr<type> ()

						
}
