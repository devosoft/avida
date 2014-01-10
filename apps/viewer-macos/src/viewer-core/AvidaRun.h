//
//  AvidaRun.h
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 10/27/10.
//  Copyright 2010-2012 Michigan State University. All rights reserved.
//  http://avida.devosoft.org/viewer-macos
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
//  following conditions are met:
//  
//  1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the
//      following disclaimer.
//  2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//      following disclaimer in the documentation and/or other materials provided with the distribution.
//  3.  Neither the name of Michigan State University, nor the names of contributors may be used to endorse or promote
//      products derived from this software without specific prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY MICHIGAN STATE UNIVERSITY AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL MICHIGAN STATE UNIVERSITY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Authors: David M. Bryson <david@programerror.com>
//

#import <Cocoa/Cocoa.h>

#include "avida/core/Types.h"
#include "avida/data/Types.h"
#include "avida/systematics/Types.h"
#include "avida/viewer.h"

@protocol ViewerListener;


@interface AvidaRun : NSObject {
@protected
  Avida::Viewer::Driver* driver;
  int initialUpdate;
}

- (id) init;
- (AvidaRun*) initWithDirectory:(NSString*)dir;
- (AvidaRun*) initWithDirectory:(NSString*)dir shouldPauseAt:(Avida::Update)update;

- (Avida::World*) world;
- (cWorld*) oldworld;

- (void) dealloc;
- (void) finalize;

- (int) numOrganisms;
- (int) currentUpdate;

@property (nonatomic, readwrite) NSSize worldSize;
@property (nonatomic, readwrite) double mutationRate;
@property (nonatomic, readwrite) int placementMode;
@property (nonatomic, readwrite) int randomSeed;
@property (nonatomic, readonly) int initialUpdate;

- (double) reactionValueOf:(const Apto::String&)reaction_name;
- (void) setReactionValueOf:(const Apto::String&)reaction_name to:(double)value;

- (bool) hasStarted;
- (bool) willPauseNow;
- (bool) willPause;
- (bool) isPaused;
- (bool) hasFinished;

- (void) pause;
- (void) pauseAt:(Avida::Update)update;
- (void) resume;
- (void) end;
- (void) sync;

- (void) injectGenome:(Avida::GenomePtr)genome atX:(int)x Y:(int)y withName:(Apto::String)name;
- (bool) hasPendingInjects;
- (int) pendingInjectCount;
- (NSPoint) locationOfPendingInjectAtIndex:(int)index;


- (void) attachListener:(id<ViewerListener>)listener;
- (void) detachListener:(id<ViewerListener>)listener;

- (void) attachRecorder:(Avida::Data::RecorderPtr)recorder;
- (void) attachRecorder:(Avida::Data::RecorderPtr)recorder concurrentUpdate:(BOOL)concurrentUpdate;
- (void) detachRecorder:(Avida::Data::RecorderPtr)recorder;

- (double) testFitnessOfGroup:(Avida::Systematics::GroupPtr)group;
- (double) testGestationTimeOfGroup:(Avida::Systematics::GroupPtr)group;
- (double) testMetabolicRateOfGroup:(Avida::Systematics::GroupPtr)group;
- (int) testEnvironmentTriggerCountFor:(NSString*)trigger ofGroup:(Avida::Systematics::GroupPtr)group;

@end
