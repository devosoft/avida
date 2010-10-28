//
//  MainWindowController.h
//  Avida
//
//  Created by David Bryson on 10/21/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//

#import <Foundation/Foundation.h>

@class AvidaRun;

@interface MainWindowController : NSObject {
  IBOutlet NSPathControl* runDirControl;
  IBOutlet NSButton* btnRunState;
  AvidaRun* currentRun;
}

-(void)awakeFromNib;

-(IBAction)setRunDir:(id)sender;
-(IBAction)toggleRunState:(id)sender;

// NSPathControlDelegate Protocol
-(void)pathControl:(NSPathControl*)pathControl willDisplayOpenPanel:(NSOpenPanel*)openPanel

@end
