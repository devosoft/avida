//
//  AvidaEDController.mm
//  avida/apps/viewer-macos
//
//  Created by David on 11/30/10.
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

#import "AvidaEDController.h"

#import "AvidaAppDelegate.h"
#import "AvidaRun.h"
#import "CenteringClipView.h"
#import "MapGridView.h"

#import "AvidaEDPopViewStatView.h"

#include "avida/viewer-core/Map.h"

static const float MAIN_SPLIT_LEFT_MIN = 140.0;
static const float MAIN_SPLIT_RIGHT_MIN = 650.0;
static const float MAIN_SPLIT_LEFT_PROPORTIONAL_RESIZE = 0.5;
static const float POP_SPLIT_LEFT_MIN = 350.0;
static const float POP_SPLIT_RIGHT_MIN = 305.0;
static const float POP_SPLIT_LEFT_PROPORTIONAL_RESIZE = 0.3;

@interface AvidaEDController (hidden)
- (void) popSplitViewAnimationEnd:(NSNumber*)collapsed;
@end
@implementation AvidaEDController (hidden)
- (void) popSplitViewAnimationEnd:(NSNumber*)collapsed {
  popSplitViewIsAnimating = NO;
  if ([collapsed boolValue]) {
    [popViewStatView setHidden:YES];
  }
}
@end

@implementation AvidaEDController

