//
//  FreezerOutlineView.m
//  viewer-macos
//
//  Created by Bryson David Michael on 1/19/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "FreezerOutlineView.h"

#import "Freezer.h"


@implementation FreezerOutlineView

- (void) awakeFromNib {
  [super awakeFromNib];
  
  imgConfig = [NSImage imageNamed:@"petri128empty.png"];
  imgGenome = [NSImage imageNamed:@"genome65.png"];
  imgWorld = [NSImage imageNamed:@"petri128.png"];
}


- (void) dragImage:(NSImage*)anImage at:(NSPoint)imageLoc offset:(NSSize)mouseOffset event:(NSEvent*)theEvent 
         pasteboard:(NSPasteboard*)pboard source:(id)sourceObject slideBack:(BOOL)slideBack
{
  Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
  
  NSImage* selImage = anImage;
  
  switch (fid.type) {
    case Avida::Viewer::CONFIG: selImage = imgConfig; break;
    case Avida::Viewer::GENOME: selImage = imgGenome; break;
    case Avida::Viewer::WORLD:  selImage = imgWorld;  break;
    default: break;
  }
  
  NSPoint location;
  NSSize size = [selImage size];
  location.x = ([self bounds].size.width - size.width) / 2;
  location.y = imageLoc.y - ([anImage size].height / 2) + (size.height / 2);
  
  printf("imageLoc: (%f, %f) loc: (%f, %f)\n", imageLoc.x, imageLoc.y, location.x, location.y);

  [super dragImage:selImage at:location offset:NSMakeSize(0,0) event:theEvent pasteboard:pboard source:sourceObject slideBack:slideBack];
}

@end
