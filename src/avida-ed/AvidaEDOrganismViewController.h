//
//  AvidaEDOrgansimViewController.h
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 3/5/12.
//  Copyright 2012 Michigan State University. All rights reserved.
//  http://avida.devosoft.org/viewer-macos
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
//  following conditions are met:
//  
//  1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the
//      following disclaimer.
//  2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//      following disclaimer in the documentation and/or other materials provided with the distribution.
//  3.  Neither the name of Michigan State University, nor the names of contributors may be used to endorse or promote
//      products derived from this software without specific prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY MICHIGAN STATE UNIVERSITY AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL MICHIGAN STATE UNIVERSITY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Authors: David M. Bryson <david@programerror.com>
//

#import <Cocoa/Cocoa.h>

#import "Definitions.h"
#import "DropDelegate.h"

#include "avida/viewer/OrganismTrace.h"

@class AvidaEDController;
@class AvidaEDEnvActionsDataSource;
@class AvidaEDOrganismSettingsViewController;
@class AvidaRun;
@class OrganismView;
@class TaskTimelineView;


@interface AvidaEDOrganismViewController : NSViewController <NSPopoverDelegate> {
  IBOutlet OrganismView* orgView;
  IBOutlet TaskTimelineView* timelineView;
  IBOutlet NSView* fullView;
  IBOutlet NSTextField* txtOrgName;
  IBOutlet NSTableView* tblTaskCounts;
  IBOutlet NSSlider* sldStatus;
  
  IBOutlet NSButton* btnBegin;
  IBOutlet NSButton* btnBack;
  IBOutlet NSButton* btnGo;
  IBOutlet NSButton* btnForward;
  IBOutlet NSButton* btnEnd;
  

  // Show/hide instruction details
  IBOutlet NSButton* btnToggleInstInfo;
  IBOutlet NSTextField* txtInstDetailsLabel;
  IBOutlet NSTextField* txtJustExecLabel;
  IBOutlet NSTextField* txtWillExecLabel;
  IBOutlet NSView* viewJustExec;
  IBOutlet NSView* viewWillExec;
  IBOutlet NSLayoutConstraint* layoutOrgViewTop;
  IBOutlet NSLayoutConstraint* layoutTxtJustExecHeight;
  
  // Show/hide various hardware components
  IBOutlet NSButton* btnToggleFunctions;
  IBOutlet NSView* viewFunctions;
  IBOutlet NSLayoutConstraint* layoutFunctionSpacing;
  IBOutlet NSButton* btnToggleRegisters;
  IBOutlet NSView* viewRegisters;
  IBOutlet NSLayoutConstraint* layoutRegisterSpacing;
  IBOutlet NSButton* btnToggleInputBuffer;
  IBOutlet NSView* viewInputBuffer;
  IBOutlet NSLayoutConstraint* layoutInputBufferSpacing;
  IBOutlet NSButton* btnToggleOutputBuffer;
  IBOutlet NSView* viewOutputBuffer;
  IBOutlet NSLayoutConstraint* layoutOutputBufferSpacing;
  IBOutlet NSButton* btnToggleCurStack;
  IBOutlet NSView* viewCurStack;
  
  
  IBOutlet AvidaEDOrganismSettingsViewController* ctlrSettings;
  IBOutlet NSPanel* panelSettings;
  NSPopover* popoverSettings;
  
  NSMutableArray* arrRegisters;
  NSMutableArray* arrInputBuffer;
  NSMutableArray* arrOutputBuffer;
  NSMutableArray* arrCurStack;
  IBOutlet NSArrayController* arrctlrRegisters;
  IBOutlet NSArrayController* arrctlrInputBuffer;
  IBOutlet NSArrayController* arrctlrOutputBuffer;
  IBOutlet NSArrayController* arrctlrCurStack;

  IBOutlet NSTextView* txtJustExec;
  IBOutlet NSTextView* txtWillExec;
  
  AvidaRun* testWorld;
  Avida::Viewer::OrganismTracePtr trace;
  int curSnapshotIndex;
  
  AvidaEDEnvActionsDataSource* envActions;
  
  NSTimer* tmrAnim;
  
  id<DropDelegate> dropDelegate;
}

- (id) initWithWorld:(AvidaRun*)world;

- (void) setDropDelegate:(id<DropDelegate>)delegate;

@property (readonly) NSMutableArray* arrRegisters;
@property (readonly) NSMutableArray* arrInputBuffer;
@property (readonly) NSMutableArray* arrOutputBuffer;
@property (readonly) NSMutableArray* arrCurStack;


- (IBAction) selectSnapshot:(id)sender;
- (IBAction) nextSnapshot:(id)sender;
- (IBAction) prevSnapshot:(id)sender;
- (IBAction) firstSnapshot:(id)sender;
- (IBAction) lastSnapshot:(id)sender;
- (IBAction) toggleAnimation:(id)sender;
- (IBAction) showSettings:(id)sender;

- (IBAction) toggleInstInfo:(id)sender;
- (IBAction) toggleFunctions:(id)sender;
- (IBAction) toggleRegisters:(id)sender;
- (IBAction) toggleInputBuffer:(id)sender;
- (IBAction) toggleOutputBuffer:(id)sender;
- (IBAction) toggleCurStack:(id)sender;


- (void) setGenome:(Avida::GenomePtr)genome withName:(NSString*)name;
- (NSString*) getOrganismName;

- (void) exportGraphic:(ExportGraphicsFileFormat)format withOptions:(NSMatrix*)optMat toURL:(NSURL*)url;

@end
