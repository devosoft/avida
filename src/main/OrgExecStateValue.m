//
//  OrgExecStateValue.m
//  viewer-macos
//
//  Created by David Michael Bryson on 3/22/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "OrgExecStateValue.h"

@implementation OrgExecStateValue

+ (void) initialize {
  [self exposeBinding:@"prefix"];
  [self exposeBinding:@"value"];
}

- (id) initWithPrefix:(NSString*)in_prefix {
  self = [super init];
  if (self) {
    prefix = in_prefix;
  }
  
  return self;
}

@synthesize prefix;

- (int) value {
  return value;
}

- (void) setValue:(int)_value {
  if (value != _value) {
    [self willChangeValueForKey:@"value"];
    [self willChangeValueForKey:@"decimalString"];
    [self willChangeValueForKey:@"hexString"];
    value = _value;
    [self didChangeValueForKey:@"hexString"];
    [self didChangeValueForKey:@"decimalString"];
    [self didChangeValueForKey:@"value"];
  }
}


- (NSString*) decimalString {
  return [NSString stringWithFormat:@"%d", value];
}

- (NSString*) hexString {
  return [NSString stringWithFormat:@"%08X", value];
}

@end
