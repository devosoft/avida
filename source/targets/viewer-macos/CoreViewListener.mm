//
//  CoreViewListener.mm
//  Avida
//
//  Created by David Bryson on 11/11/10.
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

#include "CoreViewListener.h"

#include <cassert>


@implementation CoreViewMap
- (id) initWithMap: (cCoreView_Map*)map {
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


void cMainThreadListener::NotifyMap(cCoreView_Map* map)
{
  CoreViewMap* cvm = [[CoreViewMap alloc] initWithMap:map];
  [m_target performSelectorOnMainThread:@selector(handleMap:) withObject:cvm waitUntilDone:NO];
}


void cMainThreadListener::NotifyUpdate(int update)
{
  CoreViewUpdate* cvu = [[CoreViewUpdate alloc] initWithUpdate:update];
  [m_target performSelectorOnMainThread:@selector(handleUpdate:) withObject:cvu waitUntilDone:NO];
}
