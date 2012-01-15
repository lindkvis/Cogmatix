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
		TooFarAway=1,
		Conflict=2
	};
	typedef osg::ref_ptr<ParametricSpurGearPart> Ptr;
	typedef osg::ref_ptr<const ParametricSpurGearPart> CPtr;
	Compatibility isCompatible(const ParametricSpurGearPart* part);
	ParametricSpurGear* gear();
	const ParametricSpurGear* gear() const;
	virtual bool move(float delta);
	virtual bool moveSecondary(float delta, const ParametricSpurGearPart* master);
	// Get the world axis of the spur gear
	Vec worldAxis() const;
	// Get the world position of the spur gear
	Vec worldPosition() const;
factory_protected:
	ParametricSpurGearPart(NodeID ID, CoString name, Machine* machine,
			const Vec& axis, short numberOfTeeth, double depth,
			double axisDiameter, double module, double helix);
	~ParametricSpurGearPart();
protected:
	Machine* _machine;
};

typedef std::set<ParametricSpurGearPart*> GearSet;
}
