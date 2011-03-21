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

#include "avida/viewer-core/cDriver.h"

#import "CoreViewListener.h"


@implementation AvidaRun

- (id) init {
  return nil;
}


- (AvidaRun*) initWithDirectory: (NSURL*) dir {
  self = [super init];
  
  if (self) { 
    Apto::String config_path([[dir path] cStringUsingEncoding:NSASCIIStringEncoding]);
    driver = Avida::CoreView::cDriver::InitWithDirectory(config_path);
    if (!driver) return nil;
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
