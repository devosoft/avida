//
//  MapGridView.m
//  Avida
//
//  Created by David on 11/23/10.
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

#import "MapGridView.h"

#include "avida/viewer-core/cMap.h"

#include <iostream>


static inline CGFloat sigmoid(CGFloat x, CGFloat midpoint, CGFloat steepness)
{
  CGFloat val = steepness * (x - midpoint);
  return exp(val) / (1.0 + exp(val));
}



@implementation MapGridView

- (id) initWithFrame: (NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
      map_width = 0;
      map_height = 0;
      num_colors = 0;
      color_cache = [NSMutableArray arrayWithCapacity:255];
    }
    return self;
}

- (void) drawRect: (NSRect)dirtyRect {
  [[NSColor blackColor] set];
  [NSBezierPath fillRect:dirtyRect];
  
  if (num_colors != [color_cache count]) {
    [color_cache removeAllObjects];
    if (num_colors == 10) {
      [color_cache insertObject:[NSColor greenColor] atIndex:0];
      [color_cache insertObject:[NSColor redColor] atIndex:1];
      [color_cache insertObject:[NSColor blueColor] atIndex:2];
      [color_cache insertObject:[NSColor cyanColor] atIndex:3];
      [color_cache insertObject:[NSColor yellowColor] atIndex:4];
      [color_cache insertObject:[NSColor magentaColor] atIndex:5];
      [color_cache insertObject:[NSColor orangeColor] atIndex:6];
      [color_cache insertObject:[NSColor purpleColor] atIndex:7];
      [color_cache insertObject:[NSColor brownColor] atIndex:8];
      [color_cache insertObject:[NSColor lightGrayColor] atIndex:9];
    } else {
      for (int i = 0; i < num_colors; i++) {
        CGFloat x = 0.1 + 0.8 * (static_cast<CGFloat>(i) / (num_colors - 1));
        CGFloat h = fmod((x + .27), 1.0);
        CGFloat s = sigmoid(1.0 - x, 0.1, 30);
        CGFloat b = sigmoid(x, 0.3, 10);
        [color_cache insertObject:[NSColor colorWithCalibratedHue:h saturation:s brightness:b alpha:1.0] atIndex:i];
        std::cout << "c: " << i << "  h: " << h << "  s: " << s << "  b: " << b << std::endl;
      }
    }
  }
  
  NSRect gridCellRect;
  gridCellRect.size.width = 9.0;
  gridCellRect.size.height = 9.0;

  for (int i = 0; i < map_width; i++) {
    for (int j = 0; j < map_height; j++) {
      gridCellRect.origin = NSMakePoint(10.0 * i, 10.0 * j);
      int color = map_colors[i * map_width + j];
      switch (color) {
        case -4:  continue;
        case -3:  [[NSColor darkGrayColor] set]; break;
        case -2:  [[NSColor grayColor] set]; break;
        case -1:  [[NSColor whiteColor] set]; break;
        default:  [[color_cache objectAtIndex:color] set]; break;
      }
      [NSBezierPath fillRect:gridCellRect];
    }
  }
}


- (BOOL) isOpaque {
  return YES;
}

- (void) updateState: (Avida::CoreView::cMap*)state {
  map_width = state->GetWidth();
  map_height = state->GetHeight();
  
  map_colors = state->GetColors();
  num_colors = state->GetColorScale().GetScaleRange();
  
  state->Release();
  
  [self setNeedsDisplay:YES];
}

@end
