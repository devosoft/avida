//
//  NSBitmapImageRep+GLExtensions.m
//  viewer-macos
//
//  Created by David Michael Bryson on 1/5/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "NSBitmapImageRep+GLExtensions.h"

@implementation NSBitmapImageRep (GLExtensions)

//
// Based on code from http://developer.apple.com/qa/qa2001/qa1325.html 
// and http://www.cocoadev.com/index.pl?OpenGLTextureFromNSImage
//

- (void) createGLTextureWithName:(GLuint*)texName {	
  // Set proper unpacking row length for bitmap.
	// NOTE: the Apple code just uses pixelsWide which was incorrect for certain bitmaps. Calculating it 
	// as shown here is more accurate as it takes padding into account.
	// From: http://www.cocoadev.com/index.pl?OpenGLTextureFromNSImage

  int bytesPerRow = (int)[self bytesPerRow];
  glPixelStorei(GL_UNPACK_ROW_LENGTH, bytesPerRow / ([self bitsPerPixel] >> 3));
	
  // Set byte aligned unpacking (needed for 3 byte per pixel bitmaps).
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	
  // Generate a new texture name if one was not provided.
  if (*texName == 0) {
		glGenTextures(1, texName);
	}
  glBindTexture(GL_TEXTURE_RECTANGLE_EXT, *texName);
	
  // Non-mipmap filtering (redundant for texture_rectangle).
  glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER,  GL_LINEAR);
  int samplesPerPixel = (int)[self samplesPerPixel];
	
  // Nonplanar, RGB 24 bit bitmap, or RGBA 32 bit bitmap.
  if(![self isPlanar] && (samplesPerPixel == 3 || samplesPerPixel == 4)) {
		int				pixelsHigh, i;
		unsigned char	*originalData, *flippedData;
    
		pixelsHigh = (int)[self pixelsHigh];
		
		// OpenGL uses a flipped coordinate system so reverse it here
		originalData = [self bitmapData];
		flippedData = (unsigned char *)malloc(bytesPerRow * pixelsHigh);
		
		for (i = 0; i < pixelsHigh; i++) {
			bcopy((originalData + bytesPerRow * i), (flippedData + bytesPerRow * (pixelsHigh - i - 1)), bytesPerRow);
		}
		
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0,
                 samplesPerPixel == 4 ? GL_RGBA8 : GL_RGB8,
                 (int)[self pixelsWide], pixelsHigh, 0,
                 samplesPerPixel == 4 ? GL_RGBA : GL_RGB,
                 GL_UNSIGNED_BYTE, flippedData);
		
		free(flippedData);
  } else {
    // Handle other bitmap formats.
		NSLog(@"Cannot convert image: %@", self);
  }
}

@end
