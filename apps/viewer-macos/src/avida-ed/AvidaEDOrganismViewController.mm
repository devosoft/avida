//
//  AvidaEDOrganismViewController.mm
//  avida/apps/viewer-macos
//
//  Created by David Michael Bryson on 3/5/12.
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

#import "AvidaEDOrganismViewController.h"

#import <AvidaCore/AvidaCore.h>

#import "AvidaEDController.h"
#import "AvidaEDEnvActionsDataSource.h"
#import "AvidaEDOrganismSettingsViewController.h"
#import "CAAnimationBlockDelegate.h"
#import "OrgExecStateValue.h"
#import "AvidaRun.h"
#import "Freezer.h"
#import "NSFileManager+TemporaryDirectory.h"
#import "OrganismView.h"
#import "TaskTimelineView.h"

#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Manager.h"
#include "avida/environment/Product.h"
#include "avida/viewer/Freezer.h"


static const CGFloat SPLIT_LEFT_MIN = 300;
static const CGFloat SPLIT_RIGHT_MIN = 202;


@interface AvidaEDOrganismViewController ()
- (void) viewDidLoad;
@end


@interface AvidaEDOrganismViewController (hidden)
- (void) setSnapshot:(int)snapshot;
- (void) setTaskCountsWithSnapshot:(const Avida::Viewer::HardwareSnapshot&)snapshot;
- (void) setStateDisplayWithSnapshot:(const Avida::Viewer::HardwareSnapshot&)snapshot;

- (void) startAnimation;
- (void) stopAnimation;
- (void) nextAnimationFrame:(id)sender;

- (void) createSettingsPopover;
- (NSString*) descriptionOfInst:(Avida::Instruction)inst;

- (void) drawOrgWithTimelineInRect:(NSRect)rect inContext:(NSGraphicsContext*)gc;

- (void) splitViewAnimationEnd:(NSNumber*)collapsed;

@end


@implementation AvidaEDOrganismViewController (hidden)

- (void) setSnapshot:(int)snapshot {
  assert(trace);

  if (snapshot >= 0 && snapshot < trace->SnapshotCount()) {
    curSnapshotIndex = snapshot;
    [self setTaskCountsWithSnapshot:trace->Snapshot(snapshot)];
    [self setStateDisplayWithSnapshot:trace->Snapshot(snapshot)];
    [orgView setSnapshot:&trace->Snapshot(snapshot)];
    [timelineView setCurrentPoint:snapshot];
  }
  
  [sldStatus setIntValue:curSnapshotIndex];
  [txtCycle setIntValue:curSnapshotIndex];
  
  if (curSnapshotIndex == 0) {
    [viewOffspringDrag removeFromSuperview];
    [btnBack setEnabled:NO];
    [btnGo setEnabled:YES];
    [btnForward setEnabled:YES];
    [btnEnd setEnabled:YES];
    [[[txtJustExec textStorage] mutableString] setString:@"(none)"];
    [[[txtWillExec textStorage] mutableString] setString:[self descriptionOfInst:trace->Snapshot(curSnapshotIndex).NextInstruction()]];
  } else if (curSnapshotIndex == (trace->SnapshotCount() - 1)) {
    if (trace->Snapshot(snapshot).IsPostDivide()) [orgView addSubview:viewOffspringDrag];
    [self stopAnimation];
    [btnBack setEnabled:YES];
    [btnGo setEnabled:NO];
    [btnForward setEnabled:NO];
    [btnEnd setEnabled:NO];
    [[[txtJustExec textStorage] mutableString] setString:[self descriptionOfInst:trace->Snapshot(curSnapshotIndex - 1).NextInstruction()]];
    [[[txtWillExec textStorage] mutableString] setString:@"(none)"];
  } else {
    [viewOffspringDrag removeFromSuperview];
    [btnBack setEnabled:YES];
    [btnGo setEnabled:YES];
    [btnForward setEnabled:YES];
    [btnEnd setEnabled:YES];
    [[[txtJustExec textStorage] mutableString] setString:[self descriptionOfInst:trace->Snapshot(curSnapshotIndex - 1).NextInstruction()]];
    [[[txtWillExec textStorage] mutableString] setString:[self descriptionOfInst:trace->Snapshot(curSnapshotIndex).NextInstruction()]];
  }
}



- (void) setTaskCountsWithSnapshot:(const Avida::Viewer::HardwareSnapshot&)snapshot {
  
  for (NSUInteger i = 0; i < [envActions entryCount]; i++) {
    NSString* entry_name = [envActions entryAtIndex:i];
    NSString* local_name = entry_name;
    if ([entry_name isEqual:@"oro"]) {
      entry_name = @"or";
    } else if ([entry_name isEqual:@"ant"]) {
      entry_name = @"andn";
    } else if ([entry_name isEqual:@"nan"]) {
      entry_name = @"nand";
    }

    [envActions updateEntry:local_name withValue:[NSNumber numberWithInt:snapshot.FunctionCount([entry_name UTF8String])]];
  }
  [tblTaskCounts reloadData];
}


