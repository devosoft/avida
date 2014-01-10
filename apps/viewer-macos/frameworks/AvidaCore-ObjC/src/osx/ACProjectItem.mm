//
//  ACProjectItem.mm
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

#import <AvidaCore/ACProjectItem.h>
#import <AvidaCore/Definitions_Private.h>
#import <AvidaCore/Utilities.h>


NSString* const ACPasteboardTypeProjectItem = ACMakeIdentfier(ACFrameworkIdentifier, @"ProjectItem");


// ACProjectItem Implementation
// --------------------------------------------------------------------------------------------------------------

@implementation ACProjectItem

@synthesize title;
@synthesize freezer_id;
@synthesize badgeValue;
@synthesize icon;
@synthesize children;


- (BOOL) hasBadge
{
  return (badgeValue != -1);
}


- (BOOL) hasChildren
{
  return [children count] > 0;
}


- (BOOL) hasIcon
{
  return (icon != nil);
}


// ACProjectItem Initialization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - ACProjectItem Initialization

- (ACProjectItem*) init
{
  self = [super init];
  
  if (self) {
    badgeValue = -1;
  }
  
  return self;
}


+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle
{
  return [ACProjectItem itemWithTitle:itemTitle icon:nil];
}


+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle icon:(NSImage*)itemIcon
{
  ACProjectItem* item = [[ACProjectItem alloc] init];
  
  if (item) {
    item.title = itemTitle;
    item.icon = itemIcon;
  }
  
  return item;
}


+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle
{
  return [ACProjectItem itemWithFreezerID:fid title:itemTitle icon:nil];
}


+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle icon:(NSImage*)itemIcon
{
  ACProjectItem* item = [[ACProjectItem alloc] init];
  
  if (item) {
    item.title = itemTitle;
    item.freezer_id = fid;
    item.icon = itemIcon;
  }
  
  return item;
}



- (void) encodeWithCoder:(NSCoder*)encoder {
  [encoder encodeObject:title forKey:@"title"];
  [encoder encodeInt:freezer_id.type forKey:@"type"];
  [encoder encodeInt:freezer_id.identifier forKey:@"identifier"];
  [encoder encodeInteger:badgeValue forKey:@"badgeValue"];
  [encoder encodeObject:icon forKey:@"icon"];
}

- (id) initWithCoder:(NSCoder*)decoder {
  title = [decoder decodeObjectForKey:@"title"];
  freezer_id.type = (Avida::Viewer::FreezerObjectType)[decoder decodeIntForKey:@"type"];
  freezer_id.identifier = (Avida::Viewer::FreezerObjectType)[decoder decodeIntForKey:@"identifier"];
  badgeValue = [decoder decodeIntegerForKey:@"badgeValue"];
  icon = [decoder decodeObjectForKey:@"icon"];
  return self;
}


// NSPasteboardReading
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSPasteboardReading

+ (NSArray*) readableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* readableTypes = nil;
  if (!readableTypes) {
    readableTypes = @[ACPasteboardTypeProjectItem];
  }
  return readableTypes;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString*)type pasteboard:(NSPasteboard*)pboard {
  if ([type isEqualToString:ACPasteboardTypeProjectItem]) {
    return NSPasteboardReadingAsKeyedArchive;
  }
  return 0;
}



// NSPasteboardWriting
// --------------------------------------------------------------------------------------------------------------
#pragma mark - NSPasteboardWriting

- (NSArray*) writableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* writableTypes = nil;
  if (!writableTypes) {
    writableTypes = @[ACPasteboardTypeProjectItem];
  }
  return writableTypes;
}

- (id) pasteboardPropertyListForType:(NSString*)type {
  if ([type isEqualToString:ACPasteboardTypeProjectItem]) {
    return [NSKeyedArchiver archivedDataWithRootObject:self];
  }
  return nil;
}



// Pasteboard Utilities
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Pasteboard Utilities

+ (ACProjectItem*) projectItemFromPasteboard:(NSPasteboard*)pboard {
  NSArray* classes = [[NSArray alloc] initWithObjects:[ACProjectItem class], nil];
  NSDictionary* options = [NSDictionary dictionary];
  NSArray* copiedItems = [pboard readObjectsForClasses:classes options:options];
  if (copiedItems != nil && [copiedItems count] > 0) {
    return (ACProjectItem*)[copiedItems objectAtIndex:0];
  }
  
  return nil;
}

+ (void) writeProjectItem:(ACProjectItem*)item toPasteboard:(NSPasteboard*)pboard {
  [pboard writeObjects:@[item]];
}


// --------------------------------------------------------------------------------------------------------------
@end

