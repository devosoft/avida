//
//  ACGenome+NSPasteboardAdditions.m
//  AvidaCore-ObjC
//
//  Created by David Bryson on 11/15/12.
//
//

#import "ACGenome+NSPasteboardAdditions.h"

@implementation ACGenome (NSPasteboardAdditions)

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
