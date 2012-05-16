//
//  AvidaEDExportAccessoryController.h
//  viewer-macos
//
//  Created by David on 5/16/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AvidaEDExportAccessoryController : NSViewController {
  IBOutlet NSMatrix* matOptions;
  IBOutlet NSPopUpButton* btnOptions;
  IBOutlet NSPopUpButton* btnFormat;
}

- (NSMatrix*) optionMatrix;
- (NSInteger) selectedOption;
- (NSInteger) selectedFormat;


@end
