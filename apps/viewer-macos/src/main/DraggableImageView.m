//
//  DraggableImageView.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 1/19/12.
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

#import "DraggableImageView.h"

@implementation DraggableImageView

- (void) startDrag:(NSEvent*)event {
  NSPoint location;
  NSSize size = [[self image] size];
  NSPasteboard* pboard = [NSPasteboard pasteboardWithName:(NSString*)NSDragPboard];
  [pboard clearContents];
  
  // if no delegate, then no drag can begin
  if (delegate == nil) return;
  
  [delegate draggableImageView:self writeToPasteboard:pboard];

  // Don't drag empty pasteboards
  if (pboard.pasteboardItems.count == 0) return;
  
  location.x = ([self bounds].size.width - size.width) / 2;
  location.y = ([self bounds].size.height - size.height) / 2;
  
  id<NSDraggingSource> src = (delegate != nil) ? (id<NSDraggingSource>)delegate : (id<NSDraggingSource>)self;
  [self dragImage:[self image] at:location offset:NSMakeSize(0,0) event:event pasteboard:pboard source:src slideBack:YES];
}

- (BOOL) shouldDelayWindowOrderingForEvent:(NSEvent*)event {
  return YES;
}

- (BOOL) acceptsFirstMouse:(NSEvent*)event {
  return YES;
}

- (void) mouseDown:(NSEvent*)event {
  [self setDownEvent:event];
}

- (void) mouseDragged:(NSEvent*)event {
  if ([self image]) {
    [self startDrag:downEvent];
  }
  [self setDownEvent:nil];
}

- (NSEvent*) downEvent {
  return downEvent;
}

- (void) setDownEvent:(NSEvent*)event {
  downEvent = event;
}

// NSDraggingDestination
- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(draggingEnteredDestination:sender:)]) {
    return [dropDelegate draggingEnteredDestination:self sender:sender];
  }
  return NSDragOperationNone;
}

- (NSDragOperation) draggingUpdated:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(draggingUpdatedForDestination:sender:)]) {
    return [dropDelegate draggingUpdatedForDestination:self sender:sender];
  }
  return NSDragOperationNone;
}

- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(prepareForDragOperationForDestination:sender:)]) {
    return [dropDelegate prepareForDragOperationForDestination:self sender:sender];
  }
  return NO;
}

- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(performDragOperationForDestination:sender:)]) {
    return [dropDelegate performDragOperationForDestination:self sender:sender];
  }
  return NO;
}

- (BOOL) wantsPeriodicDraggingUpdates {
  if (dropDelegate && [dropDelegate respondsToSelector:@selector(wantsPeriodicDraggingUpdatesForDestination:)]) {
    return [dropDelegate wantsPeriodicDraggingUpdatesForDestination:self];
  }
  return NO;
}

@synthesize delegate;
@synthesize dropDelegate;


@end
