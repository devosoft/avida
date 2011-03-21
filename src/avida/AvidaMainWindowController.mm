//
//  AvidaMainWindowController.mm
//  Avida
//
//  Created by David Bryson on 10/21/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//
//
//  This file is part of Avida.
//
//  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along with Avida.
//  If not, see <http://www.gnu.org/licenses/>.
//

#import "AvidaMainWindowController.h"

#import "AvidaAppDelegate.h"
#import "AvidaRun.h"
#import "MapGridView.h"

#include <iostream>


@implementation AvidaMainWindowController

- (id)initWithAppDelegate: (AvidaAppDelegate*)delegate {
  self = [super initWithWindowNibName:@"Avida-MainWindow"];
  
  if (self != nil) {
    app = delegate;
    
    currentRun = nil;
    listener = NULL;

    [self showWindow:self];
  }
  
  return self;
}


- (void) dealloc {
  delete listener;
  listener = NULL;
  [super dealloc];
}


- (void) finalize {
  delete listener;
  listener = NULL;
  [super finalize];
}


- (void) windowDidLoad {
  // Initialized the default path of the runDirControl to the user's documents directory
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  
  if ([urls count] > 0) {
    NSURL* userDocumentsURL = [urls objectAtIndex:0];
    [runDirControl setURL:userDocumentsURL];
  }
}


- (IBAction) setRunDir:(id)sender {
  // Set the current path to a selected sub-component of the path when clicked
  NSPathComponentCell* path_clicked = [runDirControl clickedPathComponentCell];
  if (path_clicked != nil) {
    [runDirControl setURL:[path_clicked URL]];
  }
}


- (IBAction) toggleRunState:(id)sender {
  if (currentRun == nil) {
    currentRun = [[AvidaRun alloc] initWithDirectory:[runDirControl URL]];
    if (currentRun == nil) {
      NSAlert* alert = [[NSAlert alloc] init];
      [alert addButtonWithTitle:@"OK"];
      NSString* pathText = [[runDirControl URL] lastPathComponent];
      NSString* msgText = [NSString stringWithFormat:@"Unable to load run configuration in \"%@\"", pathText];
      [alert setMessageText:msgText];
      [alert setInformativeText:@"Check the run log for details on how to correct your configuration files."];
      [alert setAlertStyle:NSWarningAlertStyle];
      [alert beginSheetModalForWindow:[sender window] modalDelegate:nil didEndSelector:nil contextInfo:nil];
    } else {
      if (!listener) listener = new cMainThreadListener(self);
      [currentRun attachListener:self];
      
      [btnRunState setTitle:@"Pause"];
      [txtUpdate setStringValue:@"Update: 0"];
    }
  } else {
    if ([currentRun isPaused]) {
      [currentRun resume];
      [btnRunState setTitle:@"Pause"];
    } else {
      [currentRun pause];
      [btnRunState setTitle:@"Run"];      
    }
  }
}


- (void) pathControl:(NSPathControl*)pathControl willDisplayOpenPanel: (NSOpenPanel*) openPanel {
  if (pathControl == self->runDirControl) {
    [openPanel setCanCreateDirectories:YES];
  }
}


- (void)windowWillClose: (NSNotification *)notification {
  if (currentRun != nil) {
    [currentRun end];
    currentRun = nil;
  }
  [app removeWindow:self];
}


@synthesize listener;


- (void) handleMap: (CoreViewMap*)pkg {
  [mapView updateState: [pkg map]];
}


- (void) handleUpdate: (CoreViewUpdate*)pkg {
  NSString* str = [NSString stringWithFormat:@"Update: %d", [pkg update]];
  [txtUpdate setStringValue:str]; 
}

@end
