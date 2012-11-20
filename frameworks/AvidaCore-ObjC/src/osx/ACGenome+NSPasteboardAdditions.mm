//
//  ACGenome+NSPasteboardAdditions.m
//  AvidaCore-ObjC
//
//  Created by David Bryson on 11/15/12.
//
//

#import "ACGenome+NSPasteboardAdditions.h"

@implementation ACGenome (NSPasteboardAdditions)

+ (NSArray*) readableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* readableTypes = nil;
  if (!readableTypes) {
    readableTypes = [[NSArray alloc] initWithObjects:AvidaPasteboardTypeGenome, nil];
  }
  return readableTypes;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pboard {
  if ([type isEqualToString:AvidaPasteboardTypeGenome]) {
    return NSPasteboardReadingAsKeyedArchive;
  }
  return 0;
}




- (NSArray*) writableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* writableTypes = nil;
  if (!writableTypes) {
    writableTypes = [[NSArray alloc] initWithObjects:AvidaPasteboardTypeGenome, nil];
  }
  return writableTypes;
}

- (id) pasteboardPropertyListForType:(NSString*)type {
  if ([type isEqualToString:AvidaPasteboardTypeGenome]) {
    return [NSKeyedArchiver archivedDataWithRootObject:self];
  }
  return nil;
}




+ (ACGenome*) genomeFromPasteboard:(NSPasteboard*)pboard {
  NSArray* classes = [[NSArray alloc] initWithObjects:[ACGenome class], nil];
  NSDictionary* options = [NSDictionary dictionary];
  NSArray* copiedItems = [pboard readObjectsForClasses:classes options:options];
  if (copiedItems != nil && [copiedItems count] > 0) {
    return (ACGenome*)[copiedItems objectAtIndex:0];
  }
  
  return nil;
}

+ (void) writeGenome:(ACGenome*)genome toPasteboard:(NSPasteboard*)pboard {
  [pboard writeObjects:[NSArray arrayWithObject:genome]];
}

@end
