//
//  NSBitmapImageRep+GLExtensions.m
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
