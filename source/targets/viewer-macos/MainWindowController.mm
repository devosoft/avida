//
//  MainWindowController.mm
//  Avida
//
//  Created by David Bryson on 10/21/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//

#import "MainWindowController.h"

#import "AvidaRun.h"

#include <iostream>

@implementation MainWindowController

- (void) awakeFromNib {
  // Initialized the default path of the runDirControl to the user's documents directory
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  
  if ([urls count] > 0) {
    NSURL* userDocumentsURL = [urls objectAtIndex:0];
    [runDirControl setURL:userDocumentsURL];
  }
  
  currentRun = nil;
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
    [btnRunState setTitle:@"Pause"];    
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

//- (void) pathControl: (NSPathControl*) pathControl willDisplayOpenPanel: (NSOpenPanel*) openPanel {
//  if (pathControl == this->runDirControl) {
//    [openPanel setCanCreateDirectories:YES];
//  }
//}
//
@end
