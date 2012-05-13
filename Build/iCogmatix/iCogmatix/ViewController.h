//
//  MainViewController.h
//  iCogmatix
//
//  Created by Gaute Lindkvist on 01/03/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#include <osgViewer/Viewer>
#include "LibCogmatix/EventHandler.h"

@protocol ViewControllerDelegate;

@interface ViewController : UIViewController {
	id <ViewControllerDelegate> delegate;
    
    osg::ref_ptr<osgViewer::Viewer> _viewer;
    osg::ref_ptr<LibCogmatix::EventHandler> _handler;
	CADisplayLink* _displayLink;
}

- (void)initOsg;

@property (nonatomic, assign) id <ViewControllerDelegate> delegate;
- (IBAction)done:(id)sender;
@end


@protocol ViewControllerDelegate
- (void)ViewControllerDidFinish:(ViewController *)controller;
@end
