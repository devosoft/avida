//
//  MainWindowController.mm
//  Avida
//
//  Created by David Bryson on 10/21/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//

#import "MainWindowController.h"

#include <iostream>

@implementation MainWindowController

- (IBAction) setRunDir:(id)sender {
  NSPathComponentCell* path_clicked = [runDirControl clickedPathComponentCell];
  if (path_clicked != nil) {
    [runDirControl setURL:[path_clicked URL]];
  }
  NSURL* path_url = [runDirControl URL];
  NSString* path_str = [path_url path];
  
  std::cout << "Path:" << [path_str cStringUsingEncoding:NSASCIIStringEncoding] << std::endl;
  
}

@end
