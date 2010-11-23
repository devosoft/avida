//
//  MapGridView.m
//  Avida
//
//  Created by David on 11/23/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2
//  of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#import "MapGridView.h"


@implementation MapGridView

- (id) initWithFrame: (NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    return self;
}

- (void) drawRect: (NSRect)dirtyRect {
  [[NSColor blackColor] set];
  [NSBezierPath fillRect:dirtyRect];
  
  NSRect gridCellRect;
  gridCellRect.size.width = 9.0;
  gridCellRect.size.height = 9.0;

  for (int i = 0; i < 60; i++) {
    for (int j = 0; j < 60; j++) {
      gridCellRect.origin = NSMakePoint(10.0 * i, 10.0 * j);
      [[NSColor blueColor] set];
      [NSBezierPath fillRect:gridCellRect];
    }
  }
}


- (BOOL) isOpaque {
  return YES;
}

@end
