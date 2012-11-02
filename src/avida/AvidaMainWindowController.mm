//
//  AvidaMainWindowController.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 10/21/10.
//  Copyright 2010-2012 Michigan State University. All rights reserved.
//  http://avida.devosoft.org/viewer-macos
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
//  following conditions are met:
//  
//  1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the
//      following disclaimer.
//  2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//      following disclaimer in the documentation and/or other materials provided with the distribution.
//  3.  Neither the name of Michigan State University, nor the names of contributors may be used to endorse or promote
//      products derived from this software without specific prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY MICHIGAN STATE UNIVERSITY AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL MICHIGAN STATE UNIVERSITY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Authors: David M. Bryson <david@programerror.com>
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
    currentRun = [[AvidaRun alloc] initWithDirectory:[[runDirControl URL] path]];
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
      if (!listener) listener = new MainThreadListener(self);
      [currentRun attachListener:self];
      
      [btnRunState setTitle:@"Pause"];
      [txtUpdate setStringValue:@"Update: 0"];
    }
  } else {
    if ([currentRun willPause]) {
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
//  [app removeWindow:self];
}


@synthesize listener;


- (void) handleMap: (ViewerMap*)pkg {
  [mapView updateState: [pkg map]];
}


- (void) handleUpdate: (ViewerUpdate*)pkg {
  NSString* str = [NSString stringWithFormat:@"Update: %d", [pkg update]];
  [txtUpdate setStringValue:str]; 
}

@end
