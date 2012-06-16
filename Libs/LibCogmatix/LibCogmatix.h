#pragma once
/** 
* Common definitions for LibCogmatix library
* @author Gaute Lindkvist <lindkvis@gmail.com>
*/

#include <stdint.h>

#include <osg/Vec3f>
#include <osg/Vec3d>
#include <osg/Matrixf>
#include <osg/BoundingSphere>
#include <osg/Group>
#include <Boost/foreach.hpp>

#ifdef __APPLE__
const                        // this is a const object...
class {
public:
    template<class T>          // convertible to any type
    operator T*() const      // of null non-member
    { return 0; }            // pointer...
    template<class C, class T> // or any type of null
    operator T C::*() const  // member pointer...
    { return 0; }
private:
    void operator&() const;    // whose address can't be taken
} nullptr = {}; 
#endif

namespace LibCogmatix
{
    class Machine;
    
    // Shadow masks
    const int ReceivesShadowTraversalMask = 0x1;
    const int CastsShadowTraversalMask = 0x2;

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

    osg::ref_ptr<Machine> createTestMachine(osg::Group* root);
    osg::ref_ptr<osg::Geode> createBase(const osg::Vec3f& center,float radius);
    
    osg::ref_ptr<osg::Group> createShadowedScene(osg::ref_ptr<Machine> machine, osg::ref_ptr<osg::Node> shadowee,osg::ref_ptr<osg::Node> shadowed,const osg::Vec3& lightPosition);
    bool loadShaderSource(osg::Shader* obj, const std::string& fileName );
	osg::BoundingSphere transformBoundingSphere( const osg::Matrixf& m, const osg::BoundingSphere& sphere );
    
    void getEulerFromQuat(osg::Quat q, double& a, double& b, double& c); 
    
    template<class T>
    T between (const T& minT, const T& maxT, const T& x)
    {
        return std::min (maxT, std::max(minT, x));
    } 
    
    static Vec4 red (1.0, 0.0, 0.0, 0.4);
    static Vec4 blue (0., 0., 1., 0.4);
    static Vec4 green(0., 1., 0.2, 0.4);
    static Vec4 yellow (1., 1., 0.0, 0.4);
    static Vec4 purple (0.65, 0., 1., 0.4);
    
}
