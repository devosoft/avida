//
//  FlipView.m
//  viewer-macos
//
//  Created by David Michael Bryson on 1/5/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "FlipView.h"

#import "ImageFlipperView.h"

@implementation FlipView

- (void) swapInView:(NSView*)view {
	[view setFrame:[self bounds]];
	[self addSubview:view];
}

- (void)awakeFromNib {
	[self swapInView:view1];
	
	flipperView = [[ImageFlipperView alloc] initWithFrame:[self bounds]];
	[flipperView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
}

- (void) cacheViews:(NSView*)oldView :(NSView*)newView
{
	NSBitmapImageRep* bitmap;
	NSRect rect;
	
	rect = [oldView bounds];
	bitmap = [oldView bitmapImageRepForCachingDisplayInRect:rect];
	[oldView cacheDisplayInRect:rect toBitmapImageRep:bitmap];
	
	[flipperView setImage1:bitmap];
  
	[newView setFrame:[self bounds]];
	
	rect = [newView bounds];
	bitmap = [newView bitmapImageRepForCachingDisplayInRect:rect];
	[newView cacheDisplayInRect:rect toBitmapImageRep:bitmap];
	
	[flipperView setImage2:bitmap];
}

- (IBAction) flip:(id)sender withDuration:(float)duration
{
	NSView* newView;
  NSView* oldView;
	
	if ([[self subviews] containsObject:view2]) {
		newView = view1;
		oldView = view2;
	} else {
		newView = view2;
		oldView = view1;
	}
  
	[self cacheViews:oldView :newView];
	
	[oldView removeFromSuperview];
  
	[self swapInView:flipperView];
	[flipperView flip:(newView == view1) withDuration:duration];
    
	[self swapInView:newView];
	// Forcing display here gets rid of flicker
	[self display];
	
	[flipperView removeFromSuperviewWithoutNeedingDisplay];
}

- (BOOL) isCurrentView:(NSView*)view
{
  return [view isDescendantOf:self];
}

@synthesize view1;
@synthesize view2;

@end
