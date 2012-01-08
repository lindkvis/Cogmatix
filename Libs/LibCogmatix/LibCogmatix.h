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

	static osg::BoundingSphere transform( const osg::Matrix& m, const osg::BoundingSphere& sphere )
	{
	    osg::BoundingSphere::vec_type xdash = sphere._center;
	    xdash.x() += sphere._radius;
	    xdash = xdash * m;

	    osg::BoundingSphere::vec_type ydash = sphere._center;
	    ydash.y() += sphere._radius;
	    ydash = ydash * m;

	    osg::BoundingSphere::vec_type zdash = sphere._center;
	    zdash.z() += sphere._radius;
	    zdash = zdash * m;

	    osg::BoundingSphere newSphere;
	    newSphere._center = sphere._center * m;

	    xdash -= newSphere._center;
	    osg::BoundingSphere::value_type len_xdash = xdash.length();

	    ydash -= newSphere._center;
	    osg::BoundingSphere::value_type len_ydash = ydash.length();

	    zdash -= newSphere._center;
	    osg::BoundingSphere::value_type len_zdash = zdash.length();

	    newSphere._radius = len_xdash;
	    if( newSphere._radius < len_ydash )
	        newSphere._radius = len_ydash;
	    if( newSphere._radius < len_zdash )
	        newSphere._radius = len_zdash;

	    return( newSphere );
	}
}
