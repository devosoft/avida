//
//  NSBitmapImageRep+GLExtensions.h
//  viewer-macos
//
//  Created by David Michael Bryson on 1/5/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>

@interface NSBitmapImageRep (GLExtensions)

- (void)createGLTextureWithName:(GLuint *)texName;

@end
