//
//  ImageFlipperView.m
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

#import "ImageFlipperView.h"
#import <OpenGL/OpenGL.h>
#import <OpenGL/glu.h>
#import <OpenGL/glext.h>
#import "NSBitmapImageRep+GLExtensions.h"

#define CAMERA_DISTANCE			4.0
#define DEFAULT_SLAB_WIDTH		0.02

@interface RotateAnimation : NSAnimation {
	ImageFlipperView* view;
	BOOL forward;
}

@end

@implementation RotateAnimation

- (id) initWithDuration:(NSTimeInterval)duration animationCurve:(NSAnimationCurve)animationCurve view:(ImageFlipperView*)flipView forward:(BOOL)flag {
	self = [super initWithDuration:duration animationCurve:animationCurve];
	if (self != nil) {
		view = flipView;
		forward = flag;
	}
	return self;
}

- (void) setCurrentProgress:(NSAnimationProgress)progress {
	if (forward) {
		[view rotate:progress * 180];
	} else {
		[view rotate:-progress * 180];
	}
}

@end


@implementation ImageFlipperView

+ (NSOpenGLPixelFormat*) defaultPixelFormat {
	NSOpenGLPixelFormatAttribute attr[] = {
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFASamples, 2,
    NSOpenGLPFASampleBuffers, 1,
    0
  };
	NSOpenGLPixelFormat			*pixelFormat;
	
	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
	
	if (pixelFormat == nil) NSLog(@"Could not create pixel format.");

	return pixelFormat;
}


- (id) initWithFrame:(NSRect)frameRect {
	self = [super initWithFrame:frameRect pixelFormat:[[self class] defaultPixelFormat]];
  
	if (self != nil) {
		slabWidth = DEFAULT_SLAB_WIDTH;
		[self setPostsFrameChangedNotifications: YES];
		[[self openGLContext] makeCurrentContext];
	}
	return self;
}

- (id)initWithCoder:(NSCoder*)coder {
	self = [super initWithCoder:coder];
	
	if (self != nil) {
		slabWidth = DEFAULT_SLAB_WIDTH;		
		[self setPixelFormat:[[self class] defaultPixelFormat]];
		[self setPostsFrameChangedNotifications: YES];
		[[self openGLContext] makeCurrentContext];
	}
	return self;
}

- (void) dealloc {
	glDeleteTextures(1, &texture1);
	glDeleteTextures(1, &texture2);
}

- (void) finalize {
	glDeleteTextures(1, &texture1);
	glDeleteTextures(1, &texture2);	
  
  [super finalize];
}


- (BOOL) isOpaque {
	return YES;
}


- (void) prepareOpenGL {
	int swapValue = 1;
  
	// Note: uncomment the next two lines and change -isOpaque above to return NO for a transparent view.
	//long			value = 0;
	
	//	[[self openGLContext] setValues:&value forParameter:NSOpenGLCPSurfaceOpacity];
	
	
	// Sync to prevent tearing
	[[self openGLContext] setValues:&swapValue forParameter:NSOpenGLCPSwapInterval];
  
	glClearColor(0.0, 0.0, 0.0, 0.0);
  
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_POLYGON_SMOOTH);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
}

