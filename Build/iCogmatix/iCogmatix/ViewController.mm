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
#include <osg/Material>
#include <osgGA/StateSetManipulator>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include "LibCogmatix/Factory.h"
#include "LibCogmatix/Clock.h"
#include "LibCogmatix/Light.h"
#include "LibCogmatix/GearPart.h"

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
	traits->sharedContext = nullptr;
	traits->setInheritedWindowPixelFormat = true;
	
	traits->inheritedWindowData = windata;
    
	// Create the Graphics Context
	osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
	
    _viewer = new osgViewer::Viewer();
    
  	Machine::Ptr machine = Factory::get()->CreateMachine("TestMachine");
	//LinearAxis::Ptr axisLinear = Factory::get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	//LinearAxis::Ptr axisLinear2 = Factory::get()->CreateLinearAxis(Vec(1., 0., 0.), Vec(0., 0., 0.), 0., 0., 100.);
	//LinearAxis::Ptr axisLinear3 = Factory::get()->CreateLinearAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., 0., 100.);
	//RotaryAxis::Ptr axisRotary = Factory::get()->CreateRotaryAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., -10000000., 10000000.);
	//RotaryAxis::Ptr axisRotary2 = Factory::get()->CreateRotaryAxis(Vec(0., 0., 1.), Vec(0., 0., 0.), 0., -10000000., 10000000.);
	//CompositePart::Ptr part = Factory::get()->CreateCompositePart("Test part", "D:\\Cogmotion\\3rdParty\\OpenSceneGraph\\data\\dumptruck.osg");
	ParametricSpurGearPart::Ptr gear = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(0., 0., 0.), 40, 1.5, 1.0, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear2 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(7.5, 0., 0.),10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear3 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 0.), 10, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear4 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0., 5.1), 24, 1.0, 0.5, 0.3, 0.);
	ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, 0, 12.5), 20, 1.0, 0.5, 0.3, 0.);
    
	gear2->snapTo(gear);
	gear3->snapTo(gear2);
	gear4->snapTo(gear3);
    gear5->snapTo(gear4);
	//ParametricSpurGearPart::Ptr gear5 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 1., 0.), Vec(10.5, -2., 5.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
	//ParametricSpurGearPart::Ptr gear6 = Factory::get()->CreateParametricSpurGearPart("TestGear", machine.get(), Vec(0., 0., 1.), Vec(10.5, -6., 10.1), 24, 1.0, 0.5, 0.3, 0., PI/4);
	bool bFastPaths = gear->gear()->areFastPathsUsed();
	BoxMotor::Ptr motor = Factory::get()->CreateBoxMotor(20, Vec(0., 1., 0.), Vec(0., 5., 0.), Vec(5., 5., 5.), 1.0, 7.5);
	machine->addChild(motor);
    machine->addChild(gear);
	machine->addChild(gear2);
	machine->addChild(gear3);
	machine->addChild(gear4);
	machine->addChild(gear5);
	//machine->addChild(gear6);
	Clock::get()->add(motor);
    
	Light::Ptr lightBlue = Factory::get()->CreateLight(machine.get(), Vec(20., -20., 10.), Vec4(0.9, 0.9, 1., 1.));
	machine->addChild(lightBlue);
	Light::Ptr lightRed = Factory::get()->CreateLight(machine.get(), Vec(-50., -10., 30.), Vec4(1., 0.9, 0.9, 1.));
	machine->addChild(lightRed);
    
	osg::StateSet* gearState = machine->getOrCreateStateSet();
    
	// add a reflection map to the teapot.     
	osg::Image* image = osgDB::readImageFile("skymap.jpg");
	if (image)
	{
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);
        
		osg::TexGen* texgen = new osg::TexGen;
		texgen->setMode(osg::TexGen::SPHERE_MAP);
        
		gearState->setTextureAttributeAndModes(0,texture,osg::StateAttribute::ON);
		gearState->setTextureAttributeAndModes(0,texgen,osg::StateAttribute::ON);
        
		//geode->setStateSet(stateset);
	}
    motor->start();
    
    
    /*	osg::Program* brickProgramObject = new osg::Program;
     osg::Shader* brickVertexObject = 
     new osg::Shader( osg::Shader::VERTEX );
     osg::Shader* brickFragmentObject = 
     new osg::Shader( osg::Shader::FRAGMENT );
     brickProgramObject->addShader( brickFragmentObject );
     brickProgramObject->addShader( brickVertexObject );
     loadShaderSource( brickVertexObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.vert" );
     loadShaderSource( brickFragmentObject, "D:/Cogmotion/3rdParty/OpenSceneGraph/data/shaders/brick.frag" );
     */
    osg::Group* world = new osg::Group();
    world->addChild(machine);
    //	world->addChild(camera);
    
	unsigned int clearMask = _viewer->getCamera()->getClearMask();
	_viewer->getCamera()->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
	_viewer->getCamera()->setClearStencil(0);

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
