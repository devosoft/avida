//
//  AvidaEDGraphicalValueDisplay.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 3/22/12.
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

#import "AvidaEDGraphicalValueDisplay.h"

@implementation AvidaEDGraphicalValueDisplay

- (id)initWithFrame:(NSRect)frame
{
  self = [super initWithFrame:frame];
  if (self) {
    displayValue = 0;
  }
  
  return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
  unsigned int display_bits = (unsigned int)displayValue;
  
  [[NSColor darkGrayColor] set];
  [NSBezierPath fillRect:dirtyRect];
  
  NSRect bitRect;
  bitRect.size.width = 3;
  bitRect.size.height = 3;
  bitRect.origin = NSMakePoint(1, 1);
  
  for (int i = 0; i < 32; i++) {
    
    if (display_bits & 0x80000000) {
      [[NSColor yellowColor] set];
    } else {
      [[NSColor blueColor] set];
    }
    [NSBezierPath fillRect:bitRect];
    
    bitRect.origin.x += 4;
    display_bits <<= 1;
  }
}


- (int) displayValue {
  return displayValue;
}

- (void) setDisplayValue:(int)_value {
  displayValue = _value;
  [self setNeedsDisplay:YES];
}

@end
