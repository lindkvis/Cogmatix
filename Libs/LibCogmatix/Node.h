#pragma once

#include <osg/MatrixTransform>
#include <fstream>

#include "LibCogmatix.h"

namespace LibCogmatix
{
	class RotaryAxis;
	class LinearAxis;

	/**
	 * Abstract Node class. Any Cogmatic machine component will inherit from this
	 */
	class MachineNode
	{
	public:
		NodeID ID() { return _ID; }
	protected:
		NodeID _ID; ///<Machine specific ID. Unique within the machine.

		/**
		* Made protected because the constructors should never be called manually.
		*/
		MachineNode(NodeID ID) : _ID (ID) {}
		virtual ~MachineNode() {};
	private:
	};

	/**
	 * Abtract Named node. Any item requiring a user visible name will inherit from this
	 */
	class NamedMachineNode : public MachineNode
	{
	public:
		CoString Name() const { return _machineNodeName; }
		void SetName (CoString name) { _machineNodeName = name; }
	protected:
		String _machineNodeName;
		/**
		* Made protected because the constructors should never be called manually.
		*/
		NamedMachineNode(NodeID ID, CoString name) : MachineNode (ID), _machineNodeName(name) {}
		virtual ~NamedMachineNode() {};

	private:
	};
}
