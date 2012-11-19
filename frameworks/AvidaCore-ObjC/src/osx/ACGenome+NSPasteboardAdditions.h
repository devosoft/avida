//
//  ACGenome+NSPasteboardAdditions.h
//  AvidaCore-ObjC
//
//  Created by David Bryson on 11/15/12.
//
//

#import <AvidaCore/ACGenome.h>

@interface ACGenome (NSPasteboardAdditions)

+ (ACGenome*) genomeFromPasteboard:(NSPasteboard*)pboard;
+ (void) writeGenome:(ACGenome*)genome toPasteboard:(NSPasteboard*)pboard;

@end
