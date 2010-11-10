//
//  AvidaRun.mm
//  Avida
//
//  Created by David on 10/27/10.
//  Copyright 2010 Michigan State University. All rights reserved.
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
    self->driver = new cCoreViewDriver(world);
  }
  
  return self;
}

- (bool) isPaused {
  return (self->driver->GetPauseState() == DRIVER_PAUSED);
}

- (void) pause {
  self->driver->SetPause();
}

- (void) resume {
  self->driver->Resume();
}
@end
