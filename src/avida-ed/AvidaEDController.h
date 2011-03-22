//
//  AvidaEDController.h
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

#import <Cocoa/Cocoa.h>

#include "apto/core.h"

#import "CoreViewListener.h"


@class AvidaAppDelegate;
@class AvidaRun;
@class MapGridView;


@interface AvidaEDController : NSWindowController <CoreViewListener, NSSplitViewDelegate, NSWindowDelegate> {
  IBOutlet NSView* analyzeView;
  IBOutlet NSView* orgView;
  IBOutlet NSView* popView;
  
  IBOutlet NSButton* btnRunState;
  IBOutlet NSTextField* txtUpdate;
  IBOutlet MapGridView* mapView;
  IBOutlet NSPopUpButton* mapViewMode;
  IBOutlet NSSlider* mapZoom;
  IBOutlet NSSplitView* mainSplitView;
  IBOutlet NSSplitView* popSplitView;
  
  AvidaAppDelegate* app;
  
  AvidaRun* currentRun;
  Avida::CoreView::cListener* listener;
  Avida::CoreView::cMap* map;
  Apto::Map<NSInteger, int> map_mode_to_color;
}

// Init and Dealloc Methods
-(id)initWithAppDelegate:(AvidaAppDelegate*)delegate;

-(void)dealloc;
-(void)finalize;


// NSWindowController Methods
-(void)windowDidLoad;


// Actions
-(IBAction)toggleRunState:(id)sender;
-(IBAction)changeMapViewMode:(id)sender;
-(IBAction)changeMapZoom:(id)sender;


// NSSplitViewDelegate Protocol
-(void)splitView:(NSSplitView*)splitView resizeSubviewsWithOldSize:(NSSize)oldSize;
-(BOOL)splitView:(NSSplitView*)splitView canCollapseSubview:(NSView*)subview;
-(CGFloat)splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMax ofSubviewAt:(NSInteger)index;
-(CGFloat)splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMin ofSubviewAt:(NSInteger)index;


// NSWindowDelegate Protocol
-(void)windowWillClose:(NSNotification*)notification;


// Listener Methods
@property (readonly) Avida::CoreView::cListener* listener;

-(void)handleMap:(CoreViewMap*)object;
-(void)handleUpdate:(CoreViewUpdate*)object;


@end
