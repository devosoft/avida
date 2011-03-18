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

#include "cMap.h"


@implementation MapGridView

- (id) initWithFrame: (NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
      map_width = 0;
      map_height = 0;
    }
    return self;
}

- (void) drawRect: (NSRect)dirtyRect {
  [[NSColor blackColor] set];
  [NSBezierPath fillRect:dirtyRect];
  
  NSRect gridCellRect;
  gridCellRect.size.width = 9.0;
  gridCellRect.size.height = 9.0;

  for (int i = 0; i < map_width; i++) {
    for (int j = 0; j < map_height; j++) {
      gridCellRect.origin = NSMakePoint(10.0 * i, 10.0 * j);
      switch (map_colors[i * map_width + j]) {
        case -4:  continue;
        case -3:  [[NSColor darkGrayColor] set]; break;
        case -2:  [[NSColor grayColor] set]; break;
        case -1:  [[NSColor whiteColor] set]; break;
        case 0:   [[NSColor greenColor] set]; break;
        case 1:   [[NSColor redColor] set]; break;
        case 2:   [[NSColor blueColor] set]; break;
        case 3:   [[NSColor cyanColor] set]; break;
        case 4:   [[NSColor yellowColor] set]; break;
        case 5:   [[NSColor magentaColor] set]; break;
        case 6:   [[NSColor orangeColor] set]; break;
        case 7:   [[NSColor purpleColor] set]; break;
        case 8:   [[NSColor brownColor] set]; break;
        case 9:   [[NSColor lightGrayColor] set]; break;
        default:  [[NSColor darkGrayColor] set]; break;
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
  
  state->Release();
  
  [self setNeedsDisplay:YES];
}

@end
