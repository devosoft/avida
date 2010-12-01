//
//  AvidaEDController.h
//  Avida
//
//  Created by David on 11/30/10.
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

@class AvidaAppDelegate;
@class AvidaRun;
@class MapGridView;


@interface AvidaEDController : NSWindowController <NSWindowDelegate> {
  IBOutlet NSButton* btnRunState;
  IBOutlet NSTextField* txtUpdate;
  IBOutlet MapGridView* mapView;
  
  AvidaAppDelegate* app;
  
  AvidaRun* currentRun;
  
}

// Init and Dealloc Methods
-(id)initWithAppDelegate:(AvidaAppDelegate*)delegate;

-(void)dealloc;
-(void)finalize;


-(IBAction)toggleRunState:(id)sender;


// NSWindowDelegate Protocol
-(void)windowWillClose:(NSNotification*)notification;


// Listener Methods
-(void)handleMap:(id)object;
-(void)handleUpdate:(id)object;


@end
