//
//  DraggableImageView.h
//  viewer-macos
//
//  Created by Bryson David Michael on 1/19/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <AppKit/AppKit.h>

@class DraggableImageView;

@protocol DraggableImageViewDelegate
@required
- (void) draggableImageView:(DraggableImageView*)imageView writeToPasteboard:(NSPasteboard*)pboard;
@end


@interface DraggableImageView : NSImageView
{
  NSEvent* downEvent;
  IBOutlet id<DraggableImageViewDelegate> delegate;
}

- (void) startDrag:(NSEvent*)event;

- (BOOL) shouldDelayWindowOrderingForEvent:(NSEvent*)event;
- (BOOL) acceptsFirstMouse:(NSEvent*)event;
- (void) mouseDown:(NSEvent*)event;
- (void) mouseDragged:(NSEvent*)event;

- (NSEvent*) downEvent;
- (void)setDownEvent:(NSEvent*)event;

@property (readwrite) id<DraggableImageViewDelegate> delegate;

@end
