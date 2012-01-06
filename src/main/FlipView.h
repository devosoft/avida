//
//  FlipView.h
//  viewer-macos
//
//  Created by David Michael Bryson on 1/5/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class ImageFlipperView;

@interface FlipView : NSView {
	IBOutlet NSView* view1;
	IBOutlet NSView* view2;
  
	ImageFlipperView* flipperView;
}

- (IBAction)flip:(id)sender withDuration:(float)duration;
- (BOOL) isCurrentView:(NSView*)view;

@property (readwrite, retain) NSView* view1;
@property (readwrite, retain) NSView* view2;

@end
