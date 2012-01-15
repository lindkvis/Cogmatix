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
	typedef osg::ref_ptr<ParametricSpurGearPart> Ptr;
	typedef osg::ref_ptr<const ParametricSpurGearPart> CPtr;
	bool isCompatible(const ParametricSpurGearPart* part);
	ParametricSpurGear* gear();
	const ParametricSpurGear* gear() const;
	virtual bool move(float delta);
	virtual bool moveSecondary(float delta, const ParametricSpurGearPart* master);
	// Get the world position of the spur gear
	Vec getPosition() const;
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
