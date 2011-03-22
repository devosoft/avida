//
//  AvidaEDController.mm
//  Avida
//
//  Created by David on 11/30/10.
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

#import "AvidaEDController.h"

#import "AvidaAppDelegate.h"
#import "AvidaRun.h"
#import "MapGridView.h"

#include "avida/viewer-core/cMap.h"

static const float MAIN_SPLIT_LEFT_MIN = 150.0;
static const float MAIN_SPLIT_RIGHT_MIN = 550.0;
static const float MAIN_SPLIT_LEFT_PROPORTIONAL_RESIZE = 0.5;
static const float POP_SPLIT_LEFT_MIN = 230.0;
static const float POP_SPLIT_RIGHT_MIN = 300.0;
static const float POP_SPLIT_LEFT_PROPORTIONAL_RESIZE = 0.3;

@implementation AvidaEDController

- (id)initWithAppDelegate: (AvidaAppDelegate*)delegate {
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
      if (!listener) listener = new cMainThreadListener(self);
      [currentRun attachListener:self];
      
      [txtUpdate setStringValue:@"Time (updates): 0"];
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
  }
  
}

- (IBAction) changeMapZoom:(id)sender {
  if (map) {
    [mapView setZoom:[mapZoom doubleValue]];
  }
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
    
    float dividerThickness = [splitView dividerThickness];
    
    int diffWidth = newFrame.size.width - oldSize.width;
    
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    if ((rightFrame.size.width <= POP_SPLIT_RIGHT_MIN && diffWidth < 0) || diffWidth > 0) {
      leftFrame.size.width += diffWidth;
    } else if (leftFrame.size.width > POP_SPLIT_LEFT_MIN) {
      leftFrame.size.width += diffWidth * POP_SPLIT_LEFT_PROPORTIONAL_RESIZE;
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


-(CGFloat) splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMax ofSubviewAt:(NSInteger)index {
  if (splitView == mainSplitView) return proposedMax - MAIN_SPLIT_RIGHT_MIN;
  else if (splitView == popSplitView) return proposedMax - POP_SPLIT_RIGHT_MIN;
  return proposedMax;
}


-(CGFloat) splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMin ofSubviewAt:(NSInteger)index {
  if (splitView == mainSplitView) return proposedMin + MAIN_SPLIT_LEFT_MIN;
  else if (splitView == popSplitView) return proposedMin + POP_SPLIT_LEFT_MIN;
  
  return proposedMin;
}

- (void) windowWillClose: (NSNotification*)notification {
  if (currentRun != nil) {
    [currentRun end];
    currentRun = nil;
  }
  [app removeWindow:self];
}


@synthesize listener;


- (void) handleMap: (CoreViewMap*)pkg {
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
  [mapZoom setDoubleValue:[mapView zoom]];
}


- (void) handleUpdate: (CoreViewUpdate*)pkg {
  NSString* str = [NSString stringWithFormat:@"Update: %d", [pkg update]];
  [txtUpdate setStringValue:str]; 
}


@end
