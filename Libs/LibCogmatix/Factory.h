#pragma once

#include "LibCogmatix.h"

#include "Axis.h"
#include "CompositePart.h"
#include "Light.h"
#include "Machine.h"
#include "Motor.h"
#include "Part.h"
#include "ParametricSpurGear.h"

namespace LibCogmatix
{
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
		LinearAxis::Ptr CreateLinearAxis(const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max);

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
		RotaryAxis::Ptr CreateRotaryAxis(const Vec& axisVector, const Vec& origin, float valueInitial, float min, float max);

		/**
		 * Constructs a new Machine
		 */
		Machine::Ptr CreateMachine(CoString name);	

		CompositePart::Ptr CreateCompositePart(CoString name, CoString fileName);
		Part::Ptr CreatePart(CoString name);

		ParametricSpurGearPart::Ptr CreateParametricSpurGearPart(CoString name, Machine* machine, const Vec& axis, short numberOfTeeth, double depth, double axisDiameter, double module, double helix);

		Light::Ptr CreateLight(Machine* machine, const Vec& position, const Vec4& colour);

		Motor::Ptr CreateMotor(double RPM);
	private:
		NodeID _nextID;

		// Private constructor. Called by get() method.
		Factory() : _nextID(0) {}
		~Factory(void);

		// Private copy constructor and assigned operator. Never to be used.
		Factory(Factory const&) {}	
		Factory& operator=(Factory const&) { return *m_pInstance; };

		static Factory* m_pInstance;
	};
}

