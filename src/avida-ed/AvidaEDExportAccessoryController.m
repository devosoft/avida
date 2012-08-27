//
//  AvidaEDExportAccessoryController.m
//  viewer-macos
//
//  Created by David on 5/16/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "AvidaEDExportAccessoryController.h"

#import "Definitions.h"


@implementation AvidaEDExportAccessoryController

@synthesize saveDlg;

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

- (IBAction) changeFormat:(id)sender {
  switch ([btnFormat indexOfSelectedItem]) {
    case EXPORT_GRAPHICS_JPEG:
      [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"public.jpeg"]];
      break;
    case EXPORT_GRAPHICS_PNG:
      [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"public.png"]];
      break;
    case EXPORT_GRAPHICS_PDF:
      [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"com.adobe.pdf"]];
      break;
    default:
      [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"public.jpeg"]];
      break;
  }
}

@end
