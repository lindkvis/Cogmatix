#include "StdAfx.h"
#include "Factory.h"
#include "Axis.h"
#include "CompositePart.h"
#include "Light.h"
#include "Machine.h"
#include "Motor.h"
#include "GearPart.h"
#include "ParametricSpurGear.h"

namespace LibCogmatix
{
	// Initialise the static pointer.
	Factory* Factory::m_pInstance = nullptr;

	Factory* Factory::get()
	{
		if (nullptr==m_pInstance)
			m_pInstance = new Factory();
		return m_pInstance;
	}
    
    bool Factory::removeNode(NodeID ID)
    {
        osg::Node* node = dynamic_cast<osg::Node*>(getNode(ID));
        if (!node) return false;
        osg::Node::ParentList parents = node->getParents();
        foreach (osg::Group* parent, parents)
        {
            parent->removeChild(node);
        }
        _nodeDB.erase(ID);
        return true;
    }
    
    NodeID Factory::copyNode(NodeID copyID)
    {
        osg::Node* oldNode = dynamic_cast<osg::Node*>(getNode(copyID));
        if (oldNode)
        {
            osg::Node* newNode = dynamic_cast<osg::Node*>(oldNode->clone(osg::CopyOp::DEEP_COPY_OBJECTS));
            MachineNode* machineNode = dynamic_cast<MachineNode*>(newNode);
            const MachineNode* oldMachineNode = dynamic_cast<const MachineNode*>(oldNode);
            if (machineNode && oldNode->getNumParents() > 0)
            {
                machineNode->setID(_nextID);
                _nodeDB[_nextID++] = newNode;
                osg::Group* group = dynamic_cast<osg::Group*> (oldNode->getParent(0));
                if (group)
                    group->addChild(newNode);
                machineNode->snapTo(oldMachineNode);
                return machineNode->ID();
            }
        }
        return -1;
    }

	LinearAxis::Ptr Factory::CreateLinearAxis(const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max)
	{
        LinearAxis::Ptr axis (new LinearAxis (_nextID, axisVector, origin, valueInitial, min, max));
        _nodeDB[_nextID++] = PCAST(osg::Object, axis);
		return axis;
	}

	RotaryAxis::Ptr Factory::CreateRotaryAxis(const Vec& axisVector, const Vec& origin, double axisDiameter, double axisLength, float valueInitial, float min, float max)
	{
		RotaryAxis::Ptr axis (new RotaryAxis (_nextID, axisVector, origin, axisDiameter, axisLength, valueInitial, min, max));
        _nodeDB[_nextID++] = PCAST(osg::Object, axis);
        return axis;
	}

	Machine::Ptr Factory::CreateMachine(CoString name)
	{
		Machine::Ptr machine (new Machine (_nextID, name));
        _nodeDB[_nextID++] = PCAST(osg::Object, machine);
        return machine;
	}

	CompositePart::Ptr Factory::CreateCompositePart(CoString name, CoString fileName)
	{
		CompositePart::Ptr part (new CompositePart (_nextID, name, fileName));
        _nodeDB[_nextID++] = PCAST(osg::Object, part);
        return part;
	}

	ParametricSpurGearPart::Ptr Factory::CreateParametricSpurGearPart(CoString name, Machine* machine, const Vec& axis, const Vec& origin,
			short numberOfTeeth, double depth, double axisDiameter, double module, double helix, double pitch_angle, Vec4 colour)
	{
		ParametricSpurGearPart::Ptr gear (new ParametricSpurGearPart (_nextID, name, machine, axis, origin, numberOfTeeth, depth, axisDiameter, module, helix, pitch_angle, colour));
        _nodeDB[_nextID++] = PCAST(osg::Object, gear); 
		return gear;
	}

	Light::Ptr Factory::CreateLight(Machine* machine, const Vec& position, const Vec4& colour)
	{
		Light::Ptr light (new Light(_nextID, machine, position, colour));
        _nodeDB[_nextID++] = PCAST(osg::Object, light);
        return light;
	}

	Motor::Ptr Factory::CreateMotor(double RPM, const Vec& axisVector, const Vec& origin, double axisDiameter, double axisLength)
	{
		Motor::Ptr motor (new Motor(_nextID, RPM, axisVector, origin, axisDiameter, axisLength));
        _nodeDB[_nextID++] = PCAST(osg::Object, motor);
        return motor;
	}
    
    BoxMotor::Ptr Factory::CreateBoxMotor(double RPM, const Vec& axisVector, const Vec& origin, Vec boxWidths, double axisDiameter, double axisLength)
    {
        BoxMotor::Ptr motor (new BoxMotor(_nextID, RPM, axisVector, origin, boxWidths, axisDiameter, axisLength));
        _nodeDB[_nextID++] = PCAST(osg::Object, motor);
        return motor;
    }
}
