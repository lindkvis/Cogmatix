#pragma once

#include <map>
#include <osg/Node>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include "LibCogmatix.h"
#include "Units.h"

namespace LibCogmatix
{
    class LinearAxis;
    class RotaryAxis;
    class Machine;
    class CompositePart;
    class ParametricSpurGearPart;
    class Light;
    class Motor;
    
	/**
	* Factory class, using the singleton pattern.
	*/
	class Factory
	{
	public:
		/** 
		 * Static singleton method. Will create factory if necessary
		 */
		static Factory* get();

		/**
		 * Constructs a new linear axis
	     * @axisVector Axis vector
	 	 * @origin Origin of the axis in world space
		 * @valueInitial Initial reference point along the axis. 
						This will not change orientation, but if an 
						initial position of 100 was set, a moveTo of 200 will result in a relative move of 100.
		 * @min Lower axis limit
		 * @max Upper axis limit
		 */
        osg::ref_ptr<LinearAxis> CreateLinearAxis(const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max);

		/**
		 * Constructs a new rotary axis
		 * @axisVector Axis vector
	 	 * @origin Origin of the axis in world space
		 * @valueInitial Initial reference point along the axis. 
						This will not change orientation, but if an 
						initial position of 100 was set, a moveTo of 200 will result in a relative move of 100.
		 * @min Lower axis limit
		 * @max Upper axis limit
		 */
		osg::ref_ptr<RotaryAxis> CreateRotaryAxis(const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max);

		/**
		 * Constructs a new Machine
		 */
		osg::ref_ptr<Machine> CreateMachine(CoString name);	

		osg::ref_ptr<CompositePart> CreateCompositePart(CoString name, CoString fileName);

		osg::ref_ptr<ParametricSpurGearPart> CreateParametricSpurGearPart(CoString name, Machine* machine, const Vec& axis, const Vec& origin,
				short numberOfTeeth, double depth, double axisDiameter, double module, double helix, double pitch_angle=PI/2.);

		osg::ref_ptr<Light> CreateLight(Machine* machine, const Vec& position, const Vec4& colour);

		osg::ref_ptr<Motor> CreateMotor(double RPM);
        
        osg::Object* getNode (NodeID ID) const
        {
            NodeDB::const_iterator it = _nodeDB.find (ID);
            if (it != _nodeDB.end() && it->second.valid())
            {
                return it->second.get();
            }
            return nullptr;
        }
        
        bool removeNode (NodeID ID);
        NodeID copyNode (NodeID copyID);
        
        
	private:
		NodeID _nextID;
            
        // Node database
        typedef std::map<NodeID, osg::observer_ptr<osg::Object> > NodeDB;
        NodeDB _nodeDB;
        
		// Private constructor. Called by get() method.
		Factory() : _nextID(0) {}
		~Factory(void);

		// Private copy constructor and assigned operator. Never to be used.
		Factory(Factory const&) {}	
		Factory& operator=(Factory const&) { return *m_pInstance; };

		static Factory* m_pInstance;
	};
}

