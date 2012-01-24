//
//  Genome.h
//  viewer-macos
//
//  Created by David Michael Bryson on 1/24/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "avida/core/Genome.h"

extern NSString* const AvidaPasteboardTypeGenome;


@interface Genome : NSObject  <NSCoding, NSPasteboardWriting, NSPasteboardReading> {
  NSString* name;
  NSString* genomeStr;
}
- (Genome*) initWithGenome:(NSString*)genome name:(NSString*)genomeName;

@property (readonly, nonatomic) NSString* genomeStr;
@property (readonly, nonatomic) NSString* name;


+ (Genome*) genomeFromPasteboard:(NSPasteboard*)pboard;
+ (void) writeGenome:(Genome*)genome toPasteboard:(NSPasteboard*)pboard;
@end