- (void) setStateDisplayWithSnapshot:(const Avida::Viewer::HardwareSnapshot&)snapshot {
  
  // Handle registers
  if ([arrRegisters count] != snapshot.NumRegisters()) {
    NSRange range = NSMakeRange(0, [[arrctlrRegisters arrangedObjects] count]);
    [arrctlrRegisters removeObjectsAtArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:range]];
    for (int i = 0; i < snapshot.NumRegisters(); i++) {
      NSString* prefix = [NSString stringWithFormat:@"%cX: ", (char)('A' + i)];
      OrgExecStateValue* sv = [[OrgExecStateValue alloc] initWithPrefix:prefix];
      [sv setValue:snapshot.Register(i)];
      [arrctlrRegisters addObject:sv];
    }
  } else {
    for (int i = 0; i < snapshot.NumRegisters(); i++)
      [(OrgExecStateValue*)[arrRegisters objectAtIndex:i] setValue:snapshot.Register(i)];
  }
  
  // Handle input buffer
  const Apto::Array<int>& input_buf = snapshot.Buffer("input");
  if ([arrInputBuffer count] != input_buf.GetSize()) {
    NSRange range = NSMakeRange(0, [[arrctlrInputBuffer arrangedObjects] count]);
    [arrctlrInputBuffer removeObjectsAtArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:range]];
    for (int i = 0; i < input_buf.GetSize(); i++) {
      OrgExecStateValue* sv = [[OrgExecStateValue alloc] initWithPrefix:@""];
      [sv setValue:input_buf[i]];
      [arrctlrInputBuffer addObject:sv];
    }
  } else {
    for (int i = 0; i < input_buf.GetSize(); i++)
      [(OrgExecStateValue*)[arrInputBuffer objectAtIndex:i] setValue:input_buf[i]];
  }
  
  // handle output buffer
  const Apto::Array<int>& output_buf = snapshot.Buffer("output");
  if ([arrOutputBuffer count] != output_buf.GetSize()) {
    NSRange range = NSMakeRange(0, [[arrctlrOutputBuffer arrangedObjects] count]);
    [arrctlrOutputBuffer removeObjectsAtArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:range]];
    for (int i = 0; i < output_buf.GetSize(); i++) {
      OrgExecStateValue* sv = [[OrgExecStateValue alloc] initWithPrefix:@""];
      [sv setValue:output_buf[i]];
      [arrctlrOutputBuffer addObject:sv];
    }
  } else {
    for (int i = 0; i < output_buf.GetSize(); i++)
      [(OrgExecStateValue*)[arrOutputBuffer objectAtIndex:i] setValue:output_buf[i]];
  }
  
  // handle stack a
  const Apto::Array<int>& stack_a = snapshot.Buffer("stack A");
  if ([arrCurStack count] != ((stack_a.GetSize() > 2) ? 2 : stack_a.GetSize())) {
    NSRange range = NSMakeRange(0, [[arrctlrCurStack arrangedObjects] count]);
    [arrctlrCurStack removeObjectsAtArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:range]];
    for (int i = 0; i < stack_a.GetSize() && i < 2; i++) { // capped at two for now
      OrgExecStateValue* sv = [[OrgExecStateValue alloc] initWithPrefix:@""];
      [sv setValue:stack_a[i]];
      [arrctlrCurStack addObject:sv];
    }
  } else {
    for (int i = 0; i < stack_a.GetSize() && i < 2; i++) // capped at two for now
      [(OrgExecStateValue*)[arrCurStack objectAtIndex:i] setValue:stack_a[i]];
  }

  // handle stack b
  const Apto::Array<int>& stack_b = snapshot.Buffer("stack B");
  if ([arrStackB count] != ((stack_b.GetSize() > 2) ? 2 : stack_b.GetSize())) {
    NSRange range = NSMakeRange(0, [[arrctlrStackB arrangedObjects] count]);
    [arrctlrStackB removeObjectsAtArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:range]];
    for (int i = 0; i < stack_b.GetSize() && i < 2; i++) { // capped at two for now
      OrgExecStateValue* sv = [[OrgExecStateValue alloc] initWithPrefix:@""];
      [sv setValue:stack_b[i]];
      [arrctlrStackB addObject:sv];
    }
  } else {
    for (int i = 0; i < stack_b.GetSize() && i < 2; i++) // capped at two for now
      [(OrgExecStateValue*)[arrStackB objectAtIndex:i] setValue:stack_b[i]];
  }

}


