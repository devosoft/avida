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


- (void) dragImage:(NSImage*)anImage at:(NSPoint)imageLoc offset:(NSSize)mouseOffset event:(NSEvent*)event 
         pasteboard:(NSPasteboard*)pboard source:(id)sourceObject slideBack:(BOOL)slideBack
{
  Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
  
  NSPoint clickLocation = [self convertPoint:[event locationInWindow] fromView:nil];
  NSPoint dragLocation;
  NSImage* selImage = anImage;
  
  switch (fid.type) {
    case Avida::Viewer::CONFIG:
      selImage = imgConfig;
      dragLocation.x = clickLocation.x - ([selImage size].width / 2);
      dragLocation.y = clickLocation.y + ([selImage size].height / 2);
      break;
    case Avida::Viewer::GENOME:
      selImage = imgGenome;
      dragLocation.x = clickLocation.x;
      dragLocation.y = clickLocation.y + [selImage size].height;
      break;
    case Avida::Viewer::WORLD:
      selImage = imgWorld;
      dragLocation.x = clickLocation.x - ([selImage size].width / 2);
      dragLocation.y = clickLocation.y + ([selImage size].height / 2);
      break;
    default: break;
  }
  
  [super dragImage:selImage at:dragLocation offset:NSMakeSize(0,0) event:event pasteboard:pboard source:sourceObject slideBack:slideBack];
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
