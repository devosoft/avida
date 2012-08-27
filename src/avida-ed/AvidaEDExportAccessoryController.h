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
  
  NSSavePanel* saveDlg;
}

- (NSMatrix*) optionMatrix;
- (NSInteger) selectedOption;
- (NSInteger) selectedFormat;

- (IBAction) changeFormat:(id)sender;

@property (readwrite) NSSavePanel* saveDlg;

@end