- (void) startAnimation {
  if (tmrAnim == nil) {
    tmrAnim = [NSTimer scheduledTimerWithTimeInterval:0.075 target:self selector:@selector(nextAnimationFrame:) userInfo:self repeats:YES];
    [btnGo setTitle:@"Stop"];
  }
}

- (void) stopAnimation {
  if (tmrAnim != nil) {
    [tmrAnim invalidate];
    tmrAnim = nil;
    [btnGo setTitle:@"Run"];
  }
}

- (void) nextAnimationFrame:(id)sender {
  [self setSnapshot:(curSnapshotIndex + 1)];
}


- (void) createSettingsPopover {
  if (popoverSettings == nil) {
    // create and setup our popover
    popoverSettings = [[NSPopover alloc] init];
    
    // the popover retains us and we retain the popover, we drop the popover whenever it is closed to avoid a cycle
    popoverSettings.contentViewController = ctlrSettings;    
    popoverSettings.appearance = NSPopoverAppearanceHUD;  
    popoverSettings.animates = YES;
    
    // AppKit will close the popover when the user interacts with a user interface element outside the popover.
    // note that interacting with menus or panels that become key only when needed will not cause a transient popover to close.
    popoverSettings.behavior = NSPopoverBehaviorTransient;
    
    // so we can be notified when the popover appears or closes
    popoverSettings.delegate = self;
  }
}

- (NSString*) descriptionOfInst:(Avida::Instruction)inst {
  switch (inst.GetOp()) {
    case 0: return @"a: nop-A is a no-operation instruction, and will not do anything when executed. It can, however, modify the behavior of the instruction preceding it (by changing the CPU component that it affects; see also nop-register notation and nop-head notation) or act as part of a template to denote positions in the genome.";
    case 1: return @"b: nop-B is a no-operation instruction, and will not do anything when executed. It can, however, modify the behavior of the instruction preceding it (by changing the CPU component that it affects; see also nop-register notation and nop-head notation) or act as part of a template to denote positions in the genome.";
    case 2: return @"c: nop-C is a no-operation instruction, and will not do anything when executed. It can, however, modify the behavior of the instruction preceding it (by changing the CPU component that it affects; see also nop-register notation and nop-head notation) or act as part of a template to denote positions in the genome.";
    case 3: return @"d: if-n-equ: This instruction compares the BX register to its complement. If they are not equal, the next instruction (after a modifying no-operation instruction, if one is present) is executed. If they are equal, that next instruction is skipped.";
    case 4: return @"e: if-less: This instruction compares the BX register to its complement. If BX is the lesser of the pair, the next instruction (after a modifying no-operation instruction, if one is present) is executed. If it is greater or equal, then that next instruction is skipped.";
    case 5: return @"f: if-label: This instruction reads in the template that follows it, and tests if its complement template was the most recent series of instructions copied. If so, it executed the next instruction, otherwise it skips it. This instruction is commonly used for an organism to determine when it has finished producing its offspring.";
    case 6: return @"g: mov-head: This instruction will cause the IP to jump to the position in memory of the flow-head.";
    case 7: return @"h: jmp-head: This instruction will read in the value of the CX register, and the move the IP by that fixed amount through the organism's memory.";
    case 8: return @"i: get-head: This instruction will copy the position of the IP into the CX register.";
    case 9: return @"j: set-flow: This instruction moves the flow-head to the memory position denoted in the CX register.";
    case 10: return @"k: shift-r: This instruction reads in the contents of the BX register, and shifts all of the bits in that register to the right by one. In effect, it divides the value stored in the register by two, rounding down.";
    case 11: return @"l: shift-l: This instruction reads in the contents of the BX register, and shifts all of the bits in that register to the left by one, placing a zero as the new rightmost bit, and truncating any bits beyond the 32 maximum. For values that require fewer than 32 bits, it effectively multiplies that value by two.";
    case 12: return @"m: inc: This instruction reads in the content of the BX register and increments it by one.";
    case 13: return @"n: dec: This instruction reads in the content of the BX register and decrements it by one.";
    case 14: return @"o: pop: This instruction removes the top element from the active stack, and places it into the BX register.";
    case 15: return @"p: push: This instruction reads in the contents of the BX register, and places it as a new entry at the top of the active stack. The BX register itself remains unchanged.";
    case 16: return @"q: swap-stk: This instruction toggles the active stack in the CPU. All other instructions that use a stack will always use the active one.";
    case 17: return @"r: swap: This instruction swaps the contents of the BX register with its complement.";
    case 18: return @"s: add: This instruction reads in the contents of the BX and CX registers and sums them together. The result of this operation is then placed in the BX register.";
    case 19: return @"t: sub: This instruction reads in the contents of the BX and CX registers and subtracts CX from BX (respectively). The result of this operation is then placed in the BX register.";
    case 20: return @"u: nand: This instruction reads in the contents of the BX and CX registers (each of which are 32-bit numbers) and performs a bitwise nand operation on them. The result of this operation is placed in the BX register. Note that this is the only logic operation provided in the basic avida instruction set.";
    case 21: return @"v: h-copy: This instruction reads the contents of the organism's memory at the position of the read-head, and copy that to the position of the write-head. If a non-zero copy mutation rate is set, a test will be made based on this probability to determine if a mutation occurs. If so, a random instruction (chosen from the full set with equal probability) will be placed at the write-head instead.";
    case 22: return @"w: h-alloc: This instruction allocates additional memory for the organism up to the maximum it is allowed to use for its offspring.";
    case 23: return @"x: h-divide: This instruction is used for an organism to divide off a finished offspring. The original organism keeps the state of its memory up until the read-head. The offspring's memory is initialized to everything between the read-head and the write-head. All memory past the write-head is removed entirely.";
    case 24: return @"y: IO: This is the input/output instruction. It takes the contents of the BX register and outputs it, checking it for any tasks that may have been performed. It will then place a new input into BX.";
    case 25: return @"z: h-search: This instruction will read in the template the follows it, and find the location of a complement template in the code. The BX register will be set to the distance to the complement from the current position of the instruction-pointer, and the CX register will be set to the size of the template. The flow-head will also be placed at the beginning of the complement template. If no template follows, both BX and CX will be set to zero, and the flow-head will be placed on the instruction immediately following the h-search.";
      
    default:
      return @"(unknown)";
  }
}


