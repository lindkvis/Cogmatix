//
//  ViewController.m
//  iCogmatix
//
//  Created by Gaute Lindkvist on 01/03/2012.
//  Copyright (c) 2012 Cogmatix. All rights reserved.
//

#import "ViewController.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgWidget/WindowManager>
#include <osgGA/StateSetManipulator>

#include "OsgPlugins.h"
#include "LibCogmatix/Machine.h"
#include "LibCogmatix/Clock.h"

using namespace LibCogmatix;
using namespace osgViewer;

@implementation ViewController

@synthesize delegate;

-(void)initOsg {
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	CGRect lFrame = [self.view bounds];
	unsigned int w = lFrame.size.width*2;
	unsigned int h = lFrame.size.height*2;
	
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowIOS::WindowData(self.view, osgViewer::GraphicsWindowIOS::WindowData::ALL_ORIENTATIONS);
	
	// Setup the traits parameters
	traits->x = 0;
	traits->y = 0;
	traits->width = w;
	traits->height = h - traits->y;
	traits->depth = 16; //keep memory down, default is currently 24
                        //traits->alpha = 8;
                        //   traits->stencil = 1;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = nullptr;
	traits->setInheritedWindowPixelFormat = true;
	
	traits->inheritedWindowData = windata;
    
	// Create the Graphics Context
	osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
	osg::DisplaySettings::instance()->setMinimumNumStencilBits(1);
    const unsigned int MASK_2D = 0xF0000000;
    
    _viewer = new osgViewer::Viewer();
    
    osgDB::setLibraryFilePathList("."); 
    
	osgWidget::WindowManager* wm = new osgWidget::WindowManager(
                                                                _viewer,
                                                                w,
                                                                h,
                                                                MASK_2D,
                                                                osgWidget::WindowManager::WM_PICK_DEBUG
                                                                );
	osg::Camera* camera = wm->createParentOrthoCamera();
	_viewer->getCamera()->setClearColor(osg::Vec4(0.0,0.0,0.0,1.0));
    
    //osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints;
    //hints->setDetailRatio(2.0f);
    
    osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(camera);        
    LibCogmatix::Machine::Ptr machine = createTestMachine(root);
    
	unsigned int clearMask = _viewer->getCamera()->getClearMask();
	_viewer->getCamera()->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
	_viewer->getCamera()->setClearStencil(0);
    
    _viewer->addEventHandler(new osgWidget::ResizeHandler(wm, camera));
    _viewer->addEventHandler(new osgWidget::CameraSwitchHandler(wm, camera));
    _viewer->addEventHandler(new osgViewer::StatsHandler());
    _viewer->addEventHandler(new osgViewer::WindowSizeHandler());
    
    _viewer->addEventHandler(new osgGA::StateSetManipulator(_viewer->getCamera()->getOrCreateStateSet()));
    
    _handler = new LibCogmatix::EventHandler(_viewer, wm, machine);
    _viewer->addEventHandler(_handler);
    
    _viewer->setUpViewInWindow(
                             0,
                             0,
                             w,
                             h
                             );
    _viewer->setSceneData(root);
	wm->resizeAllWindows();
    
    if(graphicsContext)
	{
		_viewer->getCamera()->setGraphicsContext(graphicsContext);
		_viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	}
    
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(deviceOrientationDidChange:) name: UIDeviceOrientationDidChangeNotification object: nil];
	
    _viewer->realize();
	Clock::get()->start();
    
    // get the created view
    //	osgViewer::GraphicsWindowIOS* window_ios = dynamic_cast<osgViewer::GraphicsWindowIOS*>(graphicsContext.get());
	
    //	NSLog(@"view: %@ bound: %@", window_ios->getView() , [window_ios->getView() bounds]);
    _viewer->frame(Clock::get()->elapsed());
}

- (id)initWithNibName:(NSString *)nibName bundle:(NSBundle *)nibBundle
{
	std::cout << "initWithNibName" << std::endl;
	
	if (self = [super initWithNibName:nibName bundle: nibBundle]) 
	{
		
	}
	
	return self;
}


- (void)viewDidLoad {
	std::cout << "viewDidLoad" << std::endl;
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor blueColor];
    
	[self initOsg];
	
	_displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateScene:)];
	[_displayLink setFrameInterval:1];
	[_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];  
} 

//
//Timer called function to update our scene and render the viewer
//
- (void)updateScene: (CADisplayLink *)sender {
    Clock::get()->tick();
    _viewer->frame(Clock::get()->elapsed());
    _handler->snapToLimit();
}

- (IBAction)done:(id)sender {
	std::cout << "done" << std::endl;
	[self.delegate ViewControllerDidFinish:self];
	
	
	[_displayLink invalidate];	
}


- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}


- (void)viewDidUnload {
	std::cout << "viewDidUnload" << std::endl;
    
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation 
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait
            || interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown
            || interfaceOrientation == UIInterfaceOrientationLandscapeLeft
            || interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

UIDeviceOrientation currentOrientation;

- (void)orientationChangedMethod
{
    CGRect lFrame = [self.view bounds];
	unsigned int w = lFrame.size.width*2;
	unsigned int h = lFrame.size.height*2;
    //  if (UIDeviceOrientationIsPortrait(currentOrientation))
    //  _viewer->getCamera()->setViewport(new osg::Viewport(0, 0, w, h));
    // else
    //  _viewer->getCamera()->setViewport(new osg::Viewport(0, 0, h, w));
}

- (void)deviceOrientationDidChange:(NSNotification *)notification {
    //Obtaining the current device orientation
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    
    if (orientation == UIDeviceOrientationFaceUp || 
        orientation == UIDeviceOrientationFaceDown || 
        orientation == UIDeviceOrientationUnknown || 
        currentOrientation == orientation) 
    {
        return;
    }
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(relayoutLayers) object:nil];
    //Responding only to changes in landscape or portrait
    currentOrientation = orientation;
    
    [self performSelector:@selector(orientationChangedMethod) withObject:nil afterDelay:0];
}

- (void)dealloc {
	
	std::cout << "dealloc" << std::endl;
	
	_displayLink = nil;
    _viewer = NULL;
	
    [super dealloc];
}


@end

//USE_GRAPICSWINDOW_IMPLEMENTATION(IOS)
