//
//  Freezer.h
//  viewer-macos
//
//  Created by Bryson David Michael on 1/11/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "avida/viewer.h"

extern NSString* const AvidaPasteboardTypeFreezerID;


@interface FreezerItem : NSObject <NSCoding, NSPasteboardWriting, NSPasteboardReading> {
  Avida::Viewer::FreezerID freezerID;
}
- (FreezerItem*) initWithFreezerID:(Avida::Viewer::FreezerID)init_id;
+ (FreezerItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid;
@property (nonatomic, readwrite) Avida::Viewer::FreezerID freezerID;
@end;


@interface Freezer : NSObject
+ (Avida::Viewer::FreezerID) freezerIDFromPasteboard:(NSPasteboard*)pboard;
+ (void) writeFreezerID:(Avida::Viewer::FreezerID)fid toPasteboard:(NSPasteboard*)pboard;
@end