- (void) drawOrgWithTimelineInRect:(NSRect)rect inContext:(NSGraphicsContext*)gc {
  NSRect timelineRect = rect;
  timelineRect.size.height = timelineView.bounds.size.height;
  [timelineView displayRectIgnoringOpacity:timelineRect inContext:gc];
  
  NSRect orgRect = rect;
  orgRect.size.height -= timelineRect.size.height;
  
  // Set current context to the supplied graphics context
  NSGraphicsContext* currentContext = [NSGraphicsContext currentContext];
  [NSGraphicsContext setCurrentContext:gc];
  
  // Translate X coordinate, drawn, and restore coordinates
  NSAffineTransform* transform = [NSAffineTransform transform];
  [transform translateXBy:0 yBy:timelineRect.size.height];
  [transform concat];
  [orgView displayRectIgnoringOpacity:orgRect inContext:gc];
  [transform invert];
  [transform concat];
  
  // Restore previous graphics context
  [NSGraphicsContext setCurrentContext:currentContext];
}

- (void) splitViewAnimationEnd:(NSNumber*)collapsed {
  splitViewIsAnimating = NO;
  if ([collapsed boolValue]) {
    [statView setHidden:YES];
  }
}



@end


@implementation AvidaEDOrganismViewController

@synthesize arrRegisters;
@synthesize arrInputBuffer;
@synthesize arrOutputBuffer;
@synthesize arrCurStack;
@synthesize arrStackB;


- (id) initWithWorld:(AvidaRun*)world {
  self = [super initWithNibName:@"AvidaED-OrganismView" bundle:nil];
  if (self) {
    tmrAnim = nil;
    testWorld = world;
  }
  return self;
}

- (void) loadView {
  [super loadView];
  [self viewDidLoad];
}


- (void) viewDidLoad {
  
  orgView.dropDelegate = dropDelegate;
  
  [btnBegin setEnabled:NO];
  [btnBack setEnabled:NO];
  [btnGo setEnabled:NO];
  [btnGo setTitle:@"Run"];
  [btnForward setEnabled:NO];
  [btnEnd setEnabled:NO];
  
  [sldStatus setEnabled:NO];
  [txtOrgName setStringValue:@"(none)"];
  [txtOrgName setEnabled:NO];

  [[[txtJustExec textStorage] mutableString] setString:@"(none)"];
  [[[txtWillExec textStorage] mutableString] setString:@"(none)"];

  [orgView registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, nil]];
  orgView.layer.anchorPoint = CGPointMake(0,0);
  
  envActions = [[AvidaEDEnvActionsDataSource alloc] init];
  
  Avida::Environment::ManagerPtr env = Avida::Environment::Manager::Of([testWorld world]);
  Avida::Environment::ConstActionTriggerIDSetPtr trigger_ids = env->GetActionTriggerIDs();
  for (Avida::Environment::ConstActionTriggerIDSetIterator it = trigger_ids->Begin(); it.Next();) {
    Avida::Environment::ConstActionTriggerPtr action = env->GetActionTrigger(*it.Get());
    NSString* entryName = [NSString stringWithAptoString:action->GetID()];
    if (action->GetID() == "or") {
      entryName = @"oro";
    } else if (action->GetID() == "andn") {
      entryName = @"ant";
    } else if (action->GetID() == "nand") {
      entryName = @"nan";
    }
    NSString* entryDesc = [NSString stringWithAptoString:action->GetDescription()];
    [envActions addNewEntry:entryName withDescription:entryDesc withOrder:action->TempOrdering()];
  }
  
  [tblTaskCounts setDataSource:envActions];
  [tblTaskCounts reloadData];
}


