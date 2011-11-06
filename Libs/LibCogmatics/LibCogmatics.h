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

#include <boost/archive/xml_wiarchive.hpp>
#include <boost/archive/xml_woarchive.hpp>
#include <boost/serialization/string.hpp> 
#include <boost/serialization/version.hpp>

using boost::serialization::make_nvp;


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
}

namespace boost {
	namespace serialization {

		template<class Archive>
		void serialize(Archive & ar, LibCogmatics::Vec & v, const unsigned int version)
		{
			ar & make_nvp("x", v[0]);
			ar & make_nvp("y", v[1]);
			ar & make_nvp("z", v[2]);
		}
	}
}
