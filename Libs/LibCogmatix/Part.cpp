#include "StdAfx.h"

#include <boost/numeric/interval.hpp>

#include "CogException.h"
#include "Machine.h"
#include "Part.h"

using namespace boost::numeric;
using namespace interval_lib;

namespace LibCogmatix
{
/****************************
 * Parametric spur gear part *
 *****************************/
ParametricSpurGearPart::ParametricSpurGearPart(NodeID ID, CoString name,
		Machine* machine, const Vec& axis, short numberOfTeeth, double depth,
		double axisDiameter, double module, double helix) :
		RotaryAxis(ID, axis, Vec(0., 0., 0.), 0., 0., 0.), _machine(machine)
{
	// Create the child
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	addChild(geode);
	ParametricSpurGear::Ptr pChild = ParametricSpurGear::Ptr(
			new ParametricSpurGear(numberOfTeeth, depth, axisDiameter, module,
					helix));
	geode->addDrawable(pChild);
	postMult(Matrix::rotate(Vec(0., 0., 1.), axis));

	// Assign to machine
	if (_machine)
		_machine->addGear(this);
}

ParametricSpurGearPart::~ParametricSpurGearPart()
{
}


ParametricSpurGearPart::Compatibility ParametricSpurGearPart::isCompatible(const ParametricSpurGearPart* part)
{
	bool bCompatibleModule = fabs(gear()->module() - part->gear()->module())
			< epsilon;
	bool bCompatibleHelixAngle = fabs(
			gear()->angle_per_unit_depth()
					- part->gear()->angle_per_unit_depth()) < epsilon;
	Vec axisOwn = worldAxis();
	Vec axisOther = part->worldAxis();
	bool bCompatibleAxis = (axisOwn - axisOther).length() < epsilon;
	// Positions along the world axis
	double n1val = worldPosition() * worldAxis();
	double n2val = part->worldPosition() * part->worldAxis();
	// Intervals along the world axis
	interval<double> planes1(n1val - gear()->depth(), n1val);
	interval<double> planes2(n2val - part->gear()->depth(), n2val);
	interval<double> overlap = intersect(planes1, planes2);
	bool bCompatiblePlane = width(overlap) > epsilon;
	// Positions in plane (2D)
	Vec posOwn = worldPosition() - axisOwn * n1val;
	Vec posOther = part->worldPosition() - axisOther * n2val;

	// total distance between them
	double distance = (posOwn - posOther).length();
	double rPitch1=gear()->pitchRadius();
	double rPitch2=part->gear()->pitchRadius();

	if (distance - (rPitch1+rPitch2) < -pitch_tolerance && bCompatiblePlane) // too close
		return Conflict;
	else if (distance - (rPitch1+rPitch2) > pitch_tolerance || !bCompatiblePlane) // too far away
		return TooFarAway;
	else // optimal distance
	{
		return (bCompatibleModule && bCompatibleHelixAngle && bCompatibleAxis) ? Compatible : Conflict;
	}
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

bool ParametricSpurGearPart::move(float delta)
{
	// Do some magic to move other gears
	if (_machine)
	{
		const GearSet& gears = _machine->gears();
		std::for_each(allof(gears), [delta, this](ParametricSpurGearPart* slave)
		{
			// don't move self
			if (slave != this && isCompatible(slave)==ParametricSpurGearPart::Compatible)
			{
				slave->moveSecondary(-delta*gear()->pitchRadius()/slave->gear()->pitchRadius(), this);
			}
		});
	}
	// Call parent move method
	return RotaryAxis::move(delta);
}

bool ParametricSpurGearPart::moveSecondary(float delta,
		const ParametricSpurGearPart* master)
{
	// find other gears to move
	if (_machine)
	{
		const GearSet& gears = _machine->gears();
		std::for_each(allof(gears), [master, delta, this](ParametricSpurGearPart* slave)
		{
			// Don't move self or master
			if (slave != this && slave != master && isCompatible(slave)==ParametricSpurGearPart::Compatible)
			{
				// Wrong. Currently moving the same distance as the original gear.
				slave->moveSecondary(-delta*gear()->pitchRadius()/slave->gear()->pitchRadius(), this);
			}
		});
	}
	// move self
	return RotaryAxis::move(delta);
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
	Vec position; // start at 0,0,0
	osg::MatrixList matrices = getParent(0)->getWorldMatrices();
	// a node may have multiple parents. Only deal with first one for now.
	if (!matrices.empty())
		position = position * matrices.front();
	return position;
}
}
