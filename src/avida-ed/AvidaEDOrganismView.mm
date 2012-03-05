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
#import "AvidaRun.h"
#import "Freezer.h"
#import "NSFileManager+TemporaryDirectory.h"

#include "avida/viewer/Freezer.h"

@interface AvidaEDOrganismView (Private)
- (AvidaRun*) world;
@end

@implementation AvidaEDOrganismView (Private)

- (AvidaRun*) world {
  if (!testWorld) {
    // Create world to test in
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* runPath = [fileManager createTemporaryDirectory];
    Avida::Viewer::FreezerID default_world(Avida::Viewer::CONFIG, 0);
    
    Avida::Viewer::FreezerPtr freezer = [ctlr freezer];
    freezer->InstantiateWorkingDir(default_world, [runPath cStringUsingEncoding:NSASCIIStringEncoding]);
    testWorld = [[AvidaRun alloc] initWithDirectory:runPath];
  }
  
  return testWorld;
}

@end


@implementation AvidaEDOrganismView


- (id) initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
  }
  
  return self;
}

- (void) awakeFromNib {
  [orgView registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, nil]];
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
    
    
    // Trace genome
    trace = Avida::Viewer::OrganismTracePtr(new Avida::Viewer::OrganismTrace([[self world] oldworld], genome));
  }
  
  return YES;
}


@end
