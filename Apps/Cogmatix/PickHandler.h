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
		std::vector<osg::Node*> _selection;
		std::vector<osg::ref_ptr<osgWidget::Frame>> _frames;

		void toggleSelection(osgViewer::View* view, osg::Node* node);
		void addToSelection(osgViewer::View* view, osg::Node* node);
		void clearSelection(osgViewer::View* view);
	public: 
		PickHandler(osgWidget::WindowManager* wm, Machine::Ptr machine) : _wm(wm), _machine(machine) {}
		~PickHandler() {}

		bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
		virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);
	};
}
