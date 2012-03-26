//
//  AvidaEDGraphicalValueDisplay.m
//  viewer-macos
//
//  Created by David Michael Bryson on 3/22/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
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
