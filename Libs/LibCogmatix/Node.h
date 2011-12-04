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
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_NVP(_ID);
		}
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
		NamedMachineNode(NodeID ID, CoString name) : _machineNodeName(name), MachineNode (ID) {}
		virtual ~NamedMachineNode() {};

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(MachineNode);
			ar & BOOST_SERIALIZATION_NVP(_machineNodeName);
		}
	};
}

BOOST_CLASS_VERSION(LibCogmatix::MachineNode, 0)
BOOST_CLASS_VERSION(LibCogmatix::NamedMachineNode, 0)