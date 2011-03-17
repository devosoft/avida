//
//  CoreViewListener.h
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

#include "Foundation/Foundation.h"

#include "cCoreViewListener.h"

@class CoreViewMap;
@class CoreViewUpdate;

class cCoreView_Map;


@protocol CoreViewListener
@property (readonly) cCoreViewListener* listener;
@optional
-(void)handleMap:(CoreViewMap*)pkg;
-(void)handleUpdate:(CoreViewUpdate*)pkg;
@end


@interface CoreViewMap : NSObject {
  cCoreView_Map* m_map;
}
-(id)initWithMap:(cCoreView_Map*)map;
@property (readonly) cCoreView_Map* map;
@end;


@interface CoreViewUpdate : NSObject {
  int m_update;
}
-(id)initWithUpdate:(int)update;
@property (readonly) int update;
@end


class cMainThreadListener : public cCoreViewListener
{
private:
  id m_target;
  
public:
  cMainThreadListener(id <CoreViewListener> target) : m_target(target) { ; }
  
  bool WantsMap() { return true; }
  bool WantsUpdate() { return true; }
  
  void NotifyMap(cCoreView_Map* map);
  void NotifyUpdate(int update);
};
