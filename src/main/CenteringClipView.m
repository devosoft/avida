//
//  CenteringClipView.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 4/22/11.
//  Copyright 2011 Michigan State University. All rights reserved.
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

#import "CenteringClipView.h"

#include <stdio.h>

@implementation CenteringClipView

- (id) initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    viewPoint = NSMakePoint(0, 0);
    hasHScroll = NO;
    hasVScroll = NO;
    adjustingScrollers = NO;
    [self setAutoresizesSubviews:NO];
  }
  return self;
}


- (void) centerView {
  NSRect docRect = [[self documentView] frame];
  NSRect clipRect = [self bounds];
  
  // Center the clipping rect origin x
  if (docRect.size.width < clipRect.size.width) {
    clipRect.origin.x = roundf((docRect.size.width - clipRect.size.width) / 2.0);
  } else {
    clipRect.origin.x = roundf(viewPoint.x * docRect.size.width - (clipRect.size.width / 2.0));
  }
  
  // Center the clipping rect origin y
  if (docRect.size.height < clipRect.size.height) {
    clipRect.origin.y = roundf((docRect.size.height - clipRect.size.height) / 2.0);
  } else {
    clipRect.origin.y = roundf(viewPoint.y * docRect.size.width - (clipRect.size.height / 2.0));
  }
  
//  printf("-----centerView-----\n");
//  printf("doc : %f x %f @ %f, %f\n", docRect.size.width, docRect.size.height, docRect.origin.x, docRect.origin.y);
//  printf("clip: %f x %f @ %f, %f\n", clipRect.size.width, clipRect.size.height, clipRect.origin.x, clipRect.origin.y);
  
  // Scroll the document to the selected center point
  NSScrollView* scrollView = (NSScrollView*)[self superview];
  [self scrollToPoint:[self constrainScrollPoint:clipRect.origin]];
  [scrollView reflectScrolledClipView:self];


	if (!adjustingScrollers) {
    BOOL hScroller = NO;
    BOOL vScroller = NO;
    
    // Determine scroll view frame dimensions (without scrollers)
//    const CGFloat scrollerWidth = ([scrollView scrollerStyle] == NSScrollerStyleLegacy) ? [NSScroller scrollerWidth] : 0.0f;
    const CGFloat scrollerWidth = [NSScroller scrollerWidth];
    NSRect frameRect = [super frame];
    CGFloat frameWidth = frameRect.size.width;
    CGFloat frameHeight = frameRect.size.height;
    if (hasVScroll) frameWidth += scrollerWidth;
    if (hasHScroll) frameHeight += scrollerWidth;
    
		// Determine needed scrollers
		if (docRect.size.width <= frameWidth && docRect.size.height <= frameHeight) {
			hScroller = NO;
			vScroller = NO;
		} else if (docRect.size.width > frameWidth && docRect.size.height <= (frameHeight - scrollerWidth)) {
			hScroller = YES;
			vScroller = NO;
		} else if (docRect.size.height > frameHeight && docRect.size.width <= (frameWidth - scrollerWidth)) {
			hScroller = NO;
			vScroller = YES;
		} else {
			hScroller = YES;
			vScroller = YES;
		}
    
    // Adjust horizontal scroller visibility
		if (hScroller != hasHScroll) {
			hasHScroll = !hasHScroll;
			adjustingScrollers = YES;
      NSScroller* horizontalScroller = [scrollView horizontalScroller];
      [horizontalScroller setKnobProportion:1.0f];
      [horizontalScroller setDoubleValue:0.0];
			[scrollView setHasHorizontalScroller:hasHScroll];
			adjustingScrollers = NO;
		}
		
    // Adjust vertical scroller visibility
		if (vScroller != hasVScroll) {
			hasVScroll = !hasVScroll;
			adjustingScrollers = YES;
      NSScroller* verticalScroller = [scrollView verticalScroller];
      [verticalScroller setKnobProportion:1.0f];
      [verticalScroller setDoubleValue:0.0];
			[scrollView setHasVerticalScroller:hasVScroll];
			adjustingScrollers = NO;
		}
    
//    NSLog(@"hscroll %@ %d", [scrollView horizontalScroller], [[scrollView horizontalScroller] isHidden]);
//    NSLog(@"vscroll %@ %d", [scrollView verticalScroller], [[scrollView verticalScroller] isHidden]);
    
//    [scrollView flashScrollers];
  }
}


// NSClipView Method Overrides

- (NSPoint) constrainScrollPoint:(NSPoint)proposedNewOrigin {
  NSRect docRect = [[self documentView] frame];
  NSRect clipRect = [self bounds];
  CGFloat maxX = docRect.size.width - clipRect.size.width;
  CGFloat maxY = docRect.size.height - clipRect.size.height;
  
//  printf("-----constrainScrollPoint-----\n");
//  printf("prop: %f, %f\n", proposedNewOrigin.x, proposedNewOrigin.y);
//  printf("doc : %f x %f @ %f, %f\n", docRect.size.width, docRect.size.height, docRect.origin.x, docRect.origin.y);
//  printf("clip: %f x %f @ %f, %f\n", clipRect.size.width, clipRect.size.height, clipRect.origin.x, clipRect.origin.y);
  
  clipRect.origin = proposedNewOrigin;
  
  if (docRect.size.width < clipRect.size.width) {
    clipRect.origin.x = roundf(maxX / 2.0);
  } else {
    clipRect.origin.x = roundf(MAX(0, MIN(clipRect.origin.x, maxX)));
  }

  if (docRect.size.height < clipRect.size.height) {
    clipRect.origin.y = roundf(maxY / 2.0);
  } else {
    clipRect.origin.y = roundf(MAX(0, MIN(clipRect.origin.y, maxY)));
  }
  
  viewPoint.x = NSMidX(clipRect) / docRect.size.width;
  viewPoint.y = NSMidY(clipRect) / docRect.size.height;
  
//  printf("orig: %f, %f\n", clipRect.origin.x, clipRect.origin.y);
  return clipRect.origin;
}


- (BOOL) copiesOnScroll
{
	NSRect docRect = [[self documentView] frame];
	NSRect clipRect = [self bounds];
  
	return (roundf(docRect.size.width - clipRect.size.width) >= 0) && (roundf(docRect.size.height - clipRect.size.height) >= 0);
}


- (void) viewBoundsChanged:(NSNotification*)notification {
  [super viewBoundsChanged:notification];
  [self centerView];
}

- (void) viewFrameChanged:(NSNotification*)notification {
  [super viewBoundsChanged:notification];
  [self centerView];  
}

- (void) setFrame:(NSRect)frameRect {
  [super setFrame:frameRect];
  [self centerView];
}

- (void) setFrameOrigin:(NSPoint)newOrigin {
  [super setFrameOrigin:newOrigin];
  [self centerView];
}

- (void) setFrameSize:(NSSize)newSize {
  [super setFrameSize:newSize];
  [self centerView];
}

- (void) setFrameRotation:(CGFloat)angle {
  [super setFrameRotation:angle];
  [self centerView];
}

- (void) setDocumentView:(NSView*)docView {
  [super setDocumentView:docView];
  [self centerView];
}

@end
