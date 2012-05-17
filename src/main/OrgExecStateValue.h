//
//  OrgExecStateValue.h
//  viewer-macos
//
//  Created by David Michael Bryson on 3/22/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface OrgExecStateValue : NSObject {
  NSString* prefix;
  int value;
}

+ (void) initialize;

- (id) initWithPrefix:(NSString*)prefix;

@property (readonly) NSString* prefix;
@property (readwrite) int value;
@property (readonly) NSString* decimalString;
@property (readonly) NSString* hexString;

@end
