//
//  FlipView.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 1/5/12.
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
