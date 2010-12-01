//
//  AvidaAppDelegate.m
//  Avida
//
//  Created by David Bryson on 10/20/10.
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

#import "AvidaAppDelegate.h"

#import "AvidaMainWindowController.h"
#import "AvidaEDController.h"


@implementation AvidaAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
}


- (IBAction)newAvida: (id)sender {
  AvidaMainWindowController* ctrl = [[AvidaMainWindowController alloc] initWithAppDelegate:self];
  if (ctrl == nil) {
    NSLog(@"Error loading Avida-MainWindow NIB");
  } else {
    [windows addObject:ctrl];
  }
}


- (IBAction)newAvidaED: (id)sender {
  AvidaEDController* ctrl = [[AvidaEDController alloc] initWithAppDelegate:self];
  if (ctrl == nil) {
    NSLog(@"Error loading Avida-ED-MainWindow NIB");
  } else {
    [windows addObject:ctrl];
  }
}


- (void)removeWindow: (id)sender {
  [windows removeObject:sender];
}


@end
