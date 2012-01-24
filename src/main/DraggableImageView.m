//
//  DraggableImageView.m
//  viewer-macos
//
//  Created by Bryson David Michael on 1/19/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "DraggableImageView.h"

@implementation DraggableImageView

- (void) startDrag:(NSEvent*)event {
  NSPoint location;
  NSSize size = [[self image] size];
  NSPasteboard* pboard = [NSPasteboard pasteboardWithName:(NSString*)NSDragPboard];
  [pboard clearContents];
  
  if (delegate != nil) {
    [delegate draggableImageView:self writeToPasteboard:pboard];
  } else {
    [pboard writeObjects:[[NSArray alloc] initWithObjects:[self image], nil]];
  }
  
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
  [downEvent autorelease];
  downEvent = [event retain];
}

@synthesize delegate;

@end
