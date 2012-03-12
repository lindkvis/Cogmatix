#pragma once

#include <osg/Geode>
#include "Node.h"

namespace LibCogmatix
{
	class CompositePart : public TMachineNode<osg::Group>
	{
	public:
		typedef	osg::ref_ptr<CompositePart> Ptr;
		typedef osg::ref_ptr<const CompositePart> CPtr;
		// TODO: make this unicode
		void loadGraphics (CoString fileName);
	factory_protected:
		CompositePart(NodeID ID, CoString name, CoString fileName) : TMachineNode<osg::Group> (ID, name)
        {
            loadGraphics(fileName);
        }
            
        ~CompositePart() {}

        CompositePart() : TMachineNode<osg::Group>() {}
        CompositePart(const CompositePart& copyFrom, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) 
            : TMachineNode<osg::Group>(copyFrom, copyop)
        {
        }
        META_Node(LibCogmatix, CompositePart);
	private:
		String _fileName;
	};
}
