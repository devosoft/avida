//
//  OrganismView.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 3/5/12.
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

#import "OrganismView.h"

#import <AvidaCore/AvidaCore.h>

#include "avida/viewer/GraphicsContext.h"
#include "avida/viewer/OrganismTrace.h"


@implementation OrganismView

@synthesize dropDelegate;

- (id)initWithFrame:(NSRect)frame
{
  self = [super initWithFrame:frame];
  if (self) {
    // Initialization code here.
    snapshot = NULL;
  }
  
  return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
//  [[NSColor darkGrayColor] set];
  [[NSColor whiteColor] set];
  [NSBezierPath fillRect:dirtyRect];
  
  if (snapshot) {
    NSRect bounds = [self bounds];
    NSPoint centerPoint = NSMakePoint(NSMidX(bounds), NSMidY(bounds));
    
    Avida::Viewer::ConstGraphicPtr graphic = snapshot->GraphicForContext(graphics_context);
    
    for (int object_idx = 0; object_idx < graphic->NumObjects(); object_idx++) {
      const Avida::Viewer::GraphicObject& obj = graphic->Object(object_idx);
      
      switch (obj.shape) {
        case Avida::Viewer::SHAPE_OVAL:
        {
          NSRect objRect = NSMakeRect(centerPoint.x + (obj.x * 72.0), centerPoint.y + (obj.y * 72.0), obj.width * 72.0, obj.height * 72.0);
          
          // Draw the Oval in the calculated rect
          if (obj.fill_color.a > 0.0 || obj.line_color.a > 0) {
            NSBezierPath* path = [NSBezierPath bezierPathWithOvalInRect:objRect];
            
            if (obj.fill_color.a > 0.0) {
              [[NSColor colorWithSRGBRed:obj.fill_color.r green:obj.fill_color.g blue:obj.fill_color.b alpha:obj.fill_color.a] set];
              [path fill];
            }
            if (obj.line_color.a > 0.0) {
              [[NSColor colorWithSRGBRed:obj.line_color.r green:obj.line_color.g blue:obj.line_color.b alpha:obj.line_color.a] set];
              [path setLineWidth:obj.line_width];
              [path stroke];              
            }
          }
          
          // Draw the label string in the center of the calculated rect
          if (obj.label.GetSize() && obj.label_color.a > 0.0) {
            // Set up string attributes for the label
            NSFont* font = [NSFont fontWithName:@"Helvetica" size:(12.0 * obj.font_size)];
            NSColor* color = [NSColor colorWithSRGBRed:obj.label_color.r green:obj.label_color.g blue:obj.label_color.b
                                                 alpha:obj.label_color.a];
            NSDictionary* str_attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                            font, NSFontAttributeName, color, NSForegroundColorAttributeName, nil];
            
            NSString* lbl = [NSString stringWithAptoString:obj.label];
            CGFloat offset_x = [lbl sizeWithAttributes:str_attributes].width / 2.0;
            CGFloat offset_y = [lbl sizeWithAttributes:str_attributes].height / 2.0;
            NSPoint lbl_location = NSMakePoint(NSMidX(objRect) - offset_x, NSMidY(objRect) - offset_y);
            [lbl drawAtPoint:lbl_location withAttributes:str_attributes];
          }
        }
          break;
        case Avida::Viewer::SHAPE_RECT:
        {
          
        }
          break;
        case Avida::Viewer::SHAPE_CURVE:
        {
          NSBezierPath* path = [NSBezierPath bezierPath];
          NSPoint startPoint = NSMakePoint(centerPoint.x + obj.x * 72.0, centerPoint.y + obj.y * 72.0);
          NSPoint endPoint = NSMakePoint(centerPoint.x + obj.x2 * 72.0, centerPoint.y + obj.y2 * 72.0);
          NSPoint ctrlPoint1 = NSMakePoint(centerPoint.x + obj.ctrl_x * 72.0, centerPoint.y + obj.ctrl_y * 72.0);
          NSPoint ctrlPoint2 = NSMakePoint(centerPoint.x + obj.ctrl_x2 * 72.0, centerPoint.y + obj.ctrl_y2 * 72.0);
          
          [path moveToPoint:startPoint];
          [path curveToPoint:endPoint controlPoint1:ctrlPoint1 controlPoint2:ctrlPoint2];
          
          [[NSColor colorWithSRGBRed:obj.line_color.r green:obj.line_color.g blue:obj.line_color.b alpha:obj.line_color.a] set];
          [path setLineWidth:obj.line_width];
          [path stroke];
        }
          break;
        default:
          break;
      }
      
    }
  }
}


- (void) resizeSubviewsWithOldSize:(NSSize)oldBoundsSize {
  
  switch (self.subviews.count) {
    case 0:
      break;
      
    case 1:
    {
      NSView* view = [self.subviews objectAtIndex:0];
      Avida::Viewer::ConstGraphicPtr graphic = snapshot->GraphicForContext(graphics_context);
      
      [view setFrameOrigin:NSMakePoint(-view.frame.size.width, -view.frame.size.height)];
      
      for (int object_idx = 0; object_idx < graphic->NumObjects(); object_idx++) {
        const Avida::Viewer::GraphicObject& obj = graphic->Object(object_idx);
        if (obj.active_region_id == 1) {
          NSPoint centerPoint = NSMakePoint(NSMidX(self.bounds), NSMidY(self.bounds));
          NSPoint objOrigin = NSMakePoint(centerPoint.x + (obj.x * 72.0) - (view.bounds.size.width / 2), centerPoint.y + (obj.y * 72.0) - (view.bounds.size.height / 2));
          [view setFrameOrigin:objOrigin];
          return;
        }
      }
    }
      break;
    default:
      assert(false); // Wha??  only expect one subview for now
  }
}



- (const Avida::Viewer::HardwareSnapshot*) snapshot {
  return snapshot;
}

- (void) setSnapshot:(const Avida::Viewer::HardwareSnapshot*)new_snapshot {
  snapshot = new_snapshot;
  [self setNeedsDisplay:YES];
}


// NSDraggingDestination
- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(draggingEnteredDestination:sender:)]) {
    return [dropDelegate draggingEnteredDestination:self sender:sender];
  }
  return NSDragOperationNone;
}

- (NSDragOperation) draggingUpdated:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(draggingUpdatedForDestination:sender:)]) {
    return [dropDelegate draggingUpdatedForDestination:self sender:sender];
  }
  return NSDragOperationNone;
}

- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(prepareForDragOperationForDestination:sender:)]) {
    return [dropDelegate prepareForDragOperationForDestination:self sender:sender];
  }
  return NO;
}

- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(performDragOperationForDestination:sender:)]) {
    return [dropDelegate performDragOperationForDestination:self sender:sender];
  }
  return NO;
}

- (BOOL) wantsPeriodicDraggingUpdates {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(wantsPeriodicDraggingUpdatesForDestination:)]) {
    return [dropDelegate wantsPeriodicDraggingUpdatesForDestination:self];
  }
  return NO;
}


@end
