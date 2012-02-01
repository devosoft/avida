//
//  DroppableGraphHostingView.h
//  viewer-macos
//
//  Created by David Michael Bryson on 2/1/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CorePlot/CorePlot.h>

@protocol DropDelegate <NSObject>
@optional
- (NSDragOperation) draggingEnteredDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender;
- (NSDragOperation) draggingUpdatedForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender;
- (BOOL) prepareForDragOperationForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender;
- (BOOL) performDragOperationForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender;
- (BOOL) wantsPeriodicDraggingUpdatesForDestination:(id<NSDraggingDestination>)destination;
@end


@interface DroppableGraphHostingView : CPTGraphHostingView <NSDraggingDestination> {
  IBOutlet id<DropDelegate> dropDelegate;
}

// NSDraggingDestination
- (NSDragOperation) draggingEntered:(id<NSDraggingInfo>)sender;
- (NSDragOperation) draggingUpdated:(id<NSDraggingInfo>)sender;
- (BOOL) prepareForDragOperation:(id<NSDraggingInfo>)sender;
- (BOOL) performDragOperation:(id<NSDraggingInfo>)sender;
- (BOOL) wantsPeriodicDraggingUpdates;   

@end
