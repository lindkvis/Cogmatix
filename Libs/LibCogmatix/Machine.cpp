#include "StdAfx.h"
#include "Machine.h"

namespace LibCogmatix
{
	Machine::Machine(NodeID ID, CoString name) : NamedMachineNode(ID, name)
	{
	}

	Machine::~Machine(void)
	{
	}

	void Machine::toggleSelection(osg::Node* node)
	{
		clearSelection();
		addToSelection(node);
	}

	void Machine::addToSelection(osg::Node* node)
	{
		osg::StateSet* gearState = node->getOrCreateStateSet();
		gearState->setAttributeAndModes(_selectionShader, osg::StateAttribute::ON);
		_selection.push_back(node);
	}

	void Machine::clearSelection()
	{
		std::for_each(allof(_selection), [this](osg::Node* selected)
		{
			osg::StateSet* gearState = selected->getOrCreateStateSet();
		//	gearState->removeAttribute(_selectionShader);
		});
		_selection.clear();
		
		
	}
}