//
//  AvidaEDExportAccessoryController.m
//  viewer-macos
//
//  Created by David on 5/16/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "AvidaEDExportAccessoryController.h"


@implementation AvidaEDExportAccessoryController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (NSMatrix*) optionMatrix {
  return matOptions;
}

- (NSInteger) selectedOption {
  if (btnOptions != nil) {
    return [btnOptions indexOfSelectedItem];
  }
  
  return -1;
}

- (NSInteger) selectedFormat {
  if (btnFormat != nil) {
    return [btnFormat indexOfSelectedItem];
  }
  
  return -1;
}

@end
