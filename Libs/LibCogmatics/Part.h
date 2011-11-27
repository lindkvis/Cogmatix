#pragma once

#include <osg/Geode>

#include "Node.h"
#include "ParametricSpurGear.h"

namespace LibCogmatics
{
	class Part : public NamedMachineNode, public osg::Geode
	{
	public:
		typedef osg::ref_ptr<Part> Ptr;
		typedef osg::ref_ptr<const Part> CPtr;

	factory_protected:
		Part(NodeID ID, CoString name);
		~Part();
	private:
		friend class boost::serialization::access;
	};

	class ParametricSpurGearPart : public Part
	{
	public:
		typedef osg::ref_ptr<ParametricSpurGearPart> Ptr;
		typedef osg::ref_ptr<const ParametricSpurGearPart> CPtr;

	factory_protected:
		ParametricSpurGearPart(NodeID ID, CoString name, short numberOfTeeth, Length depth, Length axisDiameter, Length module, Angle helixAngle);
		~ParametricSpurGearPart();
	private:
		friend class boost::serialization::access;
	};
}
