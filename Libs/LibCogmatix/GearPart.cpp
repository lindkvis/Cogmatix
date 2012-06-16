#include "StdAfx.h"
#define __USE_ISOC99
#include <boost/numeric/interval.hpp>
#undef __USE_ISOC99
#include <osg/Plane>
#include <osg/ShapeDrawable>
#include "CogException.h"
#include "Machine.h"
#include "GearPart.h"


using namespace boost::numeric;
using namespace interval_lib;

// Create an interval type that doesn't throw exceptions for empty intervals
typedef boost::numeric::interval_lib::rounded_math< double > rounding_policy;
typedef boost::numeric::interval_lib::checking_no_nan< double > checking_policy;
typedef boost::numeric::interval_lib::policies< rounding_policy, checking_policy > interval_policies;
typedef boost::numeric::interval< double, interval_policies > interval_double;

namespace LibCogmatix
{
    /****************************
     * Parametric spur gear part *
     *****************************/
    ParametricSpurGearPart::ParametricSpurGearPart(NodeID ID, CoString name,
                                                   Machine* machine, const Vec& axis, const Vec& origin, short numberOfTeeth, double depth,
                                                   double axisDiameter, double module, double helix, double pitch_angle, Vec4 colour) 
    : RotaryAxis(ID, axis, origin, axisDiameter, depth*2, 0., 0., 0.), _machine(machine)
    {
        ParametricSpurGear::GearParameters params (numberOfTeeth, depth, axisDiameter, module, helix, pitch_angle, colour);
        init(params);
    }
 
    ParametricSpurGearPart::ParametricSpurGearPart(const ParametricSpurGearPart& copyFrom, const osg::CopyOp& copyop)
    : RotaryAxis (copyFrom.ID(), copyFrom.vector(), copyFrom.origin(), copyFrom.diameter(), copyFrom.length(), 0., 0., 0.), _machine(copyFrom._machine) 
    {
        init(copyFrom.gear()->getParams());
    }
    
    void ParametricSpurGearPart::init(const ParametricSpurGear::GearParameters& params)
    {
        setNodeMask(CastsShadowTraversalMask);
        ParametricSpurGear::Ptr pChild = ParametricSpurGear::Create (params);
        // Create the child
        _geode->addDrawable(pChild);
        // Make sure we transform the gear to be perfectly aligned with the axis.
        reset();
        snapTo();
        assert (gear());
    }
    
