//
//  AvidaController.h
//  viewer-macos
//
//  Created by David Bryson on 1/3/13.
//  Copyright (c) 2013 Michigan State University. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AvidaCore/AvidaCore.h>

@class MainWindowController;
@class WorkspaceSelectWindowController;


@interface AvidaController : NSObject {
  // Core Data Objects
  ACWorkspace* workspace;
  
  // Workspace Selection
  WorkspaceSelectWindowController* workspaceSelectWindow;
  
  // Main Window
  MainWindowController* mainWindow;
}


- (void) applicationDidFinishLaunching;
- (BOOL) applicationOpenFile:(NSURL*)fileURL;

@end