- (void) setDropDelegate:(id<DropDelegate>)delegate {
  dropDelegate = delegate;
  if (orgView) [orgView setDropDelegate:delegate];
}




- (IBAction) selectSnapshot:(id)sender {
  [self stopAnimation];
  int snapshot = [sldStatus intValue];
  [self setSnapshot:snapshot];
}


- (IBAction) nextSnapshot:(id)sender {
  [self stopAnimation];
  int snapshot = [sldStatus intValue] + 1;
  if (snapshot >= trace->SnapshotCount()) snapshot = trace->SnapshotCount() - 1;
  [self setSnapshot:snapshot];
}

- (IBAction) prevSnapshot:(id)sender {
  [self stopAnimation];
  int snapshot = [sldStatus intValue] - 1;
  if (snapshot < 0) snapshot = 0;
  [self setSnapshot:snapshot];
}

- (IBAction) firstSnapshot:(id)sender {
  Avida::GenomePtr genome(new Avida::Genome(*trace->OrganismGenome()));
  [self setGenome:genome withName:[self getOrganismName]];
}

- (IBAction) lastSnapshot:(id)sender {
  [self stopAnimation];
  [self setSnapshot:(trace->SnapshotCount() - 1)];
}


- (IBAction) toggleAnimation:(id)sender {
  if (tmrAnim == nil) {
    [self startAnimation];
  } else {
    [self stopAnimation];
  }
}

- (IBAction) showSettings:(id)sender {
  NSButton* targetButton = (NSButton*)sender;
  
  [self createSettingsPopover];
  
  [popoverSettings showRelativeToRect:[targetButton bounds] ofView:sender preferredEdge:NSMaxYEdge];
}


- (IBAction) toggleInstInfo:(id)sender {
  if (btnToggleInstInfo.state == NSOffState) {
    CABasicAnimation *animation = [CABasicAnimation animation];
    animation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
    
    CAAnimationBlockDelegate* delegate = [[CAAnimationBlockDelegate alloc] init];
    delegate.blockOnAnimationSucceeded = ^() {
      [viewInstInfo removeFromSuperview];
    };
    animation.delegate = delegate;    
    
    [viewInstInfoContainer setAnimations:[NSDictionary dictionaryWithObject:animation forKey:@"alphaValue"]];
    [viewInstInfoContainer.animator setAlphaValue:0.0f];
  } else {
    [viewInstInfoContainer addSubview:viewInstInfo];
    [viewInstInfo setTranslatesAutoresizingMaskIntoConstraints:NO];
    [viewInstInfoContainer addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[viewInstInfo]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(viewInstInfo)]];
    [viewInstInfoContainer addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[viewInstInfo]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(viewInstInfo)]];
    
    CABasicAnimation *animation = [CABasicAnimation animation];
    animation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];

    [viewInstInfoContainer setAnimations:[NSDictionary dictionaryWithObject:animation forKey:@"alphaValue"]];
    [viewInstInfoContainer.animator setAlphaValue:1.0f];
  }
}


- (IBAction) changeMutRate:(id)sender {
//  // mut rate has changed, reanalyze current genome (if appropriate)
//  if (trace) {
//    Avida::GenomePtr genome(new Avida::Genome(*trace->OrganismGenome()));
//    [self setGenome:genome withName:[self getOrganismName]];
//  }
}


- (void) draggableImageView:(DraggableImageView*)imageView writeToPasteboard:(NSPasteboard*)pboard {
  
  if (!trace) return;
  
  ACGenome* genome = nil;
  
  if (imageView == imgOffspring) {
    genome = [[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:trace->OffspringGenome()->AsString()]
                                       name:[NSString stringWithFormat:@"%@ offspring",[txtOrgName stringValue]]];
  } else {
    genome = [[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:trace->OrganismGenome()->AsString()]
                                       name:[txtOrgName stringValue]];
  }
  
  [pboard writeObjects:[[NSArray alloc] initWithObjects:genome, nil]];
}




