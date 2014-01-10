//
//  MapGridView.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 11/23/10.
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

#import "MapGridView.h"

#include "avida/viewer/Map.h"

#import "Freezer.h"

#include <cassert>
#include <iostream>


static inline CGFloat sigmoid(CGFloat x, CGFloat midpoint, CGFloat steepness)
{
  CGFloat val = steepness * (x - midpoint);
  return exp(val) / (1.0 + exp(val));
}


@interface MapGridView (hidden) {  
}
- (void) setup;
- (void) adjustZoom;
- (void) updateColorCache;
@end

@implementation MapGridView (hidden)
- (void) setup {
  map_width = 0;
  map_height = 0;
  num_colors = 0;
  color_cache = [NSMutableArray arrayWithCapacity:255];
  zoom = -1;
  selected_x = -1;
  selected_y = -1;
  
  [self registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, ACPasteboardTypeGenome, nil]];
}

- (void)adjustZoom {
  if (zoom < 0) {
    NSScrollView* scrollView = [self enclosingScrollView];
    assert(scrollView != nil);
    
    NSSize bounds = [scrollView bounds].size;
    double z1 = bounds.width / map_width;
    double z2 = bounds.height / map_height;
    double zval = (z1 > z2) ? z2 : z1;
    if (zval > 15.0) zval = 15.0;
    zval = floor(zval);
    [self setZoom:zval];
  } else {
    CGFloat block_size = zoom;
    CGFloat grid_width = (block_size > 5.0) ? 1.0 : 0.0;

    NSSize mapSize;
    mapSize.width = map_width * block_size - grid_width;
    mapSize.height = map_height * block_size - grid_width;    
    [self setFrameSize:mapSize];

    [self setNeedsDisplay:YES];
  }  
}

- (void) updateColorCache {
  if (num_colors != [color_cache count]) {
    [color_cache removeAllObjects];
    if (num_colors == 10) {
      [color_cache insertObject:[NSColor colorWithCalibratedRed:0.0f green:0.7f blue:0.0f alpha:1.0f] atIndex:0]; // greenColor
      [color_cache insertObject:[NSColor redColor] atIndex:1];
      [color_cache insertObject:[NSColor blueColor] atIndex:2];
      [color_cache insertObject:[NSColor colorWithCalibratedRed:0.0f green:0.7f blue:1.0f alpha:1.0f] atIndex:3]; // cyanColor
      [color_cache insertObject:[NSColor yellowColor] atIndex:4];
      [color_cache insertObject:[NSColor magentaColor] atIndex:5];
      [color_cache insertObject:[NSColor orangeColor] atIndex:6];
      [color_cache insertObject:[NSColor purpleColor] atIndex:7];
      [color_cache insertObject:[NSColor brownColor] atIndex:8];
      [color_cache insertObject:[NSColor colorWithCalibratedWhite:0.8f alpha:1.0f] atIndex:9];
    } else {
      for (int i = 0; i < num_colors; i++) {
        CGFloat x = 0.1 + 0.8 * (static_cast<CGFloat>(i) / (num_colors - 1));
        CGFloat h = fmod((x + .27), 1.0);
        CGFloat s = sigmoid(1.0 - x, 0.1, 30);
        CGFloat b = sigmoid(x, 0.3, 10);
        [color_cache insertObject:[NSColor colorWithCalibratedHue:h saturation:s brightness:b alpha:1.0] atIndex:i];
      }
    }
  }
}

@end



@implementation MapGridView

- (id) initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    [self setup];
  }
  return self;
}


- (void) awakeFromNib {
  
  [self setup];
}


