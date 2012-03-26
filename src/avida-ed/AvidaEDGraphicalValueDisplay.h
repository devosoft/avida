//
//  AvidaEDGraphicalValueDisplay.h
//  viewer-macos
//
//  Created by David Michael Bryson on 3/22/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AvidaEDGraphicalValueDisplay : NSView {
  int displayValue;
}

@property (readwrite, nonatomic) int displayValue;

@end
