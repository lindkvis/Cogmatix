/***********************
* GUI picking handler *
*/

#include <osgDB/ReadFile>
#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgWidget/WindowManager>
#include <osgWidget/Frame>

#include "LibCogmatix/Machine.h"

using namespace LibCogmatix;

namespace Cogmatix
{
	class PickHandler : public osgGA::GUIEventHandler 
	{
	protected:
		osgWidget::WindowManager* _wm;
		Machine::Ptr _machine;
        float _mx, _my;
		void toggleSelection(osgViewer::View* view, osg::Node* node, osg::Group* parent);
//		void addToSelection(osgViewer::View* view, osg::Node* node, osg::Group* parent);
//		void clearSelection(osgViewer::View* view);
	public: 
		PickHandler(osgWidget::WindowManager* wm, Machine::Ptr machine) : _wm(wm), _machine(machine), _mx(0.), _my(0.) {}
		~PickHandler() {}

		bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
	};
}
