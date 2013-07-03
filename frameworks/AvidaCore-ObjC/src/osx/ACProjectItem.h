//
//  ACProjectItem.h
//  avida/apps/viewer-macos/frameworks/AvidaCore-OSX
//
//  Created by David M. Bryson on 3/20/13.
//  Copyright 2013 Michigan State University. All rights reserved.
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

#import <AvidaCore/ACProject.h>
#import <Cocoa/Cocoa.h>


extern NSString* const ACPasteboardTypeProjectItem;


// ACProjectItem Interface
// --------------------------------------------------------------------------------------------------------------

@interface ACProjectItem : NSObject <NSCoding, NSPasteboardReading, NSPasteboardWriting> {
  NSString* title;
  Avida::Viewer::FreezerID freezer_id;
  
  NSInteger badgeValue;
  NSImage* icon;
  
  NSArray* children;
}

@property (nonatomic, copy) NSString* title;
@property (nonatomic, assign) Avida::Viewer::FreezerID freezer_id;
@property NSInteger badgeValue;
@property (nonatomic, retain) NSImage* icon;
@property (nonatomic, copy) NSArray* children;

- (BOOL) hasBadge;
- (BOOL) hasChildren;
- (BOOL) hasIcon;


// Initialization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Initialization

- (ACProjectItem*) init;

+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle;
+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle icon:(NSImage*)itemIcon;
+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle;
+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle icon:(NSImage*)itemIcon;


// NSCoder
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSCoder

- (void) encodeWithCoder:(NSCoder*)encoder;
- (id) initWithCoder:(NSCoder*)decoder;


// NSPasteboardReading
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSPasteboardReading

+ (NSArray*) readableTypesForPasteboard:(NSPasteboard*)pboard;
+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString*)type pasteboard:(NSPasteboard*)pboard;


// NSPasteboardWriting
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSPasteboardWriting

- (NSArray*) writableTypesForPasteboard:(NSPasteboard*)pasteboard;
- (id) pasteboardPropertyListForType:(NSString*)type;


// Pasteboard Utilities
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Pasteboard Utilities

+ (ACProjectItem*) projectItemFromPasteboard:(NSPasteboard*)pboard;
+ (void) writeProjectItem:(ACProjectItem*)item toPasteboard:(NSPasteboard*)pboard;


// --------------------------------------------------------------------------------------------------------------
@end;



