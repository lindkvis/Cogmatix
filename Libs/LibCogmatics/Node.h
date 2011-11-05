#pragma once

#include <osg/MatrixTransform>
namespace LibCogmatics
{
	/**
	 * Abstract Node class. Any Cogmatic machine component will inherit from this
	 */
	class MachineNode : public osg::MatrixTransform
	{
	protected:
		NodeID _ID; ///<Machine specific ID. Unique within the machine. 4 billion parts per machine ought to be enough for everyone.>
	public:
		typedef osg::ref_ptr<MachineNode> Ptr;
		typedef osg::ref_ptr<const MachineNode> CPtr;
		NodeID ID() { return _ID; }
	protected:
		/**
		* Made protected because the constructors should never be called manually.
		*/
		MachineNode(NodeID ID) : _ID (ID) {}
		virtual ~MachineNode() {};
	};

	/**
	 * Abtract Named node. Any item requiring a user visible name will inherit from this
	 */
	class NamedMachineNode : public MachineNode
	{
	public:
		typedef osg::ref_ptr<NamedMachineNode> Ptr;
		typedef osg::ref_ptr<const NamedMachineNode> CPtr;
		const String& Name() const { return _name; }
	protected:
		String _name;
		/**
		* Made protected because the constructors should never be called manually.
		*/
		NamedMachineNode(NodeID ID, const String& name) : _name(name), MachineNode (ID) {}
		virtual ~NamedMachineNode() {};
	};
}