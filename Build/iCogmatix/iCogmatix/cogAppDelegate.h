//
//  AppDelegate.h
//  Test
//
//  Created by Gaute Lindkvist on 02/03/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class ViewController;

@interface cogAppDelegate : NSObject <UIApplicationDelegate> 
{
    UIWindow *window;
    ViewController *viewController;
}


@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet ViewController *viewController;

@end
