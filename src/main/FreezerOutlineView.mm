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
  
  [super dragImage:selImage at:location offset:NSMakeSize(0,0) event:theEvent pasteboard:pboard source:sourceObject slideBack:slideBack];
}

- (BOOL) performKeyEquivalent:(NSEvent*)theEvent {
  NSString* chars = [theEvent charactersIgnoringModifiers];
  
  if ([theEvent type] == NSKeyDown && [chars length] == 1) {
    int val = [chars characterAtIndex:0];
    
    // check for a delete
    if (val == 127 || val == 63272) {
      if ([[self delegate] respondsToSelector:@selector(outlineViewDidReceiveDeleteKey:)]) {
        [[self delegate] performSelector:@selector(outlineViewDidReceiveDeleteKey:) withObject:self];
        return YES;
      }
    }
    
    // check for the enter / space to open it up
    else if (val == 13 /*return*/ || val == 32 /*space bar*/) {
      
      if ([[self delegate] respondsToSelector:@selector(outlineViewDidReceiveEnterOrSpaceKey:)]) {
        [[self delegate] performSelector:@selector(outlineViewDidReceiveEnterOrSpaceKey:) withObject:self];
        return YES;
      }
    }
  }
  
  return [super performKeyEquivalent:theEvent];
}

@end
