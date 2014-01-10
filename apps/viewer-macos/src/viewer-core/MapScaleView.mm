//
//  MapScaleView.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 4/14/11.
//  Copyright 2011 Michigan State University. All rights reserved.
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

#import "MapScaleView.h"


static inline CGFloat sigmoid(CGFloat x, CGFloat midpoint, CGFloat steepness)
{
  CGFloat val = steepness * (x - midpoint);
  return exp(val) / (1.0 + exp(val));
}


@implementation MapScaleView

@synthesize willExport;

- (id) initWithFrame:(NSRect)frame
{
  self = [super initWithFrame:frame];
  if (self) {
    num_colors = 0;
    color_cache = [NSMutableArray arrayWithCapacity:255];
    scale_label = nil;
  }
  
  return self;
}

- (void) awakeFromNib {
  num_colors = 0;
  color_cache = [NSMutableArray arrayWithCapacity:255];
  scale_label = nil;
}



- (void) drawRect:(NSRect)dirtyRect
{
  [[NSColor lightGrayColor] set];
  [NSBezierPath fillRect:dirtyRect];

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
  
  NSRect bounds = (willExport) ? dirtyRect : [self bounds];
  NSFont* font = [NSFont fontWithName:@"Lucida Grande" size:12.0];
  NSDictionary* str_attributes = [NSDictionary dictionaryWithObject:font forKey:NSFontAttributeName];

  if (is_categorical) {
    int colors_per_row = (num_colors / 2) + (num_colors % 2);
    CGFloat color_width = round((bounds.size.width - 20.0) / colors_per_row);
    CGFloat color_height = round((bounds.size.height - 30.0) / 2.0);
    
    NSRect colorRect;
    colorRect.size.width = 20.0;
    colorRect.size.height = color_height;
    colorRect.origin.x = 10.0;
    colorRect.origin.y = 20.0 + color_height;
    
    int activeCount = 0;
    for (int i = 0; i < num_colors; i++) {
      if (activeCount == colors_per_row) {
        colorRect.origin.x = 10.0;
        colorRect.origin.y = 10.0;
      }
      
      if (color_count[i + Avida::Viewer::MAP_RESERVED_COLORS] != 0) {
        [(NSColor*)[color_cache objectAtIndex:i] set];
        [NSBezierPath fillRect:colorRect];
        NSString* lbl = [NSString stringWithUTF8String:(const char*)scale_entries[i + Avida::Viewer::MAP_RESERVED_COLORS].label];
        CGFloat offset = -[lbl sizeWithAttributes:str_attributes].height / 2.0;
        NSPoint lbl_location = NSMakePoint(colorRect.origin.x + 22.0, colorRect.origin.y + (colorRect.size.height / 2.0) + offset);
        [lbl drawAtPoint:lbl_location withAttributes:str_attributes];

        colorRect.origin.x += color_width;
        activeCount++;
      }

    }
  } else {
    NSRect scaleRect;
    scaleRect.size.width = bounds.size.width - 40.0;
    scaleRect.size.height = bounds.size.height - 40.0;
    scaleRect.origin.x = 20.0;
    scaleRect.origin.y = 20.0;

    NSGradient* scale_gradient = [[NSGradient alloc] initWithColors:color_cache];
    [scale_gradient drawInRect:scaleRect angle:0.0];
    
    for (int i = 0; i < scale_entries.GetSize(); i++) {
      NSString* lbl = [NSString stringWithUTF8String:(const char*)scale_entries[i].label];
      CGFloat offset = -[lbl sizeWithAttributes:str_attributes].width / 2.0;
      NSPoint lbl_location = NSMakePoint(scaleRect.origin.x + offset + scale_entries[i].index * scaleRect.size.width / num_colors,
                                         scaleRect.origin.y + scaleRect.size.height + 2.0);
      [lbl drawAtPoint:lbl_location withAttributes:str_attributes];
    }
  }

  if (scale_label != nil && !is_categorical) {
    CGFloat offset = -[scale_label sizeWithAttributes:str_attributes].width / 2.0;
    NSPoint lbl_location = NSMakePoint(bounds.size.width / 2.0 + bounds.origin.x + offset, 6.0);
    [scale_label drawAtPoint:lbl_location withAttributes:str_attributes];
  }
}


- (void) updateState:(Avida::Viewer::Map*)state {
  state->Retain();
  
  num_colors = state->GetColorScale().GetScaleRange();
  is_categorical = state->GetColorScale().IsCategorical();
  
  int num_entries = state->GetColorScale().GetNumLabeledEntries();
  scale_entries.Resize(num_entries);
  for (int i = 0; i < num_entries; i++) {
    scale_entries[i] = state->GetColorScale().GetEntry(i);
  }
  color_count = state->GetColorCounts();
  
  scale_label = [NSString stringWithUTF8String:(const char*)state->GetColorScaleLabel()];
  
  state->Release();
  
  [self setNeedsDisplay:YES];
}

- (void) setTempState:(NSArray*)entries {
  if (entries != nil) {
    num_colors = 10;
    scale_entries.Resize(num_colors + Avida::Viewer::MAP_RESERVED_COLORS);
    
    for (int i = 0; i < entries.count; i++) {
      scale_entries[i + Avida::Viewer::MAP_RESERVED_COLORS].index = i + 1;
      scale_entries[i + Avida::Viewer::MAP_RESERVED_COLORS].label = [(NSString*)[entries objectAtIndex:(i)] UTF8String];
    }
    
    color_count.Resize(num_colors + Avida::Viewer::MAP_RESERVED_COLORS);
    for (int i = 0; i < color_count.GetSize(); i++)
      color_count[i] = (i >= Avida::Viewer::MAP_RESERVED_COLORS && (i - Avida::Viewer::MAP_RESERVED_COLORS) < entries.count) ? 1 : 0;
    
    is_categorical = true;
    scale_label = @"";
  } else {
    num_colors = 0;
    scale_entries.Resize(Avida::Viewer::MAP_RESERVED_COLORS);
    color_count.Resize(Avida::Viewer::MAP_RESERVED_COLORS);
    is_categorical = true;
    scale_label = @"";
  }
  [self setNeedsDisplay:YES];
}

@end
