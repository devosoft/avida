//
//  AvidaEDOrganismView.mm
//  viewer-macos
//
//  Created by David Michael Bryson on 3/5/12.
//  Copyright 2012 Michigan State University. All rights reserved.
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

#import "AvidaEDOrganismView.h"

#import "AvidaEDController.h"
#import "AvidaEDEnvActionsDataSource.h"
#import "AvidaRun.h"
#import "Freezer.h"
#import "NSFileManager+TemporaryDirectory.h"
#import "NSString+Apto.h"
#import "OrganismView.h"

#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Manager.h"
#include "avida/environment/Product.h"
#include "avida/viewer/Freezer.h"



@interface AvidaEDOrganismView (hidden)
- (AvidaRun*) world;
- (void) setGenome:(Avida::GenomePtr)genome withName:(NSString*)name;

- (void) setSnapshot:(int)snapshot;
- (void) setTaskCountsWithSnapshot:(const Avida::Viewer::HardwareSnapshot&)snapshot;
@end

@implementation AvidaEDOrganismView (hidden)

- (AvidaRun*) world {
  if (!testWorld) {
    // Create world to test in
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* runPath = [fileManager createTemporaryDirectory];
    Avida::Viewer::FreezerID default_world(Avida::Viewer::CONFIG, 0);
    
    Avida::Viewer::FreezerPtr freezer = [ctlr freezer];
    freezer->InstantiateWorkingDir(default_world, [runPath cStringUsingEncoding:NSASCIIStringEncoding]);
    testWorld = [[AvidaRun alloc] initWithDirectory:runPath];
    
    
    Avida::Environment::ManagerPtr env = Avida::Environment::Manager::Of([testWorld world]);
    Avida::Environment::ConstActionTriggerIDSetPtr trigger_ids = env->GetActionTriggerIDs();
    for (Avida::Environment::ConstActionTriggerIDSetIterator it = trigger_ids->Begin(); it.Next();) {
      Avida::Environment::ConstActionTriggerPtr action = env->GetActionTrigger(*it.Get());
      NSString* entryName = [NSString stringWithAptoString:action->GetID()];
      NSString* entryDesc = [NSString stringWithAptoString:action->GetDescription()];
      [envActions addNewEntry:entryName withDescription:entryDesc withOrder:action->TempOrdering()];
    }
    [tblTaskCounts reloadData];
  }
  
  return testWorld;
}


- (void) setGenome:(Avida::GenomePtr)genome withName:(NSString*)name {
  // Trace genome
  trace = Avida::Viewer::OrganismTracePtr(new Avida::Viewer::OrganismTrace([[self world] oldworld], genome));

  [txtOrgName setStringValue:name];
  [txtOrgName setEnabled:YES];
  
  if (trace->SnapshotCount() > 0) {
    [sldStatus setMinValue:0];
    [sldStatus setMaxValue:trace->SnapshotCount() - 1];
    [sldStatus setIntValue:0];
    [sldStatus setEnabled:YES];
    
    [btnBegin setEnabled:NO];
    [btnBack setEnabled:NO];
    [btnGo setEnabled:YES];
    [btnGo setTitle:@"Run"];
    [btnForward setEnabled:YES];
    [btnEnd setEnabled:YES];
    
    [self setSnapshot:0];
  } else {
    [sldStatus setIntValue:0];
    [sldStatus setEnabled:NO];
    
    [btnBegin setEnabled:NO];
    [btnBack setEnabled:NO];
    [btnGo setEnabled:NO];
    [btnGo setTitle:@"Run"];
    [btnForward setEnabled:NO];
    [btnEnd setEnabled:NO];
    
    [orgView setSnapshot:NULL];
  }
}



- (void) setSnapshot:(int)snapshot {
  assert(trace);

  if (snapshot >= 0 && snapshot < trace->SnapshotCount()) {
    curSnapshotIndex = snapshot;
    [self setTaskCountsWithSnapshot:trace->Snapshot(snapshot)];
    [orgView setSnapshot:&trace->Snapshot(snapshot)];
  } else {
    [sldStatus setIntValue:curSnapshotIndex];
  }
  
}



- (void) setTaskCountsWithSnapshot:(const Avida::Viewer::HardwareSnapshot&)snapshot {
  
  for (NSUInteger i = 0; i < [envActions entryCount]; i++) {
    NSString* entry_name = [envActions entryAtIndex:i];
    [envActions updateEntry:entry_name withValue:[NSNumber numberWithInt:snapshot.FunctionCount([entry_name UTF8String])]];
  }
  [tblTaskCounts reloadData];
}

@end


@implementation AvidaEDOrganismView


- (id) initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    // initialize
  }
  
  return self;
}

- (void) awakeFromNib {
  
  [btnBegin setEnabled:NO];
  [btnBack setEnabled:NO];
  [btnGo setEnabled:NO];
  [btnGo setTitle:@"Run"];
  [btnForward setEnabled:NO];
  [btnEnd setEnabled:NO];
  
  [sldStatus setEnabled:NO];
  [txtOrgName setStringValue:@"(none)"];
  [txtOrgName setEnabled:NO];
  
  [orgView registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, nil]];
  
  envActions = [[AvidaEDEnvActionsDataSource alloc] init];
  [tblTaskCounts setDataSource:envActions];
  [tblTaskCounts reloadData];
}


- (IBAction) selectSnapshot:(id)sender {
  int snapshot = [sldStatus intValue];
  [self setSnapshot:snapshot];
}



- (NSDragOperation) draggingEnteredDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
    if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
      return NSDragOperationGeneric;
    }
  }
  
  return NSDragOperationNone;
}

- (NSDragOperation) draggingUpdatedForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
    if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
      return NSDragOperationGeneric;
    }
  }
  
  return NSDragOperationNone;  
}

- (BOOL) prepareForDragOperationForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
    if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
      return YES;
    }
  }
  
  return NO;
}

- (BOOL) performDragOperationForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  
  if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
    Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];    
    
    // Get genome from freezer
    Avida::Viewer::FreezerPtr freezer = [ctlr freezer];
    Avida::GenomePtr genome = freezer->InstantiateGenome(fid);
    
    [self setGenome:genome withName:[NSString stringWithAptoString:freezer->NameOf(fid)]];
  }
  
  return YES;
}


@end
