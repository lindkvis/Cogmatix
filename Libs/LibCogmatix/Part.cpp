#include "StdAfx.h"

#include <boost/numeric/interval.hpp>
#include <osg/Plane>
#include <osg/ShapeDrawable>
#include "CogException.h"
#include "Machine.h"
#include "Part.h"

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
		double axisDiameter, double module, double helix, double pitch_angle) :
		RotaryAxis(ID, axis, origin, 0., 0., 0.), _machine(machine)
{
	// Create the child
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	addChild(geode);
	ParametricSpurGear::Ptr pChild = ParametricSpurGear::Create (numberOfTeeth, depth, axisDiameter, module, helix, pitch_angle);
	geode->addDrawable(pChild);
	osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder(Vec(0., 0., 0.), axisDiameter/2., depth);
	osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(cylinder);
	geode->addDrawable(drawable);
	// Make sure we transform the gear to be perfectly aligned with the axis.
	osg::Quat quat; quat.makeRotate(Vec(0., 0., 1.), axis);
	_attitude = _attitude * quat;
	_position = _origin;
	//postMult(Matrix::rotate(Vec(0., 0., 1.), axis));

	// Assign to machine
	if (_machine)
		_machine->addGear(this);
}

ParametricSpurGearPart::~ParametricSpurGearPart()
{
}


ParametricSpurGearPart::Compatibility ParametricSpurGearPart::isCompatible(const std::set<const MachineNode*>& chain, const MachineNode* slave)
{
	if (slave == this) // A gear can not move itself
		return Self;

	if (chain.find(slave) != chain.end())
		return AlreadyMoving;
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
			else if (distance - (rPitch1+rPitch2) > PITCH_TOLERANCE || !bCompatiblePlane) // too far away
				return TooFarAway;
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

ParametricSpurGear* ParametricSpurGearPart::gear()
{
	osg::Geode* pChild = dynamic_cast<osg::Geode*>(getChild(0));
	return pChild ?
			dynamic_cast<ParametricSpurGear*>(pChild->getDrawable(0)) : nullptr;
}

const ParametricSpurGear* ParametricSpurGearPart::gear() const
{
	const osg::Geode* pChild = dynamic_cast<const osg::Geode*>(getChild(0));
	return pChild ?
			dynamic_cast<const ParametricSpurGear*>(pChild->getDrawable(0)) :
			nullptr;
}

bool ParametricSpurGearPart::move(float delta, std::set<const MachineNode*>& chain, const MachineNode* master)
{
	bool bOK=true;
	// Do some magic to move other gears
	if (_machine)
	{
		chain.insert(this);
		foreach (ParametricSpurGearPart* slave, _machine->gears())
		{
			if (bOK)
			{
				Compatibility compat = isCompatible(chain, slave);
				switch (compat)
				{
					case Compatible:
						bOK = slave->move(-delta*gear()->pitchRadius()/slave->gear()->pitchRadius(), chain, master);
						break;
					case OnAxis:
						bOK = slave->move(delta, chain, master);
						break;
					case Conflict:
						bOK = false;
						break;
					default:
						// do nothing
						break;
				}
			}
		}
	}
	// Call parent moveTo method
	RotaryAxis::moveTo(_value + delta);
	return bOK;
}

Vec ParametricSpurGearPart::worldAxis() const
{
	Vec axis = _axisVector;
	osg::MatrixList matrices = getParent(0)->getWorldMatrices();
	// a node may have multiple parents. Only deal with first one for now.
	osg::Matrixd matrix = matrices.front();
	//std::cout<< " worldMatrix = " <<matrix(0,0) << " " << matrix(1, 1) << " " << matrix(2, 2) << " " << matrix(3, 3) <<std::endl;

	axis = matrix.getRotate() * axis; // do rotation only

//	std::cout << axis[0] << " " << axis[1] << " " << axis[2] << std::endl;
	return axis;
}
Vec ParametricSpurGearPart::worldPosition() const
{
	Vec position = _origin;
	osg::MatrixList matrices = getParent(0)->getWorldMatrices();
	// a node may have multiple parents. Only deal with first one for now.
	if (!matrices.empty())
		position = position * matrices.front();
	return position;
}

osg::BoundingSphere ParametricSpurGearPart::worldBound() const
{
	return transform (getParent(0)->getWorldMatrices().front(), getBound());
}

}
