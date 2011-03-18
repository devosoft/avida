//
//  AvidaRun.mm
//  Avida
//
//  Created by David on 10/27/10.
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

#import "AvidaRun.h"

#import "CoreViewListener.h"

#include "AvidaTools.h"

#include "cDriver.h"

#include "cAvidaConfig.h"
#include "cUserFeedback.h"
#include "cWorld.h"
#include "tDictionary.h"

using namespace AvidaTools;


@implementation AvidaRun

- (id) init {
  return nil;
}


- (AvidaRun*) initWithDirectory: (NSURL*) dir {
  self = [super init];
  
  if (self) { 
    cAvidaConfig* cfg = new cAvidaConfig;
    cString config_path([[dir path] cStringUsingEncoding:NSASCIIStringEncoding]);
    
    cUserFeedback feedback;
    if (!cfg->Load("avida.cfg", config_path, &feedback, NULL, false)) {
      for (int i = 0; i < feedback.GetNumMessages(); i++) {
        switch (feedback.GetMessageType(i)) {
          case cUserFeedback::ERROR:    cerr << "error: "; break;
          case cUserFeedback::WARNING:  cerr << "warning: "; break;
          default: break;
        };
        cerr << feedback.GetMessage(i) << endl;
      }
      
      return nil;
    }
    
    cWorld* world = cWorld::Initialize(cfg, config_path, &feedback);
    
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::ERROR:    cerr << "error: "; break;
        case cUserFeedback::WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }
    
    
    if (!world) return nil;
    driver = new Avida::CoreView::cDriver(world);
    driver->Start();
  }
  
  return self;
}


- (void) dealloc {
  delete driver;
  driver = NULL;
  [super dealloc];
}


- (void) finalize { 
  delete driver;
  driver = NULL;
  [super finalize];
}


- (bool) isPaused {
  return (self->driver->GetPauseState() == DRIVER_PAUSED);
}


- (void) pause {
  driver->SetPause();
}


- (void) resume {
  driver->Resume();
}


- (void) end {
  driver->SetDone();
}


- (void) attachListener: (id<CoreViewListener>)listener {
  if (driver) driver->AttachListener([listener listener]);
}


- (void) detachListener: (id<CoreViewListener>)listener {
  if (driver) driver->DetachListener([listener listener]);
}


@end
