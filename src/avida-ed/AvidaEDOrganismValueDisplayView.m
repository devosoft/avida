//
//  AvidaEDOrganismValueDisplayView.m
//  viewer-macos
//
//  Created by David Michael Bryson on 3/22/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "AvidaEDOrganismValueDisplayView.h"

#import "AvidaEDGraphicalValueDisplay.h"


@implementation AvidaEDOrganismValueDisplayView

- (id) initWithFrame:(NSRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    // initialize
    [txtLabel setStringValue:@""];
    [valueDisplay setDisplayValue:0];
  }
  
  return self;
}

@synthesize valueDisplay;

@end
