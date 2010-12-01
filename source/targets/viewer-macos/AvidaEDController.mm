//
//  AvidaEDController.mm
//  Avida
//
//  Created by David on 11/30/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2
//  of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#import "AvidaEDController.h"

#import "AvidaAppDelegate.h"
#import "AvidaRun.h"


@implementation AvidaEDController

- (id)initWithAppDelegate: (AvidaAppDelegate*)delegate {
  self = [super initWithWindowNibName:@"Avida-ED-MainWindow"];
  
  if (self != nil) {
    app = delegate;
    [self showWindow:self];
  }
  
  return self;
}


- (void) dealloc {
//  delete viewListener;
//  viewListener = NULL;
  [super dealloc];
}


- (void) finalize {
//  delete viewListener;
//  viewListener = NULL;
  [super finalize];
}


- (IBAction) toggleRunState:(id)sender {
  if ([sender state] == NSOnState && currentRun == nil) {
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
    
    if ([urls count] > 0) {
      NSURL* userDocumentsURL = [urls objectAtIndex:0];
      NSURL* runURL = [NSURL URLWithString:@"../test" relativeToURL:userDocumentsURL];
      currentRun = [[AvidaRun alloc] initWithDirectory:runURL];
    }
    if (currentRun == nil) {
      NSAlert* alert = [[NSAlert alloc] init];
      [alert addButtonWithTitle:@"OK"];
      NSString* msgText = [NSString stringWithFormat:@"Unable to load run configuration in \"test\""];
      [alert setMessageText:msgText];
      [alert setInformativeText:@"Check the run log for details on how to correct your configuration files."];
      [alert setAlertStyle:NSWarningAlertStyle];
      [alert beginSheetModalForWindow:[sender window] modalDelegate:nil didEndSelector:nil contextInfo:nil];
      [sender setState:NSOffState];
    } else {
//      if (!viewListener) viewListener = new cCocoaListener(self);
//      [currentRun attachListener:viewListener];
      
      [txtUpdate setStringValue:@"Time (updates): 0"];
    }
  } else {
    if ([sender state] == NSOffState) {
      [currentRun resume];
    } else {
      [currentRun pause];
    }
  }
}


- (void)windowWillClose: (NSNotification*)notification {
  if (currentRun != nil) {
    [currentRun end];
    currentRun = nil;
  }
  [app removeWindow:self];
}


- (void) handleMap: (id)object {
//  [mapView updateState: viewListener->GetMap()];
}


- (void) handleUpdate: (id)object {
//  NSString* str = [NSString stringWithFormat:@"Update: %d", update];
//  [txtUpdate setStringValue:str]; 
}


@end
