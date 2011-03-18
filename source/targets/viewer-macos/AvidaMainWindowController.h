//
//  MainWindowController.h
//  Avida
//
//  Created by David Bryson on 10/21/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//
//
//  This file is part of Avida.
//
//  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
//  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along with Avida.
//  If not, see <http://www.gnu.org/licenses/>.
//

#import <Cocoa/Cocoa.h>

#import "CoreViewListener.h"

@class AvidaAppDelegate;
@class AvidaRun;
@class MapGridView;


@interface AvidaMainWindowController : NSWindowController <CoreViewListener, NSPathControlDelegate, NSWindowDelegate> {
  IBOutlet NSPathControl* runDirControl;
  IBOutlet NSButton* btnRunState;
  IBOutlet NSTextField* txtUpdate;
  IBOutlet MapGridView* mapView;
  
  AvidaAppDelegate* app;
  
  AvidaRun* currentRun;
  Avida::CoreView::cListener* listener;
}

// Init and Dealloc Methods
-(id)initWithAppDelegate:(AvidaAppDelegate*)delegate;

-(void)dealloc;
-(void)finalize;


// NSWindowController Methods
-(void)windowDidLoad;


// Actions
-(IBAction)setRunDir:(id)sender;
-(IBAction)toggleRunState:(id)sender;


// NSPathControlDelegate Protocol
-(void)pathControl:(NSPathControl*)pathControl willDisplayOpenPanel:(NSOpenPanel*)openPanel;


// NSWindowDelegate Protocol
-(void)windowWillClose:(NSNotification*)notification;


// Listener Methods
@property (readonly) Avida::CoreView::cListener* listener;

-(void)handleMap:(CoreViewMap*)object;
-(void)handleUpdate:(CoreViewUpdate*)object;


@end
