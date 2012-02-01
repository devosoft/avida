//
//  DroppableGraphHostingView.m
//  viewer-macos
//
//  Created by David Michael Bryson on 2/1/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "DroppableGraphHostingView.h"

@implementation DroppableGraphHostingView

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

@end