- (void) reshape {
  float	aspect;
  NSSize	size = [self frame].size;
	float	x, z;
	
	[[self openGLContext] makeCurrentContext];
  
  aspect = size.width / size.height;
  glViewport(0, 0, size.width, size.height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
	// Calculate  perspective such that:
	// - z-near is the z-location of the object - 2.0 (since the object can rotate towards you plus a little extra buffer)
	// - At the z-location, -1.0 and 1.0 line up with the edges of the viewport (i.e. a rect going from -1 to 1 will 
	// fill/line up with the viewport)
	z = CAMERA_DISTANCE - 2.0;
	x = z / (CAMERA_DISTANCE - slabWidth);
  
	glFrustum(-x, x, -x, x, CAMERA_DISTANCE - 2.0, CAMERA_DISTANCE + 2.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

- (void) setImage1:(NSBitmapImageRep*)bitmap {
	[[self openGLContext] makeCurrentContext];
	glDeleteTextures(1, &texture1);
	[bitmap createGLTextureWithName:&texture1];
}

- (void) setImage2:(NSBitmapImageRep*)bitmap {
	[[self openGLContext] makeCurrentContext];
	glDeleteTextures(1, &texture2);
	[bitmap createGLTextureWithName:&texture2];
}

- (void) setupLighting {
	GLfloat lightPosition[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
  
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	// Full light up to 0.0, then drop off
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0f / (CAMERA_DISTANCE * CAMERA_DISTANCE));
}

- (void) drawRect:(NSRect)rect {
	NSRect bounds = [self bounds];
	
	[[self openGLContext] makeCurrentContext];
  
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	[self setupLighting];	
	
	gluLookAt(0.0f, 0.0f, CAMERA_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	
	glRotatef(yAngle, 0.0f, 1.0f, 0.0f);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	// Draw the back face
	glNormal3f(0.0, 0.0, -1.0);
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture2);
	
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -slabWidth);
	
	glTexCoord2f(NSWidth(bounds), 0.0f);
	glVertex3f(-1.0f, -1.0f, -slabWidth);
  
	glTexCoord2f(NSWidth(bounds), NSHeight(bounds));
	glVertex3f(-1.0f, 1.0f, -slabWidth);
	
	glTexCoord2f(0.0f, NSHeight(bounds));
	glVertex3f(1.0f, 1.0f, -slabWidth);
	
	glEnd();
  
	// Draw the front face
	glNormal3f(0.0, 0.0, 1.0);	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture1);
  
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f( -1.0f, -1.0f, slabWidth);
	
	glTexCoord2f(NSWidth(bounds), 0.0f);
	glVertex3f( 1.0f, -1.0f, slabWidth);
	
	glTexCoord2f(NSWidth(bounds), NSHeight(bounds));
	glVertex3f( 1.0f, 1.0f, slabWidth);
	
	glTexCoord2f(0.0f, NSHeight(bounds));
	glVertex3f( -1.0f, 1.0f, slabWidth);
	
  glEnd();
  
	glDisable(GL_TEXTURE_RECTANGLE_EXT);
	
	// Draw edges if necessary. Note that only one of the edges is seen but
	// drawing them all just for completeness in case you want to modify this
	// to flip in some other direction.
	if (slabWidth > 0.0) {
		glColor3f(0.25, 0.25, 0.25);
		glBegin(GL_QUADS);
		
		// Draw left edge		
		glNormal3f(-1.0, 0.0, 0.0);
    
		glVertex3f(-1.0, -1.0, -slabWidth);
		glVertex3f(-1.0, -1.0, slabWidth);
		glVertex3f(-1.0, 1.0, slabWidth);
		glVertex3f(-1.0, 1.0, -slabWidth);
    
		// Draw right edge
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(1.0, -1.0, slabWidth);
		glVertex3f(1.0, -1.0, -slabWidth);
		glVertex3f(1.0, 1.0, -slabWidth);
		glVertex3f(1.0, 1.0, slabWidth);
    
		// Draw top edge
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(1.0, 1.0, slabWidth);
		glVertex3f(1.0, 1.0, -slabWidth);
		glVertex3f(-1.0, 1.0, -slabWidth);
		glVertex3f(-1.0, 1.0, slabWidth);
		
		// Draw bottom edge
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(1.0, -1.0, -slabWidth);
		glVertex3f(1.0, -1.0, slabWidth);
		glVertex3f(-1.0, -1.0, slabWidth);
		glVertex3f(-1.0, -1.0, -slabWidth);
		glEnd();
	}
	
 	if ([self inLiveResize]) {
		glFlush();
	} else {
		[[self openGLContext] flushBuffer];
	}
	
	[NSOpenGLContext clearCurrentContext];
}

- (void) rotate:(float)angle {
	yAngle = angle;
	[self display];
}


- (void) flip:(BOOL)forward withDuration:(float)duration {
	RotateAnimation* animation = [[RotateAnimation alloc] initWithDuration:duration animationCurve:NSAnimationEaseInOut view:self forward:forward];
	[animation setAnimationBlockingMode:NSAnimationBlocking];
	[animation startAnimation];
}

- (void) setSlabWidth:(float)width {
	slabWidth = width;
	// Force reshape to reset perspective
	[self reshape];
	[self setNeedsDisplay:YES];
}

@end
