//
//  MainWindowController.mm
//  Avida
//
//  Created by David Bryson on 10/21/10.
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

#import "MainWindowController.h"

#import "AvidaRun.h"

#include "cCocoaListener.h"

#include <iostream>

@implementation MainWindowController

@synthesize txtUpdate;

- (void) awakeFromNib {
  // Initialized the default path of the runDirControl to the user's documents directory
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  
  if ([urls count] > 0) {
    NSURL* userDocumentsURL = [urls objectAtIndex:0];
    [runDirControl setURL:userDocumentsURL];
  }
  
  currentRun = nil;
  viewListener = NULL;
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
      if (!viewListener) viewListener = new cCocoaListener(self);
      [currentRun attachListener:viewListener];
      [btnRunState setTitle:@"Pause"];
      [txtUpdate setStringValue:@"Update: 0"];
    }
  } else {
    if ([currentRun isPaused]) {
      [currentRun resume];
      [btnRunState setTitle:@"Paused"];
    } else {
      [currentRun pause];
      [btnRunState setTitle:@"Run"];      
    }
  }
}

- (void) pathControl: (NSPathControl*) pathControl willDisplayOpenPanel: (NSOpenPanel*) openPanel {
  if (pathControl == self->runDirControl) {
    [openPanel setCanCreateDirectories:YES];
  }
}

- (void) dealloc {
  delete viewListener;
  viewListener = NULL;
  [super dealloc];
}

- (void) finalize {
  delete viewListener;
  viewListener = NULL;
  [super finalize];
}

@end
