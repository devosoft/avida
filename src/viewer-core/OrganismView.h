//
//  OrganismView.h
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 3/5/12.
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

#import <Cocoa/Cocoa.h>

#import "DropDelegate.h"

#include "avida/viewer/GraphicsContext.h"
#include "avida/viewer/Types.h"


@interface OrganismView : NSView <NSDraggingDestination> {
  IBOutlet id<DropDelegate> dropDelegate;
  
  const Avida::Viewer::HardwareSnapshot* snapshot;
  Avida::Viewer::GraphicsContext graphics_context;
}

@property (readwrite) id<DropDelegate> dropDelegate;
@property (readwrite, nonatomic) const Avida::Viewer::HardwareSnapshot* snapshot;

- (void) resizeSubviewsWithOldSize:(NSSize)oldBoundsSize;

// NSDraggingDestination
- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender;
- (NSDragOperation) draggingUpdated:(id<NSDraggingInfo>)sender;
- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender;
- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender;
- (BOOL) wantsPeriodicDraggingUpdates;   

@end
