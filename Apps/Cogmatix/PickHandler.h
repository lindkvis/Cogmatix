/***********************
* GUI picking handler *
*/

#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>

#include "LibCogmatix/Machine.h"

using namespace LibCogmatix;

namespace Cogmatix
{
	class PickHandler : public osgGA::GUIEventHandler 
	{
	protected:
		Machine::Ptr _machine;
	public: 
		PickHandler(Machine::Ptr machine) : _machine(machine) {}
		~PickHandler() {}

		bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
	};
}