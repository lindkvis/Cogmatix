#pragma once

#include <set>
#include <osg/Geode>

#include "Axis.h"
#include "Node.h"
#include "ParametricSpurGear.h"

namespace LibCogmatix
{
class Machine;
// forward declaration

class ParametricSpurGearPart: public RotaryAxis
{
public:
	enum Compatibility
	{
		Compatible=0,
		Self=1,
		AlreadyMoving=2,
		TooFarAway=3,
		Conflict=4
	};
	typedef osg::ref_ptr<ParametricSpurGearPart> Ptr;
	typedef osg::ref_ptr<const ParametricSpurGearPart> CPtr;
	Compatibility isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave);
	ParametricSpurGear* gear();
	const ParametricSpurGear* gear() const;
	virtual bool move(float delta, std::set<const MachineNode*>& chain, const MachineNode* master);
	// Get the world axis of the spur gear
	Vec worldAxis() const;
	// Get the world position of the spur gear
	Vec worldPosition() const;
factory_protected:
	ParametricSpurGearPart(NodeID ID, CoString name, Machine* machine,
			const Vec& axis, const Vec& origin, short numberOfTeeth, double depth,
			double axisDiameter, double module, double helix);
	~ParametricSpurGearPart();
protected:
	Machine* _machine;
};

typedef std::set<ParametricSpurGearPart*> GearSet;
}
