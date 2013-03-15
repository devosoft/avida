//
//  ACMapView.h
//  AvidaCore-ObjC
//
//  Created by David Bryson on 3/15/13.
//  Copyright 2013 Michigan State University. All rights reserved.
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

#import <Cocoa/Cocoa.h>
#import <Apto/Apto.h>

#import <AvidaCore/ACProject.h>

#include "avida/viewer.h"


@class ACGenome;
@class ACMapView;


// ACMapSelectionDelegate Protocol
// --------------------------------------------------------------------------------------------------------------

@protocol ACMapSelectionDelegate <NSObject>
@optional
- (BOOL) mapView:(ACMapView*)mapView shouldSelectObjectAtPoint:(NSPoint)point;
- (void) mapViewSelectionChanged:(ACMapView*)mapView;
- (BOOL) mapView:(ACMapView*)mapView writeSelectionToPasteboard:(NSPasteboard*)pboard;
@end


// ACMapDropDelegate Protocol
// --------------------------------------------------------------------------------------------------------------

@protocol ACMapDropDelegate <NSObject>
@required
- (void) mapView:(ACMapView*)map handleDroppedProjectItem:(ACProjectItem*)item at:(NSPoint)point;
- (void) mapView:(ACMapView*)map handleDroppedGenome:(ACGenome*)genome at:(NSPoint)point;
@end



// ACMapView Interface
// --------------------------------------------------------------------------------------------------------------

@interface ACMapView : NSView <NSDraggingDestination> {
  int map_width;
  int map_height;
  int num_colors;
  double zoom;
  
  Apto::Array<int> map_colors;
  Apto::Array<int> pending_colors;
  Apto::Array<int> map_tags;
  NSMutableArray* color_cache;
  
  IBOutlet id<ACMapDropDelegate> dragDelegate;
  IBOutlet id<ACMapSelectionDelegate> selectionDelegate;
  int selected_x;
  int selected_y;
}

@property (readwrite, nonatomic) double zoom;
@property (readwrite, retain) id<ACMapSelectionDelegate> selectionDelegate;
@property (readwrite, nonatomic) NSPoint selectedObject;


// Initalization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Initialization

- (id) initWithFrame:(NSRect)frame;
- (void) awakeFromNib;


// Drawing
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Drawing

- (void) drawRect:(NSRect)rect;
- (BOOL) isOpaque;


// State Alteration
// --------------------------------------------------------------------------------------------------------------
#pragma mark - State Alteration

- (void) setDimensions:(NSSize)size;
- (void) updateState:(Avida::Viewer::Map*)state;
- (void) clearMap;

- (void) setPendingActionAt:(NSPoint)point withColor:(int)color;
- (void) clearPendingActions;


- (NSColor*) colorAt:(NSPoint)point;
- (void) clearSelectedObject;


// Interaction
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Interaction

- (void) mouseDown:(NSEvent*)event;
- (void) mouseDragged:(NSEvent*)event;


// NSDraggingDestination
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSDraggingDestination

- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender;
- (NSDragOperation) draggingUpdated:(id<NSDraggingInfo>)sender;
- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender;
- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender;
- (BOOL) wantsPeriodicDraggingUpdates;


// --------------------------------------------------------------------------------------------------------------
@end