- (void) drawRect:(NSRect)dirtyRect {
  [[NSColor lightGrayColor] set];
  [NSBezierPath fillRect:dirtyRect];
  
  [self updateColorCache];
  
  CGFloat block_size = zoom;
  CGFloat grid_width = (block_size > 5.0) ? 1.0 : 0.0;
  
  // Determine Map Dimensions
  NSRect mapRect;
  mapRect.size.width = map_width * block_size - grid_width;
  mapRect.size.height = map_height * block_size - grid_width;  
  mapRect.origin = NSMakePoint(0, 0);
  
  [[NSColor blackColor] set];
  [NSBezierPath fillRect:mapRect];
  
  
  NSRect gridCellRect;
  gridCellRect.size.width = block_size - grid_width;
  gridCellRect.size.height = block_size - grid_width;

  for (int i = 0; i < map_width; i++) {
    for (int j = 0; j < map_height; j++) {
      gridCellRect.origin = NSMakePoint(mapRect.origin.x + block_size * i, mapRect.origin.y + block_size * j);
      int color = (pending_colors[i + j * map_width] != 0) ? pending_colors[i + j * map_width] : map_colors[i + j * map_width];
      switch (color) {
        case -4:  break;
        case -3:  [[NSColor darkGrayColor] set]; [NSBezierPath fillRect:gridCellRect]; break;
        case -2:  [[NSColor grayColor] set]; [NSBezierPath fillRect:gridCellRect]; break;
        case -1:  [[NSColor whiteColor] set]; [NSBezierPath fillRect:gridCellRect];break;
        default:  [(NSColor*)[color_cache objectAtIndex:color] set]; [NSBezierPath fillRect:gridCellRect]; break;
      }
      
      if (i == selected_x && j == selected_y) {
        // Handle selected cell preferentially to tags
        [[NSColor greenColor] set];
      } else {
        // Handle tag coloration
        int tag = map_tags[i + j * map_width];
        switch (tag) {
          case -4:  continue;
          case -3:  [[NSColor darkGrayColor] set]; break;
          case -2:  [[NSColor grayColor] set]; break;
          case -1:  [[NSColor cyanColor] set]; break;
          default:  continue;
        }
      }
      // Draw tag outline
      [NSGraphicsContext saveGraphicsState];
      NSBezierPath* tagPath = [NSBezierPath bezierPathWithRect:gridCellRect];
      [tagPath setLineWidth:2.0];
      [tagPath setLineCapStyle:NSSquareLineCapStyle];
      [tagPath addClip];
      [tagPath stroke];
      [NSGraphicsContext restoreGraphicsState];
    }
  }
}


- (BOOL) isOpaque {
  return YES;
}

- (void) setDimensions:(NSSize)size {
  map_width = size.width;
  map_height = size.height;
  map_colors.Resize(map_width * map_height);
  map_colors.SetAll(-4);
  pending_colors.Resize(map_width * map_height);
  pending_colors.SetAll(0);
  map_tags.Resize(map_width * map_height);
  map_tags.SetAll(-4);
  [self adjustZoom];
}


- (void) updateState:(Avida::Viewer::Map*)state {
  state->Retain();
  
  map_width = state->GetWidth();
  map_height = state->GetHeight();
  
  map_colors = state->GetColors();
  pending_colors.Resize(map_colors.GetSize());
  pending_colors.SetAll(0);
  num_colors = state->GetColorScale().GetScaleRange();
  
  map_tags = state->GetTags();

  state->Release();
  
  [self adjustZoom];
}

- (void) clearMap {
  map_width = 0;
  map_height = 0;
  num_colors = 0;
  [color_cache removeAllObjects];
  map_colors.ResizeClear(0);
  pending_colors.ResizeClear(0);
  map_tags.ResizeClear(0);
  zoom = -1;
  selected_x = -1;
  selected_y = -1;
  [self setNeedsDisplay:YES];
}


- (void) setPendingActionAtX:(int)x Y:(int)y withColor:(int)color {
  assert(x < map_width);
  assert(y < map_height);
  
  pending_colors[x + y * map_width] = color;
  [self setNeedsDisplay:YES];
}

- (void) clearPendingActions {
  pending_colors.SetAll(0);
}

- (void) mouseDown:(NSEvent*)event {
  if (selectionDelegate != nil && map_width > 0 && map_height > 0) {
    // convert the mouse-down location into the view coords
    NSPoint clickLocation = [self convertPoint:[event locationInWindow] fromView:nil];
    
    CGFloat block_size = zoom;
    
    NSPoint selectedOrg;
    selectedOrg.x = floor(clickLocation.x / block_size);
    selectedOrg.y = floor(clickLocation.y / block_size);
    
    if (selected_x != selectedOrg.x || selected_y != selectedOrg.y) {
      if (selectionDelegate == nil ||
          ![selectionDelegate respondsToSelector:@selector(mapView:shouldSelectObjectAtPoint:)] ||
          [selectionDelegate mapView:self shouldSelectObjectAtPoint:selectedOrg]) {
        selected_x = selectedOrg.x;
        selected_y = selectedOrg.y;
        [selectionDelegate mapViewSelectionChanged:self];
        [self setNeedsDisplay:YES];
      }
    }
  }
}


- (void) mouseDragged:(NSEvent*)event {
  if (selected_x < 0 || selected_y < 0) return;
  if (selectionDelegate != nil && [selectionDelegate respondsToSelector:@selector(mapView:writeSelectionToPasteboard:)]) {
    NSImage* image = [[NSImage alloc] initWithSize:NSMakeSize(40.0, 40.0)];
    [image lockFocus];
    [[self colorOfX:selected_x Y:selected_y] set];
    [NSBezierPath fillRect:NSMakeRect(0, 0, 40, 40)];
    [image unlockFocus];
    
    
    NSPasteboard* pboard = [NSPasteboard pasteboardWithName:(NSString*)NSDragPboard];
    [pboard clearContents];
    
    if (![selectionDelegate mapView:self writeSelectionToPasteboard:pboard]) return;

    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    location.y -= 40;

    [self dragImage:image at:location offset:NSMakeSize(0,0) event:event pasteboard:pboard source:selectionDelegate slideBack:YES];
  }
}


