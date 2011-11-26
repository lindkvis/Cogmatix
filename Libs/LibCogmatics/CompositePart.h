#pragma once

#include <osg/Geode>

#include "node.h"

namespace LibCogmatics
{
	class CompositePart : public NamedMachineNode, public osg::MatrixTransform
	{
	public:
		typedef	osg::ref_ptr<CompositePart> Ptr;
		typedef osg::ref_ptr<const CompositePart> CPtr;
		// TODO: make this unicode
		void loadGraphics (CoString fileName);
	factory_protected:
		CompositePart(NodeID ID, CoString name, CoString fileName);
		~CompositePart();
	private:
		String _fileName;
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(NamedMachineNode);
			ar & BOOST_SERIALIZATION_NVP(_fileName);
		}
	};
}