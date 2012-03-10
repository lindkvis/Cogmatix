#pragma once

#include <osg/Geode>

#include "node.h"

namespace LibCogmatix
{
	class CompositePart : public TMachineNode<osg::MatrixTransform>
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
	};
}
