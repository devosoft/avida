//
//  WorkspaceSelectWindowController.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 12/7/12.
//  Copyright 2012 Michigan State University. All rights reserved.
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

#import "WorkspaceSelectWindowController.h"

#import "AvidaController.h"
#import "AvidaPreferences.h"


// WorkspaceSelectWindowController Private Interface
// --------------------------------------------------------------------------------------------------------------

@interface WorkspaceSelectWindowController ()
- (BOOL) loadWorkspace:(ACWorkspace*)workspace;
@end


// WorkspaceSelectWindowController Implementation
// --------------------------------------------------------------------------------------------------------------

@implementation WorkspaceSelectWindowController


// Initialization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Initialization

- (id) initWithAvidaController:(AvidaController*)ctlr
{
  self = [super initWithWindowNibName:@"Avida-WorkspaceSelectWindow"];
  if (self) {
    avidaCtlr = ctlr;
  }
  
  return self;
}



// Actions
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Actions

- (IBAction) cancelWorkspaceSelection:(id)sender
{
  [self close];
  [avidaCtlr workspaceSelectionCancelled];
}


- (IBAction) locateWorkspace:(id)sender
{
  [self close];

  NSOpenPanel* openDlg = [NSOpenPanel openPanel];
  [openDlg setCanChooseFiles:YES];
  [openDlg setAllowedFileTypes:[ACWorkspace fileTypes]];
  [openDlg setTitle:@"Open Workspace..."];
  [openDlg setPrompt:@"Open Workspace"];
  
  // Display the dialog.  If the OK button was pressed, process the files.
  if ([openDlg runModal] == NSFileHandlingPanelOKButton) {
    NSArray* files = [openDlg URLs];
    NSURL* workspaceURL = [files objectAtIndex:0];
    
    ACWorkspace* workspace = [[ACWorkspace alloc] initWithURL:workspaceURL];
    if (![self loadWorkspace:workspace]) [self showWindow:sender];
  } else {
    [self showWindow:sender];
  }
}


- (IBAction) openWorkspace:(id)sender
{
  [self close];
  
  NSArray* selectedWorkspaces = workspaceArrayCtlr.selectedObjects;
  assert([selectedWorkspaces count] == 1);
  
  ACWorkspace* workspace = (ACWorkspace*)[selectedWorkspaces objectAtIndex:0];
  [avidaCtlr workspaceSelected:workspace];
}


- (IBAction) newWorkspace:(id)sender
{
  [self close];

  NSSavePanel* saveDlg = [NSSavePanel savePanel];
  [saveDlg setCanCreateDirectories:YES];
  [saveDlg setAllowedFileTypes:[ACWorkspace fileTypes]];
  [saveDlg setTitle:@"Create Workspace..."];
  [saveDlg setPrompt:@"Create Workspace"];
  
  // Display the dialog.  If the OK button was pressed, process the files.
  if ([saveDlg runModal] == NSFileHandlingPanelOKButton) {
    NSURL* workspaceURL = [saveDlg URL];
    
    // Attempt to lookup the workspace in already opened spaces
    ACWorkspace* workspace = [workspaceDict objectForKey:workspaceURL];

    // If already loaded, must wipe it out prior to creating a new..
    if (workspace) {
      NSWorkspace* nsworkspace = [NSWorkspace sharedWorkspace];
      
      void (^completionHandler)(NSDictionary*, NSError*) = ^(NSDictionary* newURLs, NSError* error) {
        (void)newURLs;
        
        if (error == nil) {
          // No error means successful deletion of the old workspace. Remove from list, create a new one...
          [workspaceArrayCtlr removeObject:workspace];
          
          ACWorkspace* workspace = [ACWorkspace createAtURL:workspaceURL];
          if (![self loadWorkspace:workspace]) [self showWindow:sender];
        } else {
          // Error occurred, notify user and return to the selection window
          NSAlert* errAlert = [NSAlert alertWithError:error];
          [errAlert runModal];
          [self showWindow:sender];
        }
      };
      
      [nsworkspace recycleURLs:@[workspaceURL] completionHandler:completionHandler];
    } else {
      ACWorkspace* workspace = [ACWorkspace createAtURL:workspaceURL];
      if (![self loadWorkspace:workspace]) [self showWindow:sender];
    }
  } else {
    [self showWindow:sender];
  }
}



// NSWindowController
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSWindowController

- (void)windowDidLoad
{
  [super windowDidLoad];
  
  NSUserDefaults* userDefaults = [NSUserDefaults standardUserDefaults];
  NSArray* knownWorkspaces = [userDefaults arrayForKey:PrefKeyWorkspaceURLs];
  workspaceDict = [NSMutableDictionary dictionaryWithCapacity:[knownWorkspaces count]];
  
  // Load all known workspaces
  for (NSData* urlData in knownWorkspaces) {
    BOOL urlIsStale = NO;
    NSError* error = nil;
    NSURL* workspaceURL = (NSURL*)[NSURL URLByResolvingBookmarkData:urlData
                                                            options:NSURLBookmarkResolutionWithSecurityScope
                                                      relativeToURL:nil
                                                bookmarkDataIsStale:&urlIsStale
                                                              error:&error];
    
    
    if (!urlIsStale && workspaceURL) {
      // If already loaded, ignore
      if ([workspaceDict objectForKey:workspaceURL] != nil) continue;

      // Attempt to load the workspace object
      [workspaceURL startAccessingSecurityScopedResource];
      ACWorkspace* workspace = [[ACWorkspace alloc] initWithURL:workspaceURL];
      
      if (workspace) {
        [workspaceDict setObject:workspace forKey:workspaceURL];
        [workspaceArrayCtlr addObject:workspace];
      }
    }
  }
}


// Workspace Loading
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Workspace Loading

- (BOOL) loadWorkspace:(ACWorkspace*)workspace {
  if (workspace) {
    // Add newly loaded workspace to the selection list
    [workspaceDict setObject:workspace forKey:workspace.location];
    [workspaceArrayCtlr addObject:workspace];
    
    // Write known workspaces to preferences
    NSMutableArray* knownWorkspaces = [NSMutableArray arrayWithCapacity:[workspaceArrayCtlr.arrangedObjects count]];
    for (ACWorkspace* workspace in workspaceArrayCtlr.arrangedObjects) {
      NSError* error = nil;
      [knownWorkspaces addObject:[workspace.location bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope
                                              includingResourceValuesForKeys:@[NSURLNameKey]
                                                               relativeToURL:nil
                                                                       error:&error]];
    }
    [[NSUserDefaults standardUserDefaults] setObject:knownWorkspaces forKey:PrefKeyWorkspaceURLs];
    
    // Notify the controller that a workspace has been selected
    [avidaCtlr workspaceSelected:workspace];
    return YES;
  }
  
  return NO;
}


// --------------------------------------------------------------------------------------------------------------
@end
