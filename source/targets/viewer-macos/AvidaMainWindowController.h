//
//  MainWindowController.h
//  Avida
//
//  Created by David Bryson on 10/21/10.
//  Copyright 2010 Michigan State University. All rights reserved.
//
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2
//  of the License.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
  cCoreViewListener* listener;
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
-(void)handleMap:(CoreViewMap*)object;
-(void)handleUpdate:(CoreViewUpdate*)object;


@property (readonly) cCoreViewListener* listener;

@end
