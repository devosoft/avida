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
    tDictionary<cString> defs;
    
    cfg->Load("avida.cfg", defs, config_path, false, false);
    cWorld* world = cWorld::Initialize(cfg, config_path);
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
