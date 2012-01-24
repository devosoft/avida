//
//  Genome.m
//  viewer-macos
//
//  Created by David Michael Bryson on 1/24/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "Genome.h"

#import "NSString+Apto.h"

NSString* const AvidaPasteboardTypeGenome = @"org.devosoft.avida.genome";

@implementation Genome

- (Genome*) initWithGenome:(NSString*)genome name:(NSString*)genomeName {
  genomeStr = genome;
  name = genomeName;
  return self;
}

@synthesize genomeStr;
@synthesize name;

+ (Genome*) genomeFromPasteboard:(NSPasteboard*)pboard {
  NSArray* classes = [[NSArray alloc] initWithObjects:[Genome class], nil];
  NSDictionary* options = [NSDictionary dictionary];
  NSArray* copiedItems = [pboard readObjectsForClasses:classes options:options];
  if (copiedItems != nil && [copiedItems count] > 0) {
    return (Genome*)[copiedItems objectAtIndex:0];
  }
  
  return nil;
}

+ (void) writeGenome:(Genome*)genome toPasteboard:(NSPasteboard*)pboard {
  [pboard writeObjects:[NSArray arrayWithObject:genome]];
}





- (void) encodeWithCoder:(NSCoder*)encoder {
  [encoder encodeObject:genomeStr];
  [encoder encodeObject:name];
}

- (id) initWithCoder:(NSCoder*)decoder {
  genomeStr = [[decoder decodeObject] retain];
  name = [[decoder decodeObject] retain];
  return self;
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


@end
