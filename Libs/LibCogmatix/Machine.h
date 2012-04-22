#pragma once

#include <set>
#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <osgFX/Outline>
#include <boost/serialization/vector.hpp>
#include <Boost/serialization/shared_ptr.hpp>
#include <set>
#include "Node.h"

namespace LibCogmatix
{
    class Axis;
    class AxisDistance;
    typedef std::multiset<AxisDistance> AxisList;

	class Machine : public TMachineNode<osg::MatrixTransform>	
	{
        META_Node(LibCogmatix, Machine);	
    public:
		typedef osg::ref_ptr<Machine> Ptr;
		typedef osg::ref_ptr<const Machine> CPtr;
		AxisList axes(Axis* master=nullptr);
    factory_protected:
		Machine(NodeID ID, CoString name) : TMachineNode<osg::MatrixTransform>(ID, name) {}
        Machine(const Machine& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) 
            : TMachineNode<osg::MatrixTransform>(copyFrom, copyop)
        {
        }
		~Machine(void) {}
    private:
        Machine() : TMachineNode<osg::MatrixTransform>() {}
	};
}
