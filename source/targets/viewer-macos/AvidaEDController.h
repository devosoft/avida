//
//  AvidaEDController.h
//  Avida
//
//  Created by David on 11/30/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class AvidaAppDelegate;


@interface AvidaEDController : NSWindowController <NSWindowDelegate> {
  AvidaAppDelegate* app;
}

-(id)initWithAppDelegate:(AvidaAppDelegate*)delegate;
-(void)windowWillClose:(NSNotification*)notification;


@end