- (void) setGenome:(Avida::GenomePtr)genome withName:(NSString*)name {
  
  [self stopAnimation];

  // Trace genome
  trace = Avida::Viewer::OrganismTracePtr(new Avida::Viewer::OrganismTrace([testWorld oldworld], genome, ctlrSettings.mutRate, ctlrSettings.randomSeed));
  
  [txtOrgName setStringValue:name];
  [txtOrgName setEnabled:YES];
  
  if (trace->SnapshotCount() > 0) {
    [sldStatus setMinValue:0];
    [sldStatus setMaxValue:trace->SnapshotCount() - 1];
    [sldStatus setIntValue:0];
    [sldStatus setEnabled:YES];
    
    // update timeline view
    [timelineView clearEntries];
    [timelineView setLength:trace->SnapshotCount() - 1];
    
    NSMutableSet* nameset = [[NSMutableSet alloc] initWithCapacity:[envActions entryCount]];
    for (NSUInteger i = 0; i < [envActions entryCount]; i++) {
      [nameset addObject:[envActions entryAtIndex:i]];
    }
    
    for (int i = 0; i < trace->SnapshotCount(); i++) {
      NSMutableSet* foundset = [[NSMutableSet alloc] init];
      for (NSString* entry_name in nameset) {
        NSString* avida_name = entry_name;
        if ([entry_name isEqual:@"oro"]) {
          avida_name = @"or";
        } else if ([entry_name isEqual:@"ant"]) {
          avida_name = @"andn";
        } else if ([entry_name isEqual:@"nan"]) {
          avida_name = @"nand";
        }
        if (trace->Snapshot(i).FunctionCount([avida_name UTF8String]) > 0) {
          [timelineView addEntryWithLabel:[[envActions valueOfEntry:entry_name forKey:@"Order"] stringValue] atLocation:i];
          [foundset addObject:entry_name];
        }
      }
      
      // Remove found entries from the search list
      for (NSString* entry_name in foundset) [nameset removeObject:entry_name];
      
      // If nothing left to search for, then quit
      if ([nameset count] == 0) break;
    }
    
    
    [btnBegin setEnabled:YES];
    [btnGo setEnabled:YES];
    [btnGo setTitle:@"Run"];
    
    [self setSnapshot:0];
  } else {
    [sldStatus setIntValue:0];
    [sldStatus setEnabled:NO];
    
    [timelineView setLength:0];
    [timelineView setCurrentPoint:-1];
    
    [btnBegin setEnabled:NO];
    [btnBack setEnabled:NO];
    [btnGo setEnabled:NO];
    [btnGo setTitle:@"Run"];
    [btnForward setEnabled:NO];
    [btnEnd setEnabled:NO];

    [[[txtJustExec textStorage] mutableString] setString:@"(none)"];
    [[[txtWillExec textStorage] mutableString] setString:@"(none)"];

    [orgView setSnapshot:NULL];
  }
}


- (NSString*) getOrganismName {
  return [txtOrgName stringValue];
}

