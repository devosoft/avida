//
//  ImageFlipperView.h
//  viewer-macos
//
//  Created by David Michael Bryson on 1/5/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface ImageFlipperView : NSOpenGLView {
	float		yAngle;
	GLuint		texture1;
	GLuint		texture2;
  
	float		slabWidth;
}

- (void) flip:(BOOL)forward withDuration:(float)duration;
- (void) setSlabWidth:(float)width;

- (void) rotate:(float)angle;

- (void) setImage1:(NSBitmapImageRep*)bitmap;
- (void) setImage2:(NSBitmapImageRep*)bitmap;

@end