- (id) initWithAppDelegate:(AvidaAppDelegate*)delegate {
  self = [super initWithWindowNibName:@"Avida-ED-MainWindow"];
  
  if (self != nil) {
    app = delegate;

    currentRun = nil;
    listener = NULL;
    map = NULL;
        
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
  [mainSplitView replaceSubview:[[mainSplitView subviews] objectAtIndex:1] with:popView];
  [btnRunState setState:NSOffState];
  
  // Replace NSClipView of mapView's scrollView with a CenteringClipView
  NSClipView* clipView = [[CenteringClipView alloc] initWithFrame:[mapScrollView frame]];
  [clipView setBackgroundColor:[NSColor darkGrayColor]];
  [mapScrollView setContentView:clipView];
  [mapScrollView setDocumentView:mapView];
  [mapScrollView setScrollsDynamically:YES];
}


- (IBAction) toggleRunState:(id)sender {
  if ([sender state] == NSOnState && currentRun == nil) {
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
    
    if ([urls count] > 0) {
      NSURL* userDocumentsURL = [urls objectAtIndex:0];
      NSURL* runURL = [NSURL URLWithString:@"test" relativeToURL:userDocumentsURL];
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
      if (!listener) listener = new MainThreadListener(self);
      [currentRun attachListener:self];
      [popViewStatView setAvidaRun:currentRun];
      
      [txtUpdate setStringValue:@"0 updates"];
    }
  } else {
    if ([sender state] == NSOnState) {
      [currentRun resume];
    } else {
      [currentRun pause];
    }
  }
}

- (IBAction) changeMapViewMode:(id)sender {
  if (map) {
    map->SetMode(map_mode_to_color[[mapViewMode indexOfSelectedItem]]);
    [mapView updateState:map];
    [mapScaleView updateState:map];
  }
  
}

- (IBAction) changeMapZoom:(id)sender {
  if (map) {
    [mapView setZoom:[mapZoom doubleValue]];
  }
}

- (IBAction) changeView:(id)sender {
  
  NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
  if (sender == btnPopView) {
    if (curView != popView) {
      [mainSplitView replaceSubview:curView with:popView];
      curView = popView;
    }
    [btnPopView setState:NSOnState];
    [btnOrgView setState:NSOffState];
    [btnAnalyzeView setState:NSOffState];
  } else if (sender == btnOrgView) {
    if (curView != orgView) {
      [mainSplitView replaceSubview:curView with:orgView];
      curView = orgView;
    }
    [btnPopView setState:NSOffState];
    [btnOrgView setState:NSOnState];
    [btnAnalyzeView setState:NSOffState];
  } else if (sender == btnAnalyzeView) {
    if (curView != analyzeView) {
      [mainSplitView replaceSubview:curView with:analyzeView];
      curView = analyzeView;
    }
    [btnPopView setState:NSOffState];
    [btnOrgView setState:NSOffState];
    [btnAnalyzeView setState:NSOnState];
  }
}

- (IBAction) togglePopViewStatView:(id)sender {
  CGFloat dividerThickness = [popSplitView dividerThickness];
  
  if (popSplitViewIsAnimating) return;
  
  if ([sender state] == NSOnState) {
    // uncollapse
    NSRect oldPopViewStatViewFrame = popViewStatView.frame;
    oldPopViewStatViewFrame.size.width = 0;
    oldPopViewStatViewFrame.origin.x = popSplitView.frame.size.width;
    [popViewStatView setFrame:oldPopViewStatViewFrame];
    [popViewStatView setHidden:NO];
    
    NSMutableDictionary *expandPopViewDishViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [expandPopViewDishViewAnimationDict setObject:popViewDishView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewDishViewFrame = popViewDishView.frame;
    newPopViewDishViewFrame.size.width =  popSplitView.frame.size.width - lastPopViewStatViewWidth - dividerThickness;
    [expandPopViewDishViewAnimationDict setObject:[NSValue valueWithRect:newPopViewDishViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSMutableDictionary *expandPopViewStatViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [expandPopViewStatViewAnimationDict setObject:popViewStatView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewStatViewFrame = popViewStatView.frame;
    newPopViewStatViewFrame.size.width = lastPopViewStatViewWidth;
    newPopViewStatViewFrame.origin.x = popSplitView.frame.size.width - lastPopViewStatViewWidth;
    [expandPopViewStatViewAnimationDict setObject:[NSValue valueWithRect:newPopViewStatViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSViewAnimation *expandAnimation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects:expandPopViewDishViewAnimationDict, expandPopViewStatViewAnimationDict, nil]];
    [expandAnimation setDuration:0.25f];
    [expandAnimation startAnimation];
    [self performSelector:@selector(popSplitViewAnimationEnd:) withObject:[NSNumber numberWithBool:NO] afterDelay:0.25f];
  } else {
    // collapse
    // Store last width so we can jump back
    lastPopViewStatViewWidth = popViewStatView.frame.size.width;
    
    NSMutableDictionary *collapseMainAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [collapseMainAnimationDict setObject:popViewDishView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewDishViewFrame = popViewDishView.frame;
    newPopViewDishViewFrame.size.width =  popSplitView.frame.size.width - dividerThickness;
    [collapseMainAnimationDict setObject:[NSValue valueWithRect:newPopViewDishViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSMutableDictionary *collapsePopViewStatViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [collapsePopViewStatViewAnimationDict setObject:popViewStatView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewStatViewFrame = popViewStatView.frame;
//    newPopViewStatViewFrame.size.width = 0.0f;
    newPopViewStatViewFrame.origin.x = popViewStatView.frame.size.width;
    [collapsePopViewStatViewAnimationDict setObject:[NSValue valueWithRect:newPopViewStatViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSViewAnimation *collapseAnimation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects:collapseMainAnimationDict, collapsePopViewStatViewAnimationDict, nil]];
    [collapseAnimation setDuration:0.25f];
    [collapseAnimation startAnimation];
    [self performSelector:@selector(popSplitViewAnimationEnd:) withObject:[NSNumber numberWithBool:YES] afterDelay:0.25f];
  }
  popSplitViewIsAnimating = YES;
}


- (void) envActionStateChange:(NSMutableDictionary*)newState
{
  Apto::String enabled_actions;
  
  NSEnumerator *enumerator = [newState keyEnumerator];
  NSString* key;
  
  while ((key = [enumerator nextObject])) {
    if ([[newState objectForKey:key] unsignedIntValue] == NSOnState) {
      if (enabled_actions.GetSize()) enabled_actions += ",";
      enabled_actions += [key UTF8String];
    }
  }
  
  map->SetModeProperty(map->GetTagMode(), "enabled_actions", enabled_actions);
  [mapView updateState:map];
}



- (void) splitView:(NSSplitView*)splitView resizeSubviewsWithOldSize:(NSSize)oldSize {
  if (splitView == mainSplitView) {
    NSView* leftView = [[splitView subviews] objectAtIndex:0];
    NSView* rightView = [[splitView subviews] objectAtIndex:1];
    NSRect newFrame = [splitView frame];
    NSRect leftFrame = [leftView frame];
    NSRect rightFrame = [rightView frame];
    
    float dividerThickness = [splitView dividerThickness];
    
    int diffWidth = newFrame.size.width - oldSize.width;
    
    if ((leftFrame.size.width <= MAIN_SPLIT_LEFT_MIN && diffWidth < 0) || diffWidth > 0) {
      rightFrame.size.width += diffWidth;
      leftFrame.size.width = newFrame.size.width - rightFrame.size.width - dividerThickness;
    } else if (rightFrame.size.width <= MAIN_SPLIT_RIGHT_MIN && diffWidth < 0) {
      leftFrame.size.width += diffWidth;
      rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    } else {
      leftFrame.size.width += diffWidth * MAIN_SPLIT_LEFT_PROPORTIONAL_RESIZE;
      rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    }
    
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    rightFrame.size.height = newFrame.size.height;
    rightFrame.origin.x = leftFrame.size.width + dividerThickness;
    
    [leftView setFrame:leftFrame];
    [rightView setFrame:rightFrame];
  } else if (splitView == popSplitView) {
    NSView* leftView = [[splitView subviews] objectAtIndex:0];
    NSView* rightView = [[splitView subviews] objectAtIndex:1];
    NSRect newFrame = [splitView frame];
    NSRect leftFrame = [leftView frame];
    NSRect rightFrame = [rightView frame];
    
    CGFloat dividerThickness = [splitView dividerThickness];
    
    CGFloat diffWidth = floor(newFrame.size.width - oldSize.width);
    
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    if ((rightFrame.size.width <= POP_SPLIT_RIGHT_MIN && diffWidth < 0) || diffWidth > 0) {
      leftFrame.size.width += diffWidth;
    } else if (leftFrame.size.width > POP_SPLIT_LEFT_MIN) {
      leftFrame.size.width += floor(diffWidth * POP_SPLIT_LEFT_PROPORTIONAL_RESIZE);
    }
    
    rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    rightFrame.size.height = newFrame.size.height;
    rightFrame.origin.x = leftFrame.size.width + dividerThickness;
    
    [leftView setFrame:leftFrame];
    [rightView setFrame:rightFrame];
  }
}


- (BOOL) splitView:(NSSplitView*)splitView canCollapseSubview:(NSView*)subview {
  if (splitView == mainSplitView && subview == [[splitView subviews] objectAtIndex:0]) return YES;
  else if (splitView == popSplitView && subview == [[splitView subviews] objectAtIndex:1]) return YES;
  
  return NO;
}


- (CGFloat) splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMax ofSubviewAt:(NSInteger)index {
  if (splitView == mainSplitView) return proposedMax - MAIN_SPLIT_RIGHT_MIN;
  else if (splitView == popSplitView) return proposedMax - POP_SPLIT_RIGHT_MIN;
  return proposedMax;
}


- (CGFloat) splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMin ofSubviewAt:(NSInteger)index {
  if (splitView == mainSplitView) return proposedMin + MAIN_SPLIT_LEFT_MIN;
  else if (splitView == popSplitView) return proposedMin + POP_SPLIT_LEFT_MIN;
  
  return proposedMin;
}

- (void) splitViewDidResizeSubviews:(NSNotification*)notification {
  if (!popSplitViewIsAnimating) {
    if ([popSplitView isSubviewCollapsed:popViewStatView]) {
      [btnTogglePopViewStatView setState:NSOffState];
    } else {
      [btnTogglePopViewStatView setState:NSOnState];
    }
  }
}

- (void) windowWillClose:(NSNotification*)notification {
  if (currentRun != nil) {
    [currentRun end];
    currentRun = nil;
  }
  [app removeWindow:self];
}


@synthesize listener;


- (void) handleMap:(CoreViewMap*)pkg {
  if (!map) {
    map = [pkg map];
    [mapViewMode removeAllItems];
    map_mode_to_color.Clear();
    int idx = 0;
    for (int i = 0; i < map->GetNumModes(); i++) {
      if (!(map->GetModeSupportedTypes(i) & Avida::CoreView::MAP_GRID_VIEW_COLOR)) continue;
      [mapViewMode addItemWithTitle:[NSString stringWithUTF8String:(const char*)map->GetModeName(i)]];
      map_mode_to_color[idx++] = i;
    }
    [mapViewMode selectItemAtIndex:map->GetColorMode()];
    [mapViewMode setEnabled:TRUE];
    [mapZoom setEnabled:TRUE];
  } else {
    map = [pkg map];
  }
  [mapView updateState:map];
  [mapScaleView updateState:map];
  [mapZoom setDoubleValue:[mapView zoom]];
}


- (void) handleUpdate:(CoreViewUpdate*)pkg {
  NSString* str = [NSString stringWithFormat:@"%d updates", [pkg update]];
  [txtUpdate setStringValue:str]; 
}


@end
