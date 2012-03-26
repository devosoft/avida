//
//  AvidaEDOrganismValueDisplayView.h
//  viewer-macos
//
//  Created by David Michael Bryson on 3/22/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class AvidaEDGraphicalValueDisplay;

@interface AvidaEDOrganismValueDisplayView : NSView {
  IBOutlet NSTextField* txtLabel;
  IBOutlet AvidaEDGraphicalValueDisplay* valueDisplay;
}

@property (readonly) AvidaEDGraphicalValueDisplay* valueDisplay;

@end
