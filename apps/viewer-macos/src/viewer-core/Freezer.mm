//
//  Freezer.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 1/11/12.
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

#import "Freezer.h"

NSString* const AvidaPasteboardTypeFreezerID = @"org.devosoft.avida.freezer.id";

@implementation FreezerItem

- (FreezerItem*) initWithFreezerID:(Avida::Viewer::FreezerID)init_id {
  freezerID = init_id;
  return self;
}

+ (FreezerItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid {
  return [[FreezerItem alloc] initWithFreezerID:fid];
}

@synthesize freezerID;


- (void) encodeWithCoder:(NSCoder*)encoder {
  [encoder encodeInt:freezerID.type forKey:@"type"];
  [encoder encodeInt:freezerID.identifier forKey:@"identifier"];
}

- (id) initWithCoder:(NSCoder*)decoder {
  freezerID.type = (Avida::Viewer::FreezerObjectType)[decoder decodeIntForKey:@"type"];
  freezerID.identifier = (Avida::Viewer::FreezerObjectType)[decoder decodeIntForKey:@"identifier"];
  return self;
}


- (NSArray*) writableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* writableTypes = nil;
  if (!writableTypes) {
    writableTypes = [[NSArray alloc] initWithObjects:AvidaPasteboardTypeFreezerID, nil];
  }
  return writableTypes;
}

- (id) pasteboardPropertyListForType:(NSString*)type {
  if ([type isEqualToString:AvidaPasteboardTypeFreezerID]) {
    return [NSKeyedArchiver archivedDataWithRootObject:self];
  }
  return nil;
}


+ (NSArray*) readableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* readableTypes = nil;
  if (!readableTypes) {
    readableTypes = [[NSArray alloc] initWithObjects:AvidaPasteboardTypeFreezerID, nil];
  }
  return readableTypes;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pboard {
  if ([type isEqualToString:AvidaPasteboardTypeFreezerID]) {
    return NSPasteboardReadingAsKeyedArchive;
  }
  return 0;
}
@end


@implementation Freezer

+ (Avida::Viewer::FreezerID) freezerIDFromPasteboard:(NSPasteboard*)pboard
{
  NSArray* classes = [[NSArray alloc] initWithObjects:[FreezerItem class], nil];
  NSDictionary* options = [NSDictionary dictionary];
  NSArray* copiedItems = [pboard readObjectsForClasses:classes options:options];
  if (copiedItems != nil && [copiedItems count] > 0) {
    return [(FreezerItem*)[copiedItems objectAtIndex:0] freezerID];
  }
  
  return Avida::Viewer::FreezerID();
}

+ (void) writeFreezerID:(Avida::Viewer::FreezerID)fid toPasteboard:(NSPasteboard*)pboard
{
  [pboard writeObjects:[[NSArray alloc] initWithObjects:[FreezerItem itemWithFreezerID:fid], nil]];
}

@end
