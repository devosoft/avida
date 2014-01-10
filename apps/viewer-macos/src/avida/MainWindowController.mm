//
//  MainWindowController.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 10/21/10.
//  Copyright 2010-2013 Michigan State University. All rights reserved.
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

#import "MainWindowController.h"

#import "AvidaController.h"
#import "PopulationViewController.h"
#import "WorkspaceViewController.h"


// Global Constants
// --------------------------------------------------------------------------------------------------------------
#pragma mark Global Constants

NSString* const tbShowWorkspace = @"ShowWorkspace";
NSString* const tbRunControls = @"RunControls";
NSString* const tbViewSelect = @"ViewSelect";
NSString* const tbStatusPanel = @"StatusPanel";


// MainWindowController Private Interface
// --------------------------------------------------------------------------------------------------------------

@interface MainWindowController ()

- (void) setInterfaceRunning;
- (void) setInterfacePaused;

- (NSToolbarItem*) toolbarItemWithIdentifier:(NSString*)identifier
                                       label:(NSString*)label
                                 paleteLabel:(NSString*)paletteLabel
                                     toolTip:(NSString*)toolTip
                                      target:(id)target
                                 itemContent:(id)imageOrView
                                      action:(SEL)action
                                        menu:(NSMenu*)menu;
@end


// MainWindowController Implementation
// --------------------------------------------------------------------------------------------------------------

@implementation MainWindowController


// Init and Dealloc Methods
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Init and Dealloc Methods

- (id)initWithAvidaController: (AvidaController*)ctlr
{
  self = [super initWithWindowNibName:@"Avida-MainWindow"];
  
  if (self != nil) {
    avidaCtlr = ctlr;
    
    [toolbar setAllowsUserCustomization:YES];
    [toolbar setAutosavesConfiguration:YES];
    [toolbar setDisplayMode:NSToolbarDisplayModeIconOnly];
    
    viewPopulationCtlr = [[PopulationViewController alloc] initWithAvidaController:ctlr];    
  }
  
  return self;
}


- (void) dealloc
{
}


// Actions
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Actions

- (IBAction) toggleRunState:(id)sender
{
  if ([currentUniverse willPauseNow]) {
//    if ([currentRun numOrganisms] == 0 && ![currentRun hasPendingInjects] && [ancestorArray count] == 0) {
//      NSAlert* alert = [[NSAlert alloc] init];
//      [alert addButtonWithTitle:@"OK"];
//      [alert setMessageText:@"Unable to resume experiment; there is no start organism in the petri dish."];
//      [alert setInformativeText:@"Please drag an organism from the freezer into the settings panel or the petri dish."];
//      [alert setAlertStyle:NSWarningAlertStyle];
//      [alert beginSheetModalForWindow:[self window] modalDelegate:nil didEndSelector:nil contextInfo:nil];
//      [sender setState:NSOffState];
//      return;
//    }
    
//    if (runActive == NO) {
//      [self activateRun];
//    }
    
    [currentUniverse resume];
    [self setInterfaceRunning];
  } else {
    [currentUniverse pause];
    [self setInterfacePaused];
  }
}


- (void) setInterfaceRunning
{
  
}

- (void) setInterfacePaused
{
  
}


// NSWindowController
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSWindowController

- (void) windowDidLoad
{
  // Make the window fill the screen (a la Aperture)
  NSScreen* mainScreen = [NSScreen mainScreen];
  NSRect visibleFrame = [mainScreen visibleFrame];
  [self.window setFrame:visibleFrame display:NO];

  WorkspaceViewController* workspacePane = [[WorkspaceViewController alloc] initWithWorkspace:avidaCtlr.workspace delegate:nil];
  [splitMain replaceSubview:[splitMain.subviews objectAtIndex:0] with:workspacePane.view];
  [splitMain setHoldingPriority:NSLayoutPriorityFittingSizeCompression forSubviewAtIndex:1];
  
  CGFloat sidePaneWidth = visibleFrame.size.width / 6.0f;
  if (sidePaneWidth < 200.0f) sidePaneWidth = 200.0f;
  [splitMain setPosition:sidePaneWidth ofDividerAtIndex:0];
  
  [splitMain replaceSubview:splitMain.subviews[1] with:viewPopulationCtlr.view];
}


// NSWindowDelegate
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSWindowDelegate

- (void) windowDidBecomeMain:(NSNotification*)notification
{
  
}


- (void) windowDidResize:(NSNotification*)notification
{
  
}


