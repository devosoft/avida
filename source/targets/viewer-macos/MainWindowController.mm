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

- (void) awakeFromNib {
  // Initialized the default path of the runDirControl to the user's documents directory
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  
  if ([urls count] > 0) {
    NSURL* userDocumentsURL = [urls objectAtIndex:0];
    [runDirControl setURL:userDocumentsURL];
  }
}

- (IBAction) setRunDir:(id)sender {
  
  // Set the current path to a selected sub-component of the path when clicked
  NSPathComponentCell* path_clicked = [runDirControl clickedPathComponentCell];
  if (path_clicked != nil) {
    [runDirControl setURL:[path_clicked URL]];
  }
  

  // Get and print out the URL to standard out for debugging purposes
  NSURL* path_url = [runDirControl URL];
  NSString* path_str = [path_url path];
  
  std::cout << "Path:" << [path_str cStringUsingEncoding:NSASCIIStringEncoding] << std::endl;
}

@end
