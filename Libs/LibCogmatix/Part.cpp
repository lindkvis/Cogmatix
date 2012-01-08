#include "StdAfx.h"
#include "CogException.h"
#include "Machine.h"
#include "Part.h"

namespace LibCogmatix
{
	Part::Part(NodeID ID, CoString name) : NamedMachineNode(ID, name)
	{
	}


	Part::~Part(void)
	{
	}

	/****************************
	* Parametric spur gear part *
	*****************************/
	ParametricSpurGearPart::ParametricSpurGearPart(NodeID ID, CoString name, Machine* machine, const Vec& axis, short numberOfTeeth, double depth, double axisDiameter, double module, double helix)
		: RotaryAxis (ID, axis, Vec(0., 0., 0.), 0., 0., 0.), _machine(machine)
	{
		// Create the child
		osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		addChild(geode);
		ParametricSpurGear::Ptr pChild = ParametricSpurGear::Ptr(new ParametricSpurGear(numberOfTeeth, depth, axisDiameter, module, helix));
		geode->addDrawable(pChild);
		postMult(Matrix::rotate(Vec(0., 0., 1.), axis));

		// Assign to machine	
		if (_machine)
			_machine->addGear(this);
	}


	ParametricSpurGearPart::~ParametricSpurGearPart()
	{
	}

	bool ParametricSpurGearPart::isCompatible(const ParametricSpurGearPart* part)
	{
		return fabs(gear()->module() - part->gear()->module()) < epsilon 
			&& fabs(gear()->angle_per_unit_depth() - part->gear()->angle_per_unit_depth()) < epsilon;
	}

	ParametricSpurGear* ParametricSpurGearPart::gear()
	{
		osg::Geode* pChild = dynamic_cast<osg::Geode*>(getChild(0));
		return pChild ? dynamic_cast<ParametricSpurGear*>(pChild->getDrawable(0)) : nullptr;
	}

	const ParametricSpurGear* ParametricSpurGearPart::gear() const
	{
		const osg::Geode* pChild = dynamic_cast<const osg::Geode*>(getChild(0));
		return pChild ? dynamic_cast<const ParametricSpurGear*>(pChild->getDrawable(0)) : nullptr;
	}

	bool ParametricSpurGearPart::move(float delta)
	{
		// Do some magic to move other gears
		if (_machine)
		{
			const GearSet& gears = _machine->gears();
			std::for_each(allof(gears), [delta, this](ParametricSpurGearPart* gear)
			{
				// don't move self
				if (gear != this && isCompatible(gear))
				{
					// Wrong. Currently moving the same distance as the original gear.
					gear->moveSecondary(-delta, this);
				}
			});
		}
		// Call parent move method
		return RotaryAxis::move(delta);
	}

	bool ParametricSpurGearPart::moveSecondary(float delta, const ParametricSpurGearPart* master)
	{
		// find other gears to move
		if (_machine)
		{
			const GearSet& gears = _machine->gears();
			std::for_each(allof(gears), [master, delta, this](ParametricSpurGearPart* gear)
			{
				// Don't move self or master
				if (gear != this && gear != master && isCompatible(gear))
				{
					// Wrong. Currently moving the same distance as the original gear.
					gear->moveSecondary(-delta, this);
				}
			});
		}
		// move self
		return RotaryAxis::move(delta);
	}
}
