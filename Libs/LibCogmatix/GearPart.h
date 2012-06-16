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
    META_Node(LibCogmatix, ParametricSpurGearPart);
public:
	typedef osg::ref_ptr<ParametricSpurGearPart> Ptr;
	typedef osg::ref_ptr<const ParametricSpurGearPart> CPtr;
	virtual Compatibility isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave) const;
	virtual bool snapTo (const MachineNode* master);
    virtual bool snapTo ();
	ParametricSpurGear* gear();
	const ParametricSpurGear* gear() const;
    virtual bool move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked);

factory_protected:
	ParametricSpurGearPart(NodeID ID, CoString name, Machine* machine,
			const Vec& axis, const Vec& origin, short numberOfTeeth, double depth,
			double axisDiameter, double module, double helix, double pitch_angle, Vec4 colour);
    ParametricSpurGearPart(const ParametricSpurGearPart& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);    
    ParametricSpurGearPart() : RotaryAxis() {}
	~ParametricSpurGearPart();
protected:
	Machine* _machine;
    void init(const ParametricSpurGear::GearParameters& params);
};

typedef std::set<ParametricSpurGearPart*> GearSet;
}
