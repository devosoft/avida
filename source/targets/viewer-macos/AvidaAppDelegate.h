//
//  AvidaAppDelegate.h
//  Avida
//
//  Created by David Bryson on 10/20/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AvidaAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end