- (void) exportGraphic:(ExportGraphicsFileFormat)format withOptions:(NSMatrix*)optMat toURL:(NSURL*)url {
  
  unsigned int selected = 0;
  if ([[optMat cellWithTag:0] state] == NSOnState) selected |= 0x1;
  if ([[optMat cellWithTag:1] state] == NSOnState) selected |= 0x2;
  if ([[optMat cellWithTag:2] state] == NSOnState) selected |= 0x7;
  
  if (selected == 0) selected = 0x7;
  
  NSRect exportRect;
  switch (selected) {
    case 0x1: // Org View
      exportRect = orgView.bounds;
      break;
    case 0x2: // Timeline View
      exportRect = timelineView.bounds;
      break;
    case 0x3: // Both Org and Timeline
      exportRect = orgView.bounds;
      exportRect.size.height += timelineView.bounds.size.height;
      break;
    case 0x7: // Full View
      exportRect = fullView.bounds;
      break;
  }
  
  switch (format) {
    case EXPORT_GRAPHICS_JPEG:
    case EXPORT_GRAPHICS_PNG:
    {
      // Create imgContext
      int bitmapBytesPerRow = 4 * exportRect.size.width;
      CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
      CGContextRef imgContext = CGBitmapContextCreate(NULL, exportRect.size.width, exportRect.size.height, 8, bitmapBytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);
      CGColorSpaceRelease(colorSpace);
      
      // Draw view into graphics imgContext
      NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:imgContext flipped:NO];
      switch (selected) {
        case 0x1:
          [orgView displayRectIgnoringOpacity:exportRect inContext:gc];
          break;
        case 0x2:
          [timelineView displayRectIgnoringOpacity:exportRect inContext:gc];
          break;
        case 0x3:
          [self drawOrgWithTimelineInRect:exportRect inContext:gc];
          break;
        case 0x7:
          [fullView displayRectIgnoringOpacity:exportRect inContext:gc];
          break;
      }
      
      // Create image ref to imgContext
      CGImageRef imgRef = CGBitmapContextCreateImage(imgContext);
      
      
      // Write the appropriate file type
      if (format == EXPORT_GRAPHICS_JPEG) {
        CFMutableDictionaryRef mSaveMetaAndOpts = CFDictionaryCreateMutable(nil, 0, &kCFTypeDictionaryKeyCallBacks,  &kCFTypeDictionaryValueCallBacks);
        NSNumber* compQual = [NSNumber numberWithFloat:1.0];
        CFDictionarySetValue(mSaveMetaAndOpts, kCGImageDestinationLossyCompressionQuality, (__bridge void*)compQual);	// set the compression quality here
        CGImageDestinationRef dr = CGImageDestinationCreateWithURL ((__bridge CFURLRef)url, (CFStringRef)@"public.jpeg" , 1, NULL);
        CGImageDestinationAddImage(dr, imgRef, mSaveMetaAndOpts);
        CGImageDestinationFinalize(dr);
      } else {
        CGImageDestinationRef dr = CGImageDestinationCreateWithURL ((__bridge CFURLRef)url, (CFStringRef)@"public.png" , 1, NULL);
        CGImageDestinationAddImage(dr, imgRef, NULL);
        CGImageDestinationFinalize(dr);
      }
      
      // Clean up
      CFRelease(imgRef);
      CFRelease(imgContext);
    }
      break;
    case EXPORT_GRAPHICS_PDF:
    {
      // Create pdfContext
      NSMutableData *pdfData = [[NSMutableData alloc] init];
      CGDataConsumerRef dataConsumer = CGDataConsumerCreateWithCFData((__bridge CFMutableDataRef)pdfData);
      CGContextRef pdfContext = CGPDFContextCreate(dataConsumer, &exportRect, NULL);
      CGContextBeginPage(pdfContext, &exportRect);
      
      NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:pdfContext flipped:NO];
      switch (selected) {
        case 0x1:
          [orgView displayRectIgnoringOpacity:exportRect inContext:gc];
          break;
        case 0x2:
          [timelineView displayRectIgnoringOpacity:exportRect inContext:gc];
          break;
        case 0x3:
          [self drawOrgWithTimelineInRect:exportRect inContext:gc];
          break;
        case 0x7:
          [fullView displayRectIgnoringOpacity:exportRect inContext:gc];
          break;
      }
      
      // Close up pdfContext
      CGContextEndPage(pdfContext);
      CGPDFContextClose(pdfContext);
      
      
      // Write pdfContext to file
      [pdfData writeToURL:url atomically:NO];
      
      
      // Clean up
      CGContextRelease(pdfContext);
      CGDataConsumerRelease(dataConsumer);
    }
      break;
    default:
      break;
  }
}




- (void)popoverWillShow:(NSNotification *)notification
{
//  NSPopover* popover = [notification object];
  // add new code here when the popover will be shown
}


- (void)popoverDidShow:(NSNotification *)notification
{
  // add new code here after the popover has been shown
}


- (void)popoverWillClose:(NSNotification *)notification
{
  NSString *closeReason = [[notification userInfo] valueForKey:NSPopoverCloseReasonKey];
  if (closeReason)
  {
    // closeReason can be:
    //      NSPopoverCloseReasonStandard
    //      NSPopoverCloseReasonDetachToWindow
    //
    // add new code here if you want to respond "before" the popover closes
    //
  }
}

- (void)popoverDidClose:(NSNotification *)notification
{
  NSString *closeReason = [[notification userInfo] valueForKey:NSPopoverCloseReasonKey];
  if (closeReason == NSPopoverCloseReasonStandard)
  {
    // mut rate has changed, reanalyze current genome (if appropriate)
    if (trace) {
      Avida::GenomePtr genome(new Avida::Genome(*trace->OrganismGenome()));
      [self setGenome:genome withName:[self getOrganismName]];
    }    
  }
  
  popoverSettings = nil;
}



