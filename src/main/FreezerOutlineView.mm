//
//  FreezerOutlineView.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 1/19/12.
//  Copyright 2012 Michigan State University. All rights reserved.
//  http://avida.devosoft.org/viewer-macos
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
//  following conditions are met:
//  
//  1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the
//      following disclaimer.
//  2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//      following disclaimer in the documentation and/or other materials provided with the distribution.
//  3.  Neither the name of Michigan State University, nor the names of contributors may be used to endorse or promote
//      products derived from this software without specific prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY MICHIGAN STATE UNIVERSITY AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL MICHIGAN STATE UNIVERSITY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Authors: David M. Bryson <david@programerror.com>
//

#import "FreezerOutlineView.h"

#import "Freezer.h"


@implementation FreezerOutlineView

- (void) awakeFromNib {
  [super awakeFromNib];
  
  imgConfig = [NSImage imageNamed:@"petri128empty.png"];
  imgGenome = [NSImage imageNamed:@"genome65.png"];
  imgWorld = [NSImage imageNamed:@"avida-ed-icon.png"];
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
