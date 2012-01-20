//
//  AvidaAppDelegate.m
//  avida/apps/viewer-macos
//
//  Created by David Bryson on 10/20/10.
//  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#import "AvidaAppDelegate.h"

#import "AvidaMainWindowController.h"
#import "AvidaEDController.h"


@implementation AvidaAppDelegate

- (void) applicationDidFinishLaunching:(NSNotification*)aNotification {
  if ([windows count] == 0) {
    // @TODO Should pop up welcome window instead of default workspace
    
    [self newAvidaED:self]; 
  }
}

- (BOOL) application:(NSApplication*)theApplication openFile:(NSString*)filename {
  NSURL* fileURL = [NSURL fileURLWithPath:filename];
  AvidaEDController* ctrl = [[AvidaEDController alloc] initWithAppDelegate:self inWorkspace:fileURL];
  if (ctrl == nil) {
    NSLog(@"Error loading Avida-ED-MainWindow NIB");
    return NO;
  } else {
    [windows addObject:ctrl];
  }

  return YES;
}



- (IBAction) newAvida:(id)sender {
  AvidaMainWindowController* ctrl = [[AvidaMainWindowController alloc] initWithAppDelegate:self];
  if (ctrl == nil) {
    NSLog(@"Error loading Avida-MainWindow NIB");
  } else {
    [windows addObject:ctrl];
  }
}


- (IBAction) newAvidaED:(id)sender {
  AvidaEDController* ctrl = [[AvidaEDController alloc] initWithAppDelegate:self];
  if (ctrl == nil) {
    NSLog(@"Error loading Avida-ED-MainWindow NIB");
  } else {
    [windows addObject:ctrl];
  }
}


- (IBAction) openAvidaEDWorkspace:(id)sender {

  NSOpenPanel* openDlg = [NSOpenPanel openPanel];  
  [openDlg setCanChooseFiles:YES];
  [openDlg setAllowedFileTypes:[NSArray arrayWithObject:@"org.devosoft.avida.avida-ed-workspace"]];
  
  // Display the dialog.  If the OK button was pressed, process the files.
  if ([openDlg runModal] == NSOKButton) {
    NSArray* files = [openDlg URLs];    
    NSURL* fileURL = [files objectAtIndex:0];
    
    AvidaEDController* ctrl = [[AvidaEDController alloc] initWithAppDelegate:self inWorkspace:fileURL];
    if (ctrl == nil) {
      NSLog(@"Error loading Avida-ED-MainWindow NIB");
    } else {
      [windows addObject:ctrl];
    }
  }
}


- (IBAction) duplicateAvidaEDWorkspace:(id)sender {
  NSSavePanel* saveDlg = [NSSavePanel savePanel];  
  [saveDlg setCanCreateDirectories:YES];
  [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"org.devosoft.avida.avida-ed-workspace"]];
  
  // Display the dialog.  If the OK button was pressed, process the files.
  if ([saveDlg runModal] == NSOKButton) {
    NSURL* fileURL = [saveDlg URL];
    NSWindowController* cur_ctrl = [[[NSApplication sharedApplication] mainWindow] windowController];
    if (cur_ctrl != nil && [cur_ctrl respondsToSelector:@selector(duplicateFreezerAtURL:)]) {
      [(AvidaEDController*)cur_ctrl duplicateFreezerAtURL:fileURL];
    }
    
    AvidaEDController* new_ctrl = [[AvidaEDController alloc] initWithAppDelegate:self inWorkspace:fileURL];
    if (new_ctrl == nil) {
      NSLog(@"Error loading Avida-ED-MainWindow NIB");
    } else {
      [windows addObject:new_ctrl];
    }
  }  
}


- (BOOL) validateMenuItem:(NSMenuItem*)item {
  if ([item action] == @selector(duplicateAvidaEDWorkspace:)) {
    NSWindowController* ctrl = [[[NSApplication sharedApplication] mainWindow] windowController];
    if (ctrl == nil || ![ctrl respondsToSelector:@selector(duplicateFreezerAtURL:)]) return NO;
  }
  return YES;
}




- (void) removeWindow:(id)sender {
  [windows removeObject:sender];
}


@end
