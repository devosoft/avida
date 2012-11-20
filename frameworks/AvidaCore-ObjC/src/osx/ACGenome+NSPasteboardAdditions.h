//
//  ACGenome+NSPasteboardAdditions.h
//  AvidaCore-ObjC
//
//  Created by David Bryson on 11/15/12.
//
//

#import <AvidaCore/ACGenome.h>

@interface ACGenome (NSPasteboardAdditions) <NSPasteboardReading, NSPasteboardWriting>

// NSPasteboardReading
+ (NSArray*) readableTypesForPasteboard:(NSPasteboard*)pboard;
+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString*)type pasteboard:(NSPasteboard*)pboard;

// NSPasteboardWriting
- (NSArray*) writableTypesForPasteboard:(NSPasteboard*)pasteboard;
- (id) pasteboardPropertyListForType:(NSString*)type;


// ACGenome utility methods
+ (ACGenome*) genomeFromPasteboard:(NSPasteboard*)pboard;
+ (void) writeGenome:(ACGenome*)genome toPasteboard:(NSPasteboard*)pboard;

@end
