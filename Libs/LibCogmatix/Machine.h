#pragma once

#include <set>
#include <osg/MatrixTransform>
#include <osgFX/Outline>
#include <boost/serialization/vector.hpp>
#include <Boost/serialization/shared_ptr.hpp>

#include "Node.h"
#include "Part.h"

namespace LibCogmatix
{
	class Machine : public NamedMachineNode, public osg::MatrixTransform	
	{
	protected:
		GearSet _gears;
	factory_protected:
		Machine(NodeID ID, CoString name);
		~Machine(void);
	public:
		typedef osg::ref_ptr<Machine> Ptr;
		typedef osg::ref_ptr<const Machine> CPtr;
		// TODO: May want to just search the tree
		void addGear(ParametricSpurGearPart* gear) { _gears.insert(gear); }
		const GearSet& gears() const { return _gears; }
		void removeGear(ParametricSpurGearPart* gear) { _gears.erase(gear); }
		void clearGears() { _gears.clear(); }
	};
}
