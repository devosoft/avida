//
//  TaskTimelineView.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 8/27/12.
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

#import "TaskTimelineView.h"


@implementation TaskTimelineViewEntry

@synthesize location, label;

- (TaskTimelineViewEntry*) initWithLabel:(NSString*)in_label atLocation:(int)in_location {
  
  label = in_label;
  location = in_location;
  
  return self;
}

@end


@implementation TaskTimelineView

- (id) initWithFrame:(NSRect)frame
{
  self = [super initWithFrame:frame];
  if (self) {
    length = 0;
    entries = [[NSMutableSet alloc] init];
  }
  
  return self;
}

- (void) drawRect:(NSRect)dirtyRect {
  
  const int TICK_MARKS = 6;
  
  [[NSColor whiteColor] set];
  [NSBezierPath fillRect:dirtyRect];
  
  if (length > 0) {
    NSBezierPath* path = [NSBezierPath bezierPath];
    
    CGFloat lr_padding = 10.0f;
    CGFloat axis_height = 20.0f;
    CGFloat tick_length = 5.0f;
    CGFloat cur_point_width = 6.0;
    CGFloat bounds_width = self.bounds.size.width;
    
    // Set up axis line
    NSPoint point = NSMakePoint(lr_padding, axis_height);
    [path moveToPoint:point];
    point.x = bounds_width - lr_padding;
    [path lineToPoint:point];
    
    // start tick mark
    point.x = lr_padding;
    [path moveToPoint:point];
    point.y = axis_height - tick_length;
    [path lineToPoint:point];
    
    for (int i = 1; i < TICK_MARKS; i++) {
      // middle major tick mark(s)
      point.x = round(i * (bounds_width - lr_padding - lr_padding) / TICK_MARKS) + lr_padding;
      point.y = axis_height - tick_length;
      [path moveToPoint:point];
      point.y = axis_height;
      [path lineToPoint:point];
    }
    
    for (int i = 1; i <= TICK_MARKS; i++) {
      // minor tick mark(s)
      point.x = round(((CGFloat)i - 0.5f) * (bounds_width - lr_padding - lr_padding) / TICK_MARKS) + lr_padding;
      point.y = axis_height - tick_length;
      [path moveToPoint:point];
      point.y = axis_height;
      [path lineToPoint:point];
    }

    
    // end tick mark
    point.x = bounds_width - lr_padding;
    point.y = axis_height - tick_length;
    [path moveToPoint:point];
    point.y = axis_height;
    [path lineToPoint:point];
    
    // Draw axis path
    [[NSColor blackColor] set];
    [path stroke];
    
    
    NSFont* font = [NSFont fontWithName:@"Lucida Grande" size:12.0];
    NSDictionary* str_attributes = [NSDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
    NSString* lbl_str;
    CGFloat offset;
    

    // Draw start label
    lbl_str = @"0";
    offset = -[lbl_str sizeWithAttributes:str_attributes].width / 2.0f;
    point.y = axis_height - tick_length - 1.0f - [lbl_str sizeWithAttributes:str_attributes].height;
    point.x = lr_padding + offset;
    [lbl_str drawAtPoint:point withAttributes:str_attributes];
    
    for (int i = 1; i < TICK_MARKS; i++) {
      // Draw middle label(s)
      lbl_str = [NSString stringWithFormat:@"%d", (int)(i * length / TICK_MARKS)];
      offset = -[lbl_str sizeWithAttributes:str_attributes].width / 2.0f;
      point.x = round(i * (bounds_width - lr_padding - lr_padding) / TICK_MARKS) + lr_padding + offset;
      [lbl_str drawAtPoint:point withAttributes:str_attributes];
    }
    
    // Draw end label
    lbl_str = [NSString stringWithFormat:@"%d", length];
    offset = -[lbl_str sizeWithAttributes:str_attributes].width / 2.0f;
    point.x = bounds_width - lr_padding + offset;
    [lbl_str drawAtPoint:point withAttributes:str_attributes];
    
    // Draw labeled locations
    path = [NSBezierPath bezierPath];
    for (TaskTimelineViewEntry* entry in entries) {
      // Set up tick path
      point.y = axis_height;
      point.x = round(entry.location * (bounds_width - lr_padding - lr_padding) / length) + lr_padding;
      [path moveToPoint:point];
      point.y += tick_length;
      [path lineToPoint:point];

      // Draw label
      offset = -[entry.label sizeWithAttributes:str_attributes].width / 2.0f;
      point.y += 1.0f;
      point.x += offset;
      [entry.label drawAtPoint:point withAttributes:str_attributes];
    }
    [path stroke]; // draw actual tick marks
    
    if (currentPoint >= 0) {
      NSRect currentPointRect = NSMakeRect(round(currentPoint * (bounds_width - lr_padding - lr_padding) / length) + lr_padding - (cur_point_width / 2.0f), axis_height - (cur_point_width / 2.0f), cur_point_width, cur_point_width);
      path = [NSBezierPath bezierPathWithOvalInRect:currentPointRect];
      [[NSColor redColor] set];
      [path fill];
    }
  }
}


- (void) addEntryWithLabel:(NSString*)label atLocation:(int)location {
  [entries addObject:[[TaskTimelineViewEntry alloc] initWithLabel:label atLocation:location]];
}


- (void) clearEntries {
  [entries removeAllObjects];
}


- (int) length {
  return length;
}

- (void) setLength:(int)in_length {
  length = in_length;
  [self setNeedsDisplay:YES];
}

- (int) currentPoint {
  return currentPoint;
}

- (void) setCurrentPoint:(int)in_point {
  currentPoint = in_point;
  [self setNeedsDisplay:YES];
}

@end
