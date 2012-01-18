//
//  Freezer.m
//  viewer-macos
//
//  Created by Bryson David Michael on 1/11/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
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