    ParametricSpurGearPart::~ParametricSpurGearPart()
    {
    }
    
    
    ParametricSpurGearPart::Compatibility ParametricSpurGearPart::isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave) const
    {
        if (slave == this) // A gear can not move itself
            return Self;
        
        Compatibility rotaryAxisCompatibility = RotaryAxis::isCompatible(chain, slave);
        // Check axis compatibility first
        if (rotaryAxisCompatibility != MachineNode::NotDetermined)
            return rotaryAxisCompatibility;
        
        const ParametricSpurGearPart* part = dynamic_cast<const ParametricSpurGearPart*>(slave);
        if (part)
        {
            bool bCompatibleModule = fabs(gear()->module() - part->gear()->module())
            < EPSILON;
            bool bCompatibleHelixAngle = fabs(
                                              gear()->helixAnglePerUnitDepth()
                                              - part->gear()->helixAnglePerUnitDepth()) < EPSILON;
            Vec axisOwn = worldAxis();
            Vec axisOther = part->worldAxis();
            bool bCompatiblePitchAngle = true;
            bool bCompatibleAxis = false;
            if (fabs(gear()->pitchAngle() - PI/2.) < EPSILON)
            {
                bCompatiblePitchAngle = fabs(part->gear()->pitchAngle() - PI/2.) < EPSILON;
                bCompatibleAxis = (axisOwn - axisOther).length() < EPSILON;
                // Positions along the world axis
                double n1val = worldPosition() * worldAxis();
                double n2val = part->worldPosition() * part->worldAxis();
                // Intervals along the world axis
                interval_double planes1(n1val - gear()->depth(), n1val);
                interval_double planes2(n2val - part->gear()->depth(), n2val);
                interval_double overlap = intersect(planes1, planes2);
                bool bCompatiblePlane = width(overlap) > EPSILON;
                // Positions in plane (2D)
                Vec posOwn = worldPosition() - axisOwn * n1val;
                Vec posOther = part->worldPosition() - axisOther * n2val;
                
                // total distance between them
                double distance = (posOwn - posOther).length();
                double rPitch1=gear()->pitchRadius();
                double rPitch2=part->gear()->pitchRadius();
                
                if (bCompatibleAxis && distance < AXIS_TOLERANCE && !bCompatiblePlane && (worldPosition() - part->worldPosition()).length() < 3.*gear()->depth()) // on same axis
                    return OnAxis;
                else if (distance - (rPitch1+rPitch2) < -PITCH_TOLERANCE && bCompatiblePlane) // too close
                    return Conflict;
                else if (!bCompatiblePlane)
                    return DifferentPlane;
                else if (bCompatibleAxis && (distance - (rPitch1+rPitch2) > PITCH_TOLERANCE)) // too far away
                {
                    if (distance - (rPitch1+rPitch2) < SNAP_TOLERANCE)
                        return CanSnapTo;
                    else
                        return TooFarAway;
                }
            }
            else if (fabs(gear()->pitchAngle() - PI/4.) < EPSILON && fabs(part->gear()->pitchAngle() - PI/4.) < EPSILON)
            {
                bCompatibleAxis = fabs(axisOwn * axisOther) < EPSILON;
                bCompatiblePitchAngle = true;
                
                osg::Plane planeOwn(axisOwn, worldPosition());
                osg::Plane planeOther(axisOther, part->worldPosition());
                osg::BoundingSphere sphereOwn = worldBound();
                osg::BoundingSphere sphereOther = part->worldBound();
                osg::BoundingSphere sphereOwnLarge (sphereOwn._center, sphereOwn._radius*1.025);
                osg::BoundingSphere sphereOwnSmall (sphereOwn._center, sphereOwn._radius*0.975);
                osg::BoundingSphere sphereOtherLarge (sphereOther._center, sphereOther._radius*1.025);
                osg::BoundingSphere sphereOtherSmall (sphereOther._center, sphereOther._radius*0.975);
                
                if (planeOwn.intersect(sphereOtherSmall)==0 || planeOther.intersect(sphereOwnSmall))
                    return Conflict; // too close
                else if (planeOwn.intersect(sphereOtherLarge)!=0 || planeOther.intersect(sphereOwnLarge)!=0)
                    return TooFarAway; // too far away
            }
            
            return (bCompatiblePitchAngle && bCompatibleModule && bCompatibleHelixAngle && bCompatibleAxis) ? Compatible : Conflict;
        }
        return TooFarAway; // not correct, but we don't have any other types of gears yet.
    }
        
    bool ParametricSpurGearPart::snapTo()
    {
        assert (_machine);
        AxisList axes = _machine->axes(this);

        for (AxisList::const_iterator it = axes.begin(); it != axes.end(); ++it)
        {
            const AxisDistance& dist = (*it);
            Compatibility compat = dist.axis->isCompatible(std::set<const MachineNode*>(), this);
            if (compat==Compatible || compat == CanSnapTo || compat == Conflict)
            {
                return snapTo(dist.axis);
            }
        }
        return false;
    }
    
    bool ParametricSpurGearPart::snapTo (const MachineNode* masterNode)
    {

        const ParametricSpurGearPart* master = dynamic_cast<const ParametricSpurGearPart*>(masterNode);
        if (!master)
            return RotaryAxis::snapTo(masterNode);
        Compatibility compat = masterNode->isCompatible(std::set<const MachineNode*>(), this);
        if (compat == Compatible || compat == CanSnapTo || compat == Conflict)
        {
            Vec axisOwn = worldAxis();
            Vec axisOther = master->worldAxis();
            // Positions along the world axis
            double n1val = worldPosition() * axisOwn;
            double n2val = master->worldPosition() * axisOther;
            
            // Position vectors
            Vec posOwn = worldPosition() - axisOwn * n1val;
            Vec posOther = master->worldPosition() - axisOther * n2val;
            Vec vecDist = posOther - posOwn;
            // total distance between them
            double distance = vecDist.length();
            if (distance < EPSILON)
            {
                vecDist = Vec(1., 0., 0.);
            }
            vecDist.normalize();
            double rPitch1=gear()->pitchRadius();
            double rPitch2=master->gear()->pitchRadius();
            
            osg::Matrixd masterM = master->worldMatrix();
            osg::Matrixd masterMI = osg::Matrixd::inverse(masterM);
            
            osg::Matrixd M = worldMatrix();
            osg::Matrixd MI = osg::Matrixd::inverse(M);
            
            Vec newPosOwn = worldPosition() + vecDist * (distance - (rPitch1+rPitch2));
            setOrigin (newPosOwn * MI);
            reset();
           
            // now deal with angles. But first convert orientation vector to local XY coordinates
            osg::Quat masterAttitude = master->getAttitude().inverse();
            osg::Quat slaveAttitude = getAttitude().inverse();
            double toothRatioMaster = master->gear()->toothRatio(masterAttitude * (vecDist * masterMI), _value);
            double angleSlave = gear()->angleFromRatio(toothRatioMaster, slaveAttitude * (-vecDist * MI), _value);
            
            osg::Quat qRot;
            qRot.makeRotate (angleSlave, _axisVector);
            _attitude = _attitude * qRot;
            return true;
        }
        return false;
    }
    
    ParametricSpurGear* ParametricSpurGearPart::gear()
    {
        return findChildOfType<ParametricSpurGear>(this);
    }
    
    const ParametricSpurGear* ParametricSpurGearPart::gear() const
    {
        return findChildOfType<const ParametricSpurGear>(this);
    }
    
    bool ParametricSpurGearPart::move (float delta, std::set<const MachineNode*>& chain, const MachineNode* master, bool blocked)
    {
        float my_delta=delta;
        const ParametricSpurGearPart* gear_master = dynamic_cast<const ParametricSpurGearPart*>(master);
        if (gear_master)
            my_delta *= -(gear_master->gear()->pitchRadius()/gear()->pitchRadius());
        return RotaryAxis::move(my_delta, chain, master, blocked);
    }
    
}
