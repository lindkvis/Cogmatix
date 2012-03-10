#pragma once

#include <set>
#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <osgFX/Outline>
#include <boost/serialization/vector.hpp>
#include <Boost/serialization/shared_ptr.hpp>
#include "Node.h"

namespace LibCogmatix
{
    class ParametricSpurGearPart;
    typedef std::set<ParametricSpurGearPart*> GearSet;

	class Machine : public TMachineNode<osg::MatrixTransform>	
	{
	protected:
		GearSet _gears;
	public:
		typedef osg::ref_ptr<Machine> Ptr;
		typedef osg::ref_ptr<const Machine> CPtr;
		// TODO: May want to just search the tree
		void addGear(ParametricSpurGearPart* gear) { _gears.insert(gear); }
		const GearSet& gears() const { return _gears; }
		void removeGear(ParametricSpurGearPart* gear) { _gears.erase(gear); }
		void clearGears() { _gears.clear(); }
    factory_protected:
		Machine(NodeID ID, CoString name) : TMachineNode<osg::MatrixTransform>(ID, name) {}
		~Machine(void) {}
	};
}