- (NSColor*) colorOfX:(int)x Y:(int)y {
  [self updateColorCache];
  int color = map_colors[x + (y * map_width)];
  switch (color) {
    case -4:  return [NSColor blackColor];
    case -3:  return [NSColor darkGrayColor];
    case -2:  return [NSColor grayColor];
    case -1:  return [NSColor whiteColor];
    default:  return (NSColor*)[color_cache objectAtIndex:color];
  }
  return [NSColor blackColor];
}

@synthesize zoom;
- (void) setZoom:(double)zval {
  zoom = round(zval);
  
  CGFloat block_size = zoom;
  CGFloat grid_width = (block_size > 5.0) ? 1.0 : 0.0;

  NSSize mapSize;
  mapSize.width = map_width * block_size - grid_width;
  mapSize.height = map_height * block_size - grid_width;
  
  [self setFrameSize:mapSize];
  
  [self setNeedsDisplay:YES];
}

@synthesize selectionDelegate;


- (NSPoint) selectedObject {
  return NSMakePoint(selected_x, selected_y);
}

- (void) setSelectedObject:(NSPoint)point {
  int new_x = floor(point.x);
  int new_y = floor(point.y);
  if (new_x < map_width && new_x >= 0 && new_y < map_height && new_y >= 0) {
    if (new_x != selected_x || new_y != selected_y) {
      selected_x = floor(point.x);
      selected_y = floor(point.y);
      [selectionDelegate mapViewSelectionChanged:self];
      [self setNeedsDisplay:YES];
    }
  }
}

- (void) clearSelectedObject {
  if (selected_x != -1) {
    selected_x = -1;
    selected_y = -1;
    [selectionDelegate mapViewSelectionChanged:self];
    [self setNeedsDisplay:YES];
  }
}


- (void) setFrame:(NSRect)frameRect {
  [super setFrame:frameRect];
}

- (void) setFrameOrigin:(NSPoint)newOrigin {
  [super setFrameOrigin:newOrigin];
}

- (void) setFrameSize:(NSSize)newSize {
  [super setFrameSize:newSize];
}

- (void) setFrameRotation:(CGFloat)angle {
  [super setFrameRotation:angle];
}

// NSDraggingDestination
- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender
{
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    if (sourceDragMask & NSDragOperationGeneric) {
      return NSDragOperationGeneric;
    }
  }
  
  if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
    if (sourceDragMask & NSDragOperationCopy) {
      return NSDragOperationCopy;
    }
  }
  
  return NSDragOperationNone;
}

- (NSDragOperation) draggingUpdated:(id<NSDraggingInfo>)sender
{
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    if (sourceDragMask & NSDragOperationGeneric) {
      return NSDragOperationGeneric;
    }
  }
  if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
    if (sourceDragMask & NSDragOperationCopy) {
      return NSDragOperationCopy;
    }
  }
  
  return NSDragOperationNone;
}

- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender
{
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    if (sourceDragMask & NSDragOperationGeneric) {
      return YES;
    }
  }
  if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
    if (sourceDragMask & NSDragOperationCopy) {
      return YES;
    }
  }
  
  return NO;
}

- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender
{
  NSPasteboard* pboard = [sender draggingPasteboard];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
    switch (fid.type) {
      case Avida::Viewer::CONFIG: [dragDelegate mapView:self handleDraggedConfig:fid]; break;
      case Avida::Viewer::GENOME:
        {
          NSPoint location = [self convertPoint:[sender draggingLocation] fromView:nil];
          CGFloat block_size = zoom;
          
          int x = floor(location.x / block_size);
          int y = floor(location.y / block_size);
          [dragDelegate mapView:self handleDraggedFreezerGenome:fid atX:x Y:y];
        }        
        break;
      case Avida::Viewer::WORLD:  [dragDelegate mapView:self handleDraggedWorld:fid]; break;
      default: break;
    }
  }
  if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
    ACGenome* genome = [ACGenome genomeFromPasteboard:pboard];
    if (genome != nil) {
      NSPoint location = [self convertPoint:[sender draggingLocation] fromView:nil];
      CGFloat block_size = zoom;
      
      int x = floor(location.x / block_size);
      int y = floor(location.y / block_size);
      [dragDelegate mapView:self handleDraggedGenome:genome atX:x Y:y];
    }
  }
  
  return YES;
}

- (BOOL) wantsPeriodicDraggingUpdates
{
  return NO;
}


- (NSDragOperation) draggingSession:(NSDraggingSession*)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context {
  switch (context) {
    case NSDraggingContextWithinApplication:
      return NSDragOperationCopy;
      
    case NSDraggingContextOutsideApplication:
    default:
      return NSDragOperationNone;
      break;
  }
}

- (BOOL) ignoreModifierKeysForDraggingSession:(NSDraggingSession*)session
{
  return YES;
}

@end
