//
//  AvidaRun.mm
//  Avida
//
//  Created by David on 10/27/10.
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

#import "AvidaRun.h"

#include "AvidaTools.h"

#include "cAvidaConfig.h"
#include "cCoreViewDriver.h"
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
    driver = new cCoreViewDriver(world);
    driver->Start();
  }
  
  return self;
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


- (void) attachListener: (cCoreViewListener*)listener {
  if (driver) driver->AttachListener(listener);
}

- (void) detachListener: (cCoreViewListener*)listener {
  if (driver) driver->DetachListener(listener);
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

@end
