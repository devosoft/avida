//
//  MapGridView.h
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

#import <Cocoa/Cocoa.h>

#include "apto/core.h"

namespace Avida {
  namespace CoreView {
    class cMap;
  };
};


@interface MapGridView : NSView {
  int map_width;
  int map_height;
  int num_colors;
  double zoom;
  
  Apto::Array<int> map_colors;
  NSMutableArray* color_cache;
  
}

- (id)initWithFrame:(NSRect)frame;

- (void)drawRect:(NSRect)rect;
- (BOOL)isOpaque;

- (void)updateState:(Avida::CoreView::cMap*)state;

@property (readwrite, nonatomic) double zoom;

@end
