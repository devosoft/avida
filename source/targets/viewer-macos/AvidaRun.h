//
//  AvidaRun.h
//  Avida
//
//  Created by David on 10/27/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>


class cCoreViewDriver;


@interface AvidaRun : NSObject {
@protected
  cCoreViewDriver* driver;
}

-(id)init;
-(AvidaRun*)initWithDirectory: (NSURL*) dir;

-(bool)isPaused;
-(void)pause;
-(void)resume;

@end