- (IBAction) toggleStatView:(id)sender {
  
  if (splitViewIsAnimating) return;
  
  if ([sender state] == NSOnState) {
    // uncollapse
    [statSplitView setDividerStyle:NSSplitViewDividerStyleThin];
    CGFloat dividerThickness = [statSplitView dividerThickness];
    
    NSRect oldStatViewFrame = statView.frame;
    oldStatViewFrame.size.width = 0;
    oldStatViewFrame.origin.x = statSplitView.frame.size.width;
    [statView setFrame:oldStatViewFrame];
    [statView setHidden:NO];
    
    NSMutableDictionary *shrinkMainViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [shrinkMainViewAnimationDict setObject:mainView forKey:NSViewAnimationTargetKey];
    NSRect newMainViewFrame = mainView.frame;
    newMainViewFrame.size.width =  statSplitView.frame.size.width - SPLIT_RIGHT_MIN - dividerThickness;
    [shrinkMainViewAnimationDict setObject:[NSValue valueWithRect:newMainViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSMutableDictionary *expandStatViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [expandStatViewAnimationDict setObject:statView forKey:NSViewAnimationTargetKey];
    NSRect newStatViewFrame = statView.frame;
    newStatViewFrame.size.width = SPLIT_RIGHT_MIN;
    newStatViewFrame.origin.x = statSplitView.frame.size.width - SPLIT_RIGHT_MIN;
    [expandStatViewAnimationDict setObject:[NSValue valueWithRect:newStatViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSViewAnimation *expandAnimation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects:shrinkMainViewAnimationDict, expandStatViewAnimationDict, nil]];
    [expandAnimation setDuration:0.25f];
    [expandAnimation startAnimation];
    [self performSelector:@selector(splitViewAnimationEnd:) withObject:[NSNumber numberWithBool:NO] afterDelay:0.25f];
  } else {
    // collapse
    // Store last width so we can jump back
    
    [statSplitView setDividerStyle:NSSplitViewDividerStylePaneSplitter];
    CGFloat dividerThickness = [statSplitView dividerThickness];
    
    NSMutableDictionary *expandMainAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [expandMainAnimationDict setObject:mainView forKey:NSViewAnimationTargetKey];
    NSRect newMainViewFrame = mainView.frame;
    newMainViewFrame.size.width =  statSplitView.frame.size.width - dividerThickness;
    [expandMainAnimationDict setObject:[NSValue valueWithRect:newMainViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSMutableDictionary *collapseStatViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [collapseStatViewAnimationDict setObject:statView forKey:NSViewAnimationTargetKey];
    NSRect newStatViewFrame = statView.frame;
    //    newPopViewStatViewFrame.size.width = 0.0f;
    newStatViewFrame.origin.x = statView.frame.size.width;
    [collapseStatViewAnimationDict setObject:[NSValue valueWithRect:newStatViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSViewAnimation *collapseAnimation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects:expandMainAnimationDict, collapseStatViewAnimationDict, nil]];
    [collapseAnimation setDuration:0.25f];
    [collapseAnimation startAnimation];
    [self performSelector:@selector(splitViewAnimationEnd:) withObject:[NSNumber numberWithBool:YES] afterDelay:0.25f];
  }
  splitViewIsAnimating = YES;
}


- (void) splitView:(NSSplitView*)splitView resizeSubviewsWithOldSize:(NSSize)oldSize {

  NSView* leftView = [[splitView subviews] objectAtIndex:0];
  NSView* rightView = [[splitView subviews] objectAtIndex:1];
  NSRect newFrame = [splitView frame];
  NSRect leftFrame = [leftView frame];
  NSRect rightFrame = [rightView frame];
  
  CGFloat dividerThickness = [splitView dividerThickness];
  
  if ([splitView isSubviewCollapsed:statView]) {
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    leftFrame.size.width = newFrame.size.width - dividerThickness;
  } else {
    if (rightFrame.size.width != SPLIT_RIGHT_MIN && !splitViewIsAnimating) {
      leftFrame.size.width = newFrame.size.width - dividerThickness - SPLIT_RIGHT_MIN;
      rightFrame.size.width = SPLIT_RIGHT_MIN;
    } else {
      CGFloat diffWidth = floor(newFrame.size.width - oldSize.width);
      leftFrame.size.width += floor(diffWidth / 2);
      rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    }
    
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    rightFrame.size.height = newFrame.size.height;
    rightFrame.origin.x = leftFrame.size.width + dividerThickness;
  }
  
  [leftView setFrame:leftFrame];
  [rightView setFrame:rightFrame];
  [splitView adjustSubviews];
}


- (BOOL) splitView:(NSSplitView*)splitView canCollapseSubview:(NSView*)subview {
  if (subview == statView) return YES;
  
  return NO;
}


- (CGFloat) splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMax ofSubviewAt:(NSInteger)index {
  return proposedMax - SPLIT_RIGHT_MIN;
}


- (CGFloat) splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMin ofSubviewAt:(NSInteger)index {
  return proposedMin + SPLIT_LEFT_MIN;  
}

- (void) splitViewDidResizeSubviews:(NSNotification*)notification {
  if (!splitViewIsAnimating) {
    if ([statSplitView isSubviewCollapsed:statView]) {
      [btnToggleStatView setState:NSOffState];
      [statSplitView setDividerStyle:NSSplitViewDividerStylePaneSplitter];
    } else {
      [btnToggleStatView setState:NSOnState];
      [statSplitView setDividerStyle:NSSplitViewDividerStyleThin];
    }
    [statSplitView adjustSubviews];
  }
}

@end
