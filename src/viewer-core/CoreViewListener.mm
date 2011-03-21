//
//  CoreViewListener.mm
//  Avida
//
//  Created by David Bryson on 11/11/10.
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

#include "CoreViewListener.h"

#include <cassert>


@implementation CoreViewMap
- (id) initWithMap: (Avida::CoreView::cMap*)map {
  m_map = map;
  return self;
}
@synthesize map = m_map;
@end


@implementation CoreViewUpdate
- (id) initWithUpdate: (int)update {
  m_update = update;
  return self;
}
@synthesize update = m_update;
@end


void cMainThreadListener::NotifyMap(Avida::CoreView::cMap* map)
{
  CoreViewMap* cvm = [[CoreViewMap alloc] initWithMap:map];
  [m_target performSelectorOnMainThread:@selector(handleMap:) withObject:cvm waitUntilDone:NO];
}


void cMainThreadListener::NotifyUpdate(int update)
{
  CoreViewUpdate* cvu = [[CoreViewUpdate alloc] initWithUpdate:update];
  [m_target performSelectorOnMainThread:@selector(handleUpdate:) withObject:cvu waitUntilDone:NO];
}
