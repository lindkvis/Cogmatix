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
		OnAxis=1, // driven by gears axis.
		Self=2,
		AlreadyMoving=3,
		TooFarAway=4,
		Conflict=5
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
	// Get the world bounding sphere of the spur gear
	osg::BoundingSphere worldBound() const;
factory_protected:
	ParametricSpurGearPart(NodeID ID, CoString name, Machine* machine,
			const Vec& axis, const Vec& origin, short numberOfTeeth, double depth,
			double axisDiameter, double module, double helix, double pitch_angle);
	~ParametricSpurGearPart();
protected:
	Machine* _machine;
};

typedef std::set<ParametricSpurGearPart*> GearSet;
}
