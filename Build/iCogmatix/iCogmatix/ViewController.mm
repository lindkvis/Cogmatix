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
#include <osgViewer/GraphicsWindowIOS>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osg/PolygonMode>
#include <iostream>
#include <osg/TexGen>
#include <osg/Texture2D>
#include <osgWidget/Util>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/WindowManager>

#include "LibCogmatix/Factory.h"
#include "LibCogmatix/Clock.h"

using namespace LibCogmatix;
using namespace osgViewer;

@implementation ViewController

@synthesize delegate;

-(void)initOsg {
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	CGRect lFrame = [self.view bounds];
	unsigned int w = lFrame.size.width;
	unsigned int h = lFrame.size.height;
	
	osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowIOS::WindowData(self.view, osgViewer::GraphicsWindowIOS::WindowData::IGNORE_ORIENTATION);
	
	// Setup the traits parameters
	traits->x = 0;
	traits->y = 40;
	traits->width = w;
	traits->height = h - traits->y;
	traits->depth = 16; //keep memory down, default is currently 24
	//traits->alpha = 8;
	//traits->stencil = 8;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->setInheritedWindowPixelFormat = true;
	
	traits->inheritedWindowData = windata;
    
	// Create the Graphics Context
	osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
	
    _viewer = new osgViewer::Viewer();
    
    Machine::Ptr machine = Factory::get()->CreateMachine("TestMachine");
	LinearAxis::Ptr axisLinear = Factory::get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	LinearAxis::Ptr axisLinear2 = Factory::get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	LinearAxis::Ptr axisLinear3 = Factory::get()->CreateLinearAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., 0., 100.);

	ParametricSpurGearPart::Ptr gear = Factory::get()->CreateParametricSpurGearPart("TestGear1", machine.get(), Vec(0., 1., 0.), Vec(0., 0., 0.), 40, 1.5, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear2 = Factory::get()->CreateParametricSpurGearPart("TestGear2", machine.get(), Vec(0., 1., 0.), Vec(7.5, 0., 0.),10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear3 = Factory::get()->CreateParametricSpurGearPart("TestGear3", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 0.), 10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear4 = Factory::get()->CreateParametricSpurGearPart("TestGear4", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 5.1), 24, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear5", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0, 12.5), 20, 1.0, 0.5, 0.3, 0.);
    
	gear4->snapTo(gear3);
    gear5->snapTo(gear4);

	bool bFastPaths = gear->gear()->areFastPathsUsed();
	Motor::Ptr motor = Factory::get()->CreateMotor(20);
	machine->addChild(motor);
	motor->addChild(gear);
	machine->addChild(gear2);
	machine->addChild(gear3);
	machine->addChild(gear4);
	machine->addChild(gear5);
    
	Clock::get()->add(motor);
    
	Light::Ptr lightBlue = Factory::get()->CreateLight(machine.get(), Vec(20., -20., 10.), Vec4(0.5, 0.5, 1., 1.));
	machine->addChild(lightBlue);
	Light::Ptr lightRed = Factory::get()->CreateLight(machine.get(), Vec(-50., -10., 30.), Vec4(1., 0.5, 0.5, 1.));
	machine->addChild(lightRed);
 
    motor->start();
    
    osg::Group* world = new osg::Group();
	world->addChild(machine);

    if(graphicsContext)
	{
		_viewer->getCamera()->setGraphicsContext(graphicsContext);
		_viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	}
	
	_viewer->setSceneData(world);
	_viewer->setCameraManipulator(new osgGA::MultiTouchTrackballManipulator());
    _viewer->realize();
	Clock::get()->start();
    
    // get the created view
	osgViewer::GraphicsWindowIOS* window_ios = dynamic_cast<osgViewer::GraphicsWindowIOS*>(graphicsContext.get());
	
	NSLog(@"view: %@ bound: %@", window_ios->getView() , [window_ios->getView() bounds]);
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


/*
 // Override to allow orientations other than the default portrait orientation.
 - (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
 // Return YES for supported orientations
 return (interfaceOrientation == UIInterfaceOrientationPortrait);
 }
 */


- (void)dealloc {
	
	std::cout << "dealloc" << std::endl;
	
	_displayLink = nil;
    _viewer = NULL;
	
    [super dealloc];
}


@end

USE_GRAPICSWINDOW_IMPLEMENTATION(IOS)