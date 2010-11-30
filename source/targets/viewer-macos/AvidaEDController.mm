//
//  AvidaEDController.mm
//  Avida
//
//  Created by David on 11/30/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "AvidaEDController.h"

#import "AvidaAppDelegate.h"


@implementation AvidaEDController

- (id)initWithAppDelegate: (AvidaAppDelegate*)delegate {
  self = [super initWithWindowNibName:@"Avida-ED-MainWindow"];
  
  if (self != nil) {
    app = delegate;
    [self showWindow:self];
  }
  
  return self;
}

- (void)windowWillClose: (NSNotification *)notification {
  [app removeWindow:self];
}


@end