- (void) windowWillClose:(NSNotification*)notification
{
  [[NSApplication sharedApplication] terminate:self];
}


- (NSSize) windowWillResize:(NSWindow*)window toSize:(NSSize)frameSize
{
  return frameSize;
}


// NSToolbarDelegate
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSToolbarDelegate

- (NSToolbarItem*) toolbarItemWithIdentifier:(NSString*)identifier
                                       label:(NSString*)label
                                 paleteLabel:(NSString*)paletteLabel
                                     toolTip:(NSString*)toolTip
                                      target:(id)target
                                 itemContent:(id)imageOrView
                                      action:(SEL)action
                                        menu:(NSMenu*)menu
{
  // here we create the NSToolbarItem and setup its attributes in line with the parameters
  NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:identifier];
  
  [item setLabel:label];
  [item setPaletteLabel:paletteLabel];
  [item setToolTip:toolTip];
  [item setTarget:target];
  [item setAction:action];
  
  // Set the right attribute, depending on if we were given an image or a view
  if ([imageOrView isKindOfClass:[NSImage class]]) {
    [item setImage:imageOrView];
  } else if ([imageOrView isKindOfClass:[NSView class]]) {
    [item setView:imageOrView];
  } else {
    assert(!"Invalid itemContent: object");
  }
  
  
  // If this NSToolbarItem is supposed to have a menu "form representation" associated with it
  // (for text-only mode), we set it up here.  Actually, you have to hand an NSMenuItem
  // (not a complete NSMenu) to the toolbar item, so we create a dummy NSMenuItem that has our real
  // menu as a submenu.
  //
  if (menu != nil) {
    // we actually need an NSMenuItem here, so we construct one
    NSMenuItem *mItem = [[NSMenuItem alloc] init];
    [mItem setSubmenu:menu];
    [mItem setTitle:label];
    [item setMenuFormRepresentation:mItem];
  }
  
  return item;
}



- (NSToolbarItem*) toolbar:(NSToolbar*)toolbar itemForItemIdentifier:(NSString*)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag
{
  NSToolbarItem *toolbarItem = nil;
  
  if ([itemIdentifier isEqualToString:tbShowWorkspace]) {
    toolbarItem = [self toolbarItemWithIdentifier:tbShowWorkspace
                                            label:@"Workspace"
                                      paleteLabel:@"Workspace"
                                          toolTip:@"Show/Hide the Workspace"
                                           target:self
                                      itemContent:tbViewShowWorkspace
                                           action:nil
                                             menu:nil];
  } else if ([itemIdentifier isEqualToString:tbRunControls]) {
    toolbarItem = [self toolbarItemWithIdentifier:tbRunControls
                                            label:@""
                                      paleteLabel:@"Run Controls"
                                          toolTip:@""
                                           target:self
                                      itemContent:tbViewRunControls
                                           action:nil
                                             menu:nil];
  } else if ([itemIdentifier isEqualToString:tbViewSelect]) {
    toolbarItem = [self toolbarItemWithIdentifier:tbViewSelect
                                            label:@"View Mode"
                                      paleteLabel:@"View Mode"
                                          toolTip:@"Select the Main View Mode"
                                           target:self
                                      itemContent:tbViewViewSelect
                                           action:nil
                                             menu:nil];
  } else if ([itemIdentifier isEqualToString:tbStatusPanel]) {
    toolbarItem = [self toolbarItemWithIdentifier:tbViewSelect
                                            label:@"Status"
                                      paleteLabel:@"Status"
                                          toolTip:@"Show/Hide the Status Panel"
                                           target:self
                                      itemContent:tbViewStatusPanel
                                           action:nil
                                             menu:nil];
  }
  
  return toolbarItem;
}


- (NSArray*) toolbarDefaultItemIdentifiers:(NSToolbar*)toolbar
{
  return @[tbShowWorkspace,
           NSToolbarSpaceItemIdentifier,
           tbRunControls,
           NSToolbarFlexibleSpaceItemIdentifier,
           tbViewSelect,
           NSToolbarFlexibleSpaceItemIdentifier,
           tbStatusPanel];
}


- (NSArray*) toolbarAllowedItemIdentifiers:(NSToolbar*)toolbar
{
  return @[tbShowWorkspace,
           tbRunControls,
           tbViewSelect,
           tbStatusPanel,
           NSToolbarSpaceItemIdentifier,
           NSToolbarFlexibleSpaceItemIdentifier];
}


// --------------------------------------------------------------------------------------------------------------
@end
