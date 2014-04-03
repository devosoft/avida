//
//  AvidaEDController.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 11/30/10.
//  Copyright 2010-2012 Michigan State University. All rights reserved.
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

#import "AvidaEDController.h"

#import <AvidaCore/AvidaCore.h>


#import "AvidaEDAppDelegate.h"
#import "AvidaRun.h"
#import "CenteringClipView.h"
#import "FlipView.h"
#import "Freezer.h"
#import "MapGridView.h"
#import "MapScaleView.h"
#import "NSFileManager+TemporaryDirectory.h"

#import "AvidaEDAnalyzeViewController.h"
#import "AvidaEDExportAccessoryController.h"
#import "AvidaEDPopViewStatView.h"
#import "AvidaEDOrganismViewController.h"

#include "avida/viewer/Map.h"

static const float MAIN_SPLIT_LEFT_MIN = 140.0;
static const float MAIN_SPLIT_RIGHT_MIN = 710.0;
static const float POP_SPLIT_LEFT_MIN = 350.0;
static const float POP_SPLIT_RIGHT_MIN = 360.0;
static const float POP_SPLIT_LEFT_PROPORTIONAL_RESIZE = 0.3;


NSString* const AvidaPasteboardTypePopulation = @"org.devosoft.avida.population";

@interface Population : NSObject <NSCoding, NSPasteboardWriting, NSPasteboardReading> {
}
@end;


@implementation Population

- (void) encodeWithCoder:(NSCoder*)encoder {
}

- (id) initWithCoder:(NSCoder*)decoder {
  return self;
}


- (NSArray*) writableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* writableTypes = nil;
  if (!writableTypes) {
    writableTypes = [[NSArray alloc] initWithObjects:AvidaPasteboardTypePopulation, nil];
  }
  return writableTypes;
}

- (id) pasteboardPropertyListForType:(NSString*)type {
  if ([type isEqualToString:AvidaPasteboardTypeFreezerID]) {
    return [NSKeyedArchiver archivedDataWithRootObject:self];
  }
  return nil;
}


+ (NSArray*) readableTypesForPasteboard:(NSPasteboard*)pboard {
  static NSArray* readableTypes = nil;
  if (!readableTypes) {
    readableTypes = [[NSArray alloc] initWithObjects:AvidaPasteboardTypePopulation, nil];
  }
  return readableTypes;
}

+ (NSPasteboardReadingOptions)readingOptionsForType:(NSString *)type pasteboard:(NSPasteboard *)pboard {
  if ([type isEqualToString:AvidaPasteboardTypePopulation]) {
    return NSPasteboardReadingAsKeyedArchive;
  }
  return 0;
}
@end








static NSInteger sortFreezerItems(id f1, id f2, void* context)
{
  Avida::Viewer::FreezerPtr freezer = *(Avida::Viewer::FreezerPtr*)context;
  Apto::String f1s = freezer->NameOf([f1 freezerID]);
  Apto::String f2s = freezer->NameOf([f2 freezerID]);
  
  if (f1s < f2s) return NSOrderedAscending;
  if (f1s > f2s) return NSOrderedDescending;
  return NSOrderedSame;
}




@interface AvidaEDController (hidden)
- (void) setup;

- (void) popSplitViewAnimationEnd:(NSNumber*)collapsed;
- (void) setupFreezer;
- (void) loadRunFromFreezer:(Avida::Viewer::FreezerID)freezerID;
- (void) loadRunFromFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo;
- (void) saveRunToFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo;
- (void) saveAnyToFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo;
- (void) nonDefaultFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo;
- (void) clearCurrentRun;
- (void) freezeCurrentConfig;
- (void) freezeCurrentRun;
- (void) freezeGenome:(ACGenome*)genome;
- (void) removeFromFreezer:(Avida::Viewer::FreezerID)freezerID;
- (void) saveAncestorsToFreezerID:(Avida::Viewer::FreezerID)freezerID;
- (void) activateRun;
- (void) activateRunWithID:(Avida::Viewer::FreezerID)freezerID;
- (NSPoint) locationOfOrg:(NSUInteger)orgIndex withOrgCount:(NSUInteger)count;
- (void) updatePendingInjectColors;

- (void) setInterfacePaused;
- (void) setInterfaceRunning;

- (void) drawMapWithScaleInRect:(NSRect)rect inContext:(NSGraphicsContext*)gc;

- (NSString*) uniqueNameForAncestorWithName:(NSString*)genome_name;

- (BOOL) createNonDefaultFreezer;
@end

@implementation AvidaEDController (hidden)

- (void) setup {
  currentRun = nil;
  listener = NULL;
  map = NULL;
  popSplitViewIsAnimating = NO;  
  ancestorArray = [[NSMutableArray alloc] init];
  autoAncestorArray = [[NSMutableArray alloc] init];
  manualAncestorArray = [[NSMutableArray alloc] init];
  sheetActive = NO;
}



- (void) popSplitViewAnimationEnd:(NSNumber*)collapsed {
  popSplitViewIsAnimating = NO;
  if ([collapsed boolValue]) {
    [popViewStatView setHidden:YES];
  }
}

- (void) setupFreezer {
  freezerConfigs = [[NSMutableArray alloc] initWithCapacity:freezer->NumEntriesOfType(Avida::Viewer::CONFIG)];
  for (Avida::Viewer::Freezer::Iterator it = freezer->EntriesOfType(Avida::Viewer::CONFIG); it.Next();) {
    [freezerConfigs addObject:[[FreezerItem alloc] initWithFreezerID:*it.Get()]];
    [freezerConfigs sortUsingFunction:&sortFreezerItems context:&freezer];
  }
  
  freezerWorlds = [[NSMutableArray alloc] initWithCapacity:freezer->NumEntriesOfType(Avida::Viewer::WORLD)];
  for (Avida::Viewer::Freezer::Iterator it = freezer->EntriesOfType(Avida::Viewer::WORLD); it.Next();) {
    [freezerWorlds addObject:[[FreezerItem alloc] initWithFreezerID:*it.Get()]];
    [freezerWorlds sortUsingFunction:&sortFreezerItems context:&freezer];
  }

  freezerGenomes = [[NSMutableArray alloc] initWithCapacity:freezer->NumEntriesOfType(Avida::Viewer::GENOME)];
  for (Avida::Viewer::Freezer::Iterator it = freezer->EntriesOfType(Avida::Viewer::GENOME); it.Next();) {
    [freezerGenomes addObject:[[FreezerItem alloc] initWithFreezerID:*it.Get()]];
    [freezerGenomes sortUsingFunction:&sortFreezerItems context:&freezer];
  }  
}

- (void) loadRunFromFreezer:(Avida::Viewer::FreezerID)freezerID {
  if (!freezer->IsValid(freezerID)) return;
  
  // clean up old run
  if (currentRun != nil) {
    if (runActive == YES) {
      // Offer to freeze current run...
      NSAlert* alert = [[NSAlert alloc] init];
      [alert addButtonWithTitle:@"Save"];
      [alert addButtonWithTitle:@"Discard"];
      [alert addButtonWithTitle:@"Cancel"];
      [alert setMessageText:@"The petri dish of the current experiment has not been saved in the freezer."];
      [alert setInformativeText:@"Would you like to save or discard the current petri dish before starting a new experiment?"];
      [alert setAlertStyle:NSWarningAlertStyle];
      void* contextInfo = new Avida::Viewer::FreezerID(freezerID);
      sheetActive = YES;
      [alert beginSheetModalForWindow:[self window] modalDelegate:self didEndSelector:@selector(loadRunFromFreezerAlertDidEnd:returnCode:contextInfo:) contextInfo:contextInfo];
      return;
    }
    
    [self clearCurrentRun];
  }
  
  runActive = NO;
  [matCfgRepeatability setEnabled:YES];
  [txtCfgWorldX setEnabled:YES];
  [txtCfgWorldY setEnabled:YES];

  
  // create working directory
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSString* runPath = [fileManager createTemporaryDirectory];
  
  // instantiate config
  freezer->InstantiateWorkingDir(freezerID, [runPath cStringUsingEncoding:NSASCIIStringEncoding]);
  
  // create run object
  if (freezerID.type == Avida::Viewer::CONFIG) {
    currentRun = [[AvidaRun alloc] initWithDirectory:runPath];
    [txtUpdate setStringValue:@"(not started)"];
    [txtUpdate setTextColor:[NSColor disabledControlTextColor]];
  } else {
    currentRun = [[AvidaRun alloc] initWithDirectory:runPath];
    [self activateRunWithID:freezerID];
    [txtUpdate setStringValue:[NSString stringWithFormat:@"%d updates", [currentRun currentUpdate]]];
    [currentRun pauseAt:[currentRun currentUpdate] + 1];
    [currentRun resume];
  }

  [mapView setDimensions:[currentRun worldSize]];
  [mapZoom setDoubleValue:[mapView zoom]];

  // update interface
  [txtRun setStringValue:[NSString stringWithAptoString:freezer->NameOf(freezerID)]];
  [self setInterfacePaused];
  
  double mutrate = [currentRun mutationRate];
  [sldCfgMutRate setFloatValue:(mutrate == 0) ? [sldCfgMutRate minValue] : log10(mutrate)];
  [txtCfgMutRate setFloatValue:mutrate];
  NSSize worldsize = [currentRun worldSize];
  [txtCfgWorldX setIntValue:worldsize.width];
  [txtCfgWorldY setIntValue:worldsize.height];
  [matCfgPlacement selectCellWithTag:([currentRun placementMode] == 0) ? 0 : 1];
  [matCfgRepeatability selectCellWithTag:([currentRun randomSeed] == -1) ? 0 : 1];
  [matCfgPauseAt selectCellWithTag:0];
  [[matCfgEnv cellWithTag:0] setState:([currentRun reactionValueOf:"NOT"]  > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:1] setState:([currentRun reactionValueOf:"NAND"] > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:2] setState:([currentRun reactionValueOf:"AND"]  > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:3] setState:([currentRun reactionValueOf:"ORN"]  > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:4] setState:([currentRun reactionValueOf:"OR"]   > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:5] setState:([currentRun reactionValueOf:"ANDN"] > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:6] setState:([currentRun reactionValueOf:"NOR"]  > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:7] setState:([currentRun reactionValueOf:"XOR"]  > 0.0) ? NSOnState : NSOffState];
  [[matCfgEnv cellWithTag:8] setState:([currentRun reactionValueOf:"EQU"]  > 0.0) ? NSOnState : NSOffState];

  // Clear ancestors
  NSRange range = NSMakeRange(0, [[ancestorArrayCtlr arrangedObjects] count]);
  [ancestorArrayCtlr removeObjectsAtArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:range]];
  autoAncestorArray = [[NSMutableArray alloc] init];
  manualAncestorArray = [[NSMutableArray alloc] init];
  
  // Load ancestors
  Apto::String ancestor_str(freezer->LoadAttachment(freezerID, "ancestors"));
  Apto::String ancestor_name = ancestor_str.Pop('\n');
  while (ancestor_name.GetSize()) {
    Apto::String genome_str = ancestor_str.Pop('\n');
    ACGenome* genome = [[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:genome_str] name:[NSString stringWithAptoString:ancestor_name]];
    [ancestorArrayCtlr addObject:genome];
    [autoAncestorArray addObject:genome];
    ancestor_name = ancestor_str.Pop('\n');
  }

  ancestor_str = freezer->LoadAttachment(freezerID, "ancestors_manual");
  ancestor_name = ancestor_str.Pop('\n');
  while (ancestor_name.GetSize()) {
    Apto::String genome_str = ancestor_str.Pop('\n');
    Apto::String location = ancestor_str.Pop('\n');
    ACGenome* genome = [[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:genome_str] name:[NSString stringWithAptoString:ancestor_name]];
    [ancestorArrayCtlr addObject:genome];
    int x = Apto::StrAs(location.Pop(','));
    int y = Apto::StrAs(location);
    genome.location = NSMakePoint(x, y);
    [manualAncestorArray addObject:genome];
    ancestor_name = ancestor_str.Pop('\n');
  }

  if (freezerID.type == Avida::Viewer::CONFIG) [self updatePendingInjectColors];


  listener = new MainThreadListener(self);
  [currentRun attachListener:self];
}

- (void) loadRunFromFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo
{
  Avida::Viewer::FreezerID* freezerID = (Avida::Viewer::FreezerID*)contextInfo;
  
  switch (returnCode) {
    case NSAlertFirstButtonReturn:
      [self freezeCurrentRun];
      
    case NSAlertSecondButtonReturn:
      [self clearCurrentRun];
      [self loadRunFromFreezer:*freezerID];
      break;
      
    case NSAlertThirdButtonReturn:
    default:
      break;
  }
  
  delete freezerID;
}


- (void) saveRunToFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo
{
  switch (returnCode) {
    case NSAlertFirstButtonReturn:
      [self freezeCurrentRun];
      break;
      
    case NSAlertSecondButtonReturn:
      [self freezeCurrentConfig];
      break;
      
    case NSAlertThirdButtonReturn:
    default:
      break;
  }
  sheetActive = NO;
}

- (void) saveAnyToFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo
{
  switch (returnCode) {
    case NSAlertFirstButtonReturn:
      [self freezeCurrentRun];
      break;
      
    case NSAlertSecondButtonReturn:
      [self freezeGenome:[popViewStatView selectedOrgGenome]];
      break;
      
    case NSAlertThirdButtonReturn:
      [self freezeCurrentConfig];
      break;
      
    default:
      break;
  }
  sheetActive = NO;
}


- (void) clearCurrentRun {
  // Clear main listener
  [currentRun detachListener:self];
  delete listener;
  listener = NULL;
  
  // Clear map view
  [mapView clearMap];
  map = NULL;
  [mapScaleView setTempState:nil];

  // Clear stats panel
  [popViewStatView clearAvidaRun];
  
  // End run
  [currentRun end];
  currentRun = nil;
  curUpdate = -1;
}


- (void) freezeCurrentRun {
  [currentRun pause];
  
  // @TODO - fix this ugly busy wait
  while (![currentRun isPaused]);
  
  if (isDefaultFreezer && ![self createNonDefaultFreezer]) return;

  Apto::String name = freezer->NewUniqueNameForType(Avida::Viewer::WORLD, [[self runName] UTF8String]);
  Avida::Viewer::FreezerID f = freezer->SaveWorld([currentRun oldworld], name);
  if (freezer->IsValid(f)) {
    // Save ancestor info
    [self saveAncestorsToFreezerID:f];
    
    // Save plot info
    [popViewStatView saveRunToFreezer:freezer withID:f];
    
    FreezerItem* fi = [[FreezerItem alloc] initWithFreezerID:f];
    [freezerWorlds addObject:fi];
    [outlineFreezer reloadData];
    [outlineFreezer editColumn:0 row:[outlineFreezer rowForItem:fi] withEvent:nil select:YES];
  }
}

- (void) freezeCurrentConfig {
  if (isDefaultFreezer && ![self createNonDefaultFreezer]) return;

  Apto::String name = freezer->NewUniqueNameForType(Avida::Viewer::CONFIG, [[self runName] UTF8String]);
  Avida::Viewer::FreezerID f = freezer->SaveConfig([currentRun oldworld], name);
  if (freezer->IsValid(f)) {
    // Save ancestor info
    [self saveAncestorsToFreezerID:f];    
    
    FreezerItem* fi = [[FreezerItem alloc] initWithFreezerID:f];
    [freezerConfigs addObject:fi];
    [outlineFreezer reloadData];
    [outlineFreezer editColumn:0 row:[outlineFreezer rowForItem:fi] withEvent:nil select:YES];
  }
}

- (void) freezeGenome:(ACGenome*)genome {
  if (isDefaultFreezer && ![self createNonDefaultFreezer]) return;

  Apto::String name = freezer->NewUniqueNameForType(Avida::Viewer::GENOME, [[genome name] UTF8String]);
  Avida::GenomePtr genome_ptr(new Avida::Genome([[genome genomeStr] UTF8String]));
  Avida::Viewer::FreezerID f = freezer->SaveGenome(genome_ptr, name);
  if (freezer->IsValid(f)) {
    FreezerItem* fi = [[FreezerItem alloc] initWithFreezerID:f];
    [freezerGenomes addObject:fi];
    [outlineFreezer reloadData];
    [outlineFreezer editColumn:0 row:[outlineFreezer rowForItem:fi] withEvent:nil select:YES];
  }
}


- (void) removeFromFreezer:(Avida::Viewer::FreezerID)freezerID {
  if (freezerID.identifier == 0) return;
  
  NSAlert* alert = [[NSAlert alloc] init];
  [alert addButtonWithTitle:@"Discard"];
  [alert addButtonWithTitle:@"Cancel"];
  [alert setMessageText:@"Are you sure you would like to discard the item from the freezer?"];
  [alert setInformativeText:@"The selected freezer item will be permanently removed."];
  [alert setAlertStyle:NSWarningAlertStyle];
  if ([alert runModal] == NSAlertFirstButtonReturn) {

    freezer->Remove(freezerID);
    
    NSMutableArray* freezer_array = freezerConfigs;
    switch (freezerID.type) {
      case Avida::Viewer::CONFIG: break;
      case Avida::Viewer::GENOME: freezer_array = freezerGenomes; break;
      case Avida::Viewer::WORLD:  freezer_array = freezerWorlds;  break;
    }

    for (int i = 0; i < [freezer_array count]; i++) {
      if ([[freezer_array objectAtIndex:i] freezerID].identifier == freezerID.identifier) {
        [freezer_array removeObjectAtIndex:i];
        break;
      }
    }
    
    [outlineFreezer reloadData];
  }
}

- (void) saveAncestorsToFreezerID:(Avida::Viewer::FreezerID)freezerID
{
  Apto::String ancestors;

  for (NSUInteger idx = 0; idx < [autoAncestorArray count]; idx++) {
    ACGenome* genome = [autoAncestorArray objectAtIndex:idx];
    ancestors += [[genome name] UTF8String];
    ancestors += "\n";
    ancestors += [[genome genomeStr] UTF8String];
    ancestors += "\n";
  }
  
  freezer->SaveAttachment(freezerID, "ancestors", ancestors);

  
  ancestors = "";
  
  for (NSUInteger idx = 0; idx < [manualAncestorArray count]; idx++) {
    ACGenome* genome = [manualAncestorArray objectAtIndex:idx];
    ancestors += [[genome name] UTF8String];
    ancestors += "\n";
    ancestors += [[genome genomeStr] UTF8String];
    ancestors += "\n";
    ancestors += Apto::AsStr((int)genome.location.x);
    ancestors += ",";
    ancestors += Apto::AsStr((int)genome.location.y);
    ancestors += "\n";
  }
  
  freezer->SaveAttachment(freezerID, "ancestors_manual", ancestors);
}



- (void) activateRun {
  [self activateRunWithID:Avida::Viewer::FreezerID()];
}

- (void) activateRunWithID:(Avida::Viewer::FreezerID)fid {
  runActive = YES;
  [popViewStatView setAvidaRun:currentRun fromFreezer:freezer withID:fid];
  [matCfgRepeatability setEnabled:NO];
  [txtCfgWorldX setEnabled:NO];
  [txtCfgWorldY setEnabled:NO];
  [txtUpdate setTextColor:[NSColor controlTextColor]];

  
  // Setup inject queue based on ancestors box
  for (NSUInteger idx = 0; idx < [manualAncestorArray count]; idx++) {
    ACGenome* genome = [manualAncestorArray objectAtIndex:idx];
    Avida::GenomePtr genome_ptr(new Avida::Genome([[genome genomeStr] UTF8String]));
    
    NSPoint coord = genome.location;
    [currentRun injectGenome:genome_ptr atX:coord.x Y:coord.y withName:[[genome name] UTF8String]];
  }

  for (NSUInteger idx = 0; idx < [autoAncestorArray count]; idx++) {
    ACGenome* genome = [autoAncestorArray objectAtIndex:idx];
    Avida::GenomePtr genome_ptr(new Avida::Genome([[genome genomeStr] UTF8String]));
    
    NSPoint coord = [self locationOfOrg:idx withOrgCount:[autoAncestorArray count]];
    [currentRun injectGenome:genome_ptr atX:coord.x Y:coord.y withName:[[genome name] UTF8String]];
  }
}

- (NSPoint) locationOfOrg:(NSUInteger)orgIndex withOrgCount:(NSUInteger)count {
  assert(orgIndex < count);
  
  NSSize worldSize = [currentRun worldSize];
  
  if (count == 1) {
    return NSMakePoint(round(worldSize.width / 2.0), round(worldSize.height / 2.0));
  }
  
  if (count == 2) {
    return NSMakePoint(round(worldSize.width / 2.0), round(worldSize.height / 3.0) * (1 + orgIndex));
  }
  
  if (count == 3) {
    if (orgIndex == 0) return NSMakePoint(round(worldSize.width / 2.0), round(worldSize.height / 3.0));
    return NSMakePoint(round(worldSize.width / 3.0) * orgIndex, round(worldSize.height / 3.0) * 2.0);
  }

  if (count == 4) {
    if (orgIndex == 0)
      return NSMakePoint(round(worldSize.width / 3.0), round(worldSize.height / 3.0));
    else if (orgIndex == 1)
      return NSMakePoint(round(worldSize.width / 3.0) * 2.0, round(worldSize.height / 3.0));
    else if (orgIndex == 2)
      return NSMakePoint(round(worldSize.width / 3.0), round(worldSize.height / 3.0) * 2.0);
    else
      return NSMakePoint(round(worldSize.width / 3.0) * 2.0, round(worldSize.height / 3.0) * 2.0);
  }
  
  if (count == 5) {
    switch (orgIndex) {
      case 0:
        return NSMakePoint(round(worldSize.width / 2.0), round(worldSize.height / 2.0));
      case 1:
      case 3:
        return NSMakePoint(round(worldSize.width / 4.0 * orgIndex), round(worldSize.height / 4.0 * orgIndex));
      case 2:
        return NSMakePoint(round(worldSize.width / 4.0), round(worldSize.height * 0.75));
      case 4:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height / 4.0));
    }
  }
  
  if (count >= 6 && count <= 9) {
    switch (orgIndex) {
      case 0:
        return NSMakePoint(round(worldSize.width / 4.0), round(worldSize.height / 4.0));
      case 1:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height / 4.0));
      case 2:
        return NSMakePoint(round(worldSize.width / 4.0), round(worldSize.height / 2.0));
      case 3:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height / 2.0));
      case 4:
        return NSMakePoint(round(worldSize.width / 4.0), round(worldSize.height * 0.75));
      case 5:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.75));
      case 6:
      case 7:
      case 8:
        return NSMakePoint(round(worldSize.width / 2.0), round(worldSize.height / (count - 5) * (orgIndex - 5)));
    }
  }
  
  if (count == 10) {
    switch (orgIndex) {
      case 0:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.2));
      case 1:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height / 3.0));
      case 2:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.2));
      case 3:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.4));
      case 4:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.4));
      case 5:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.6));
      case 6:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.6));
      case 7:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.8));
      case 8:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height / 3.0 * 2.0));
      case 9:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.8));
    }
  }

  if (count == 11) {
    switch (orgIndex) {
      case 0:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.2));
      case 1:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height * 0.25));
      case 2:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.2));
      case 3:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.4));
      case 4:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.4));
      case 5:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.6));
      case 6:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.6));
      case 7:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.8));
      case 8:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height * 0.75));
      case 9:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.8));
      case 10:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height * 0.5));
    }
  }
  
  if (count == 12) {
    switch (orgIndex) {
      case 0:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.2));
      case 1:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height * 0.2));
      case 2:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.2));
      case 3:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.4));
      case 4:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height * 0.4));
      case 5:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.4));
      case 6:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.6));
      case 7:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height * 0.6));
      case 8:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.6));
      case 9:
        return NSMakePoint(round(worldSize.width * 0.25), round(worldSize.height * 0.8));
      case 10:
        return NSMakePoint(round(worldSize.width * 0.50), round(worldSize.height * 0.8));
      case 11:
        return NSMakePoint(round(worldSize.width * 0.75), round(worldSize.height * 0.8));
    }
  }
  
  
  
  NSUInteger spacing = round((worldSize.width * worldSize.height) / (count + 1));
  NSUInteger cellID = spacing * (orgIndex + 1);

  return NSMakePoint(cellID / (NSUInteger)worldSize.width, cellID % (NSUInteger)worldSize.width);
}

- (void) updatePendingInjectColors
{
  [mapView clearPendingActions];

  // Pending Queued Injects (these are already queued, thus should be colored first
  for (int i = 0; i < [currentRun pendingInjectCount]; i++) {
    NSPoint coord = [currentRun locationOfPendingInjectAtIndex:i];
    [mapView setPendingActionAtX:coord.x Y:coord.y withColor:-2];
  }
  
  // Pending Ancestor Injects
  for (NSUInteger idx = 0; idx < [manualAncestorArray count]; idx++) {
    NSPoint coord = [(ACGenome*)[manualAncestorArray objectAtIndex:idx] location];
    [mapView setPendingActionAtX:coord.x Y:coord.y withColor:-2];
  }
  for (NSUInteger idx = 0; idx < [autoAncestorArray count]; idx++) {
    NSPoint coord = [self locationOfOrg:idx withOrgCount:[autoAncestorArray count]];
    [mapView setPendingActionAtX:coord.x Y:coord.y withColor:-2];
  }

  if (!runActive && [mapViewMode indexOfSelectedItem] == 3) {
    NSMutableArray* tempAncestors = [[NSMutableArray alloc] init];
    for (ACGenome* genome in autoAncestorArray) {
      [tempAncestors addObject:genome.name];
      if (tempAncestors.count == 10) break;
    }
    for (ACGenome* genome in manualAncestorArray) {
      if (tempAncestors.count == 10) break;
      [tempAncestors addObject:genome.name];
    }
    [mapScaleView setTempState:tempAncestors];
  }
}


- (void) drawMapWithScaleInRect:(NSRect)rect inContext:(NSGraphicsContext*)gc {
  NSRect scaleRect = rect;
  scaleRect.size.height = mapScaleView.bounds.size.height;
  mapScaleView.willExport = YES;
  [mapScaleView displayRectIgnoringOpacity:scaleRect inContext:gc];
  mapScaleView.willExport = NO;
  
  NSRect mapRect = rect;
  mapRect.size.height -= scaleRect.size.height;
  
  // Set current context to the supplied graphics context
  NSGraphicsContext* currentContext = [NSGraphicsContext currentContext];
  [NSGraphicsContext setCurrentContext:gc];
  
  // Translate X coordinate, drawn, and restore coordinates
  NSAffineTransform* transform = [NSAffineTransform transform];
  [transform translateXBy:0 yBy:scaleRect.size.height];
  [transform concat];
  [mapView displayRectIgnoringOpacity:mapRect inContext:gc];
  [transform invert];
  [transform concat];

  // Restore previous graphics context
  [NSGraphicsContext setCurrentContext:currentContext];
}


- (void) setInterfacePaused {
  [btnRunState setTitle:@"Run"];
  [[app toggleRunMenuItem] setTitle:@"Run"];
  if (!runActive) {
    [sldCfgMutRate setEnabled:YES];
    [txtCfgMutRate setEnabled:YES];
    [matCfgPlacement setEnabled:YES];
    [matCfgEnv setEnabled:YES];
  }

  if (![currentRun willPause]) [matCfgPauseAt selectCellWithTag:2];
}

- (void) setInterfaceRunning {
  [btnRunState setTitle:@"Pause"];
  [[app toggleRunMenuItem] setTitle:@"Pause"];
  [sldCfgMutRate setEnabled:NO];
  [txtCfgMutRate setEnabled:NO];
  [matCfgPlacement setEnabled:NO];
  [matCfgEnv setEnabled:NO];
}



- (NSString*) uniqueNameForAncestorWithName:(NSString*)genome_name {
  bool unique = true;
  for (ACGenome* ancestor in ancestorArray) {
    if ([genome_name isEqualToString:ancestor.name]) {
      unique = false;
      break;
    }
  }
  
  int suffix = 1;
  while (!unique) {
    bool clash_found = false;
    NSString* proposed = [NSString stringWithFormat:@"%@-%d", genome_name, suffix];
    for (ACGenome* ancestor in ancestorArray) {
      if ([proposed isEqualToString:ancestor.name]) {
        clash_found = true;
        break;
      }
    }
    
    if (!clash_found) {
      return proposed;
    }
    suffix++;
  }
  
  return genome_name;
}


- (BOOL) createNonDefaultFreezer {
  
  
  NSAlert* alert = [[NSAlert alloc] init];
  [alert addButtonWithTitle:@"Save Workspace"];
  [alert addButtonWithTitle:@"Open Workspace"];
  [alert addButtonWithTitle:@"Cancel"];
  [alert setMessageText:@"Before you can save items to the freezer you must choose a workspace to work with."];
  [alert setInformativeText:@"Would you like to save the default workspace or open an existing one?"];
  [alert setAlertStyle:NSWarningAlertStyle];
  if (sheetActive) {
    [self nonDefaultFreezerAlertDidEnd:alert returnCode:[alert runModal] contextInfo:NULL];
  } else {
    [alert beginSheetModalForWindow:[self window]
                      modalDelegate:self
                     didEndSelector:@selector(nonDefaultFreezerAlertDidEnd:returnCode:contextInfo:)
                        contextInfo:NULL];
  }
  
  return (!isDefaultFreezer);
}
  
- (void) nonDefaultFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo {
  
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSURL* fileURL = nil;
  
  NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  
  assert([urls count] != 0);
  
  NSURL* userDocumentsURL = [urls objectAtIndex:0];
  NSURL* defaultFreezerURL = [NSURL URLWithString:@"default.avidaedworkspace" relativeToURL:userDocumentsURL];

  
  switch (returnCode) {
    case NSAlertFirstButtonReturn:
    {
      NSSavePanel* saveDlg = [NSSavePanel savePanel];
      [saveDlg setCanCreateDirectories:YES];
      [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"org.devosoft.avida.avida-ed-workspace"]];
      [saveDlg setTitle:@"Save Workspace As..."];
      [saveDlg setPrompt:@"Save Workspace"];
      
      // Display the dialog.  If the OK button was pressed, process the files.
      if ([saveDlg runModal] == NSFileHandlingPanelOKButton) {
        fileURL = [saveDlg URL];
        
        if ([[defaultFreezerURL absoluteURL] isEqual:[fileURL absoluteURL]] ||
            ([defaultFreezerURL isFileURL] && [fileURL isFileURL] && [[defaultFreezerURL path] isEqual:[fileURL path]])) {
          // cannot duplicate to the default freezer
          return;
        }

        if ([app isWorkspaceOpenForURL:fileURL]) {
          // cannot open already open workspace
          return;
        }

        [self duplicateFreezerAtURL:fileURL];
      } else {
        return;
      }
    }
      break;
      
    case NSAlertSecondButtonReturn:
    {
      NSOpenPanel* openDlg = [NSOpenPanel openPanel];
      [openDlg setCanChooseFiles:YES];
      [openDlg setAllowedFileTypes:[NSArray arrayWithObjects:@"org.devosoft.avida.avida-ed-workspace", @"avidaedworkspace", nil]];
      
      // Display the dialog.  If the OK button was pressed, process the files.
      if ([openDlg runModal] == NSFileHandlingPanelOKButton) {
        NSArray* files = [openDlg URLs];
        fileURL = [files objectAtIndex:0];
        
        if ([[defaultFreezerURL absoluteURL] isEqual:[fileURL absoluteURL]] ||
            ([defaultFreezerURL isFileURL] && [fileURL isFileURL] && [[defaultFreezerURL path] isEqual:[fileURL path]])) {
          // cannot duplicate to the default freezer
          return;
        }
        
        if ([app isWorkspaceOpenForURL:fileURL]) {
          // cannot open already open workspace
          return;
        }
      } else {
        return;
      }
    }
      break;
      
    case NSAlertThirdButtonReturn:
    default:
      return;
  }
  
  freezerURL = fileURL;
  Apto::String freezer_path([[freezerURL path] cStringUsingEncoding:NSASCIIStringEncoding]);
  freezer = Avida::Viewer::Freezer::CreateWithPath(freezer_path);
  [self setupFreezer];
  [outlineFreezer reloadData];
  [outlineFreezer expandItem:freezerConfigs];
  [outlineFreezer expandItem:freezerGenomes];
  [outlineFreezer expandItem:freezerWorlds];

  NSString* workspaceName = [[freezerURL lastPathComponent] stringByDeletingPathExtension];
  [self.window setTitle:[NSString stringWithFormat:@"Avida-ED : %@ Workspace", workspaceName]];

  
  // Hide freezer extension
  NSDictionary* fileAttrs = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:NSFileExtensionHidden];
  [fileManager setAttributes:fileAttrs ofItemAtPath:[freezerURL path] error:nil];
  
  isDefaultFreezer = false;
}

@end

@implementation AvidaEDController


- (id) initWithAppDelegate:(AvidaEDAppDelegate*)delegate {
  self = [super initWithWindowNibName:@"AvidaED-MainWindow"];
  
  if (self != nil) {
    app = delegate;
    
    [self setup];
    
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* tempPath = [fileManager createTemporaryDirectory];
    freezerURL = [[NSURL fileURLWithPath:tempPath isDirectory:YES] URLByAppendingPathComponent:@"default.avidaedworkspace" isDirectory:YES];
    
    isDefaultFreezer = YES;
    
    Apto::String freezer_path([[freezerURL path] cStringUsingEncoding:NSASCIIStringEncoding]);
    freezer = Avida::Viewer::Freezer::CreateWithPath(freezer_path);
    [self setupFreezer];
        
    [self showWindow:self];
  }
  
  return self;
}


- (id) initWithAppDelegate:(AvidaEDAppDelegate*)delegate inWorkspace:(NSURL*)dir {
  self = [super initWithWindowNibName:@"AvidaED-MainWindow"];
  
  if (self != nil) {
    app = delegate;
    
    [self setup];
    
    freezerURL = dir;
    isDefaultFreezer = NO;

    Apto::String freezer_path([[freezerURL path] cStringUsingEncoding:NSASCIIStringEncoding]);
    freezer = Avida::Viewer::Freezer::CreateWithPath(freezer_path);
    [self setupFreezer];
    
    // Hide freezer extension
    NSDictionary* fileAttrs = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:NSFileExtensionHidden];
    [[NSFileManager defaultManager] setAttributes:fileAttrs ofItemAtPath:[freezerURL path] error:nil];

    [self showWindow:self];
  }
  
  return self;
}


- (void) dealloc {
  delete listener;
  listener = NULL;
}


- (void) finalize {
  delete listener;
  listener = NULL;
  [super finalize];
}



- (void) duplicateFreezerAtURL:(NSURL*)url {
  Apto::String path = [[url path] cStringUsingEncoding:NSASCIIStringEncoding];
  freezer->DuplicateFreezerAt(path);
}

- (BOOL) saveFreezerAsAtURL:(NSURL*)fileURL {
  NSFileManager* fileManager = [NSFileManager defaultManager];

  NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
  
  assert([urls count] != 0);
  
  NSURL* userDocumentsURL = [urls objectAtIndex:0];
  NSURL* defaultFreezerURL = [NSURL URLWithString:@"default.avidaedworkspace" relativeToURL:userDocumentsURL];

  if ([[defaultFreezerURL absoluteURL] isEqual:[fileURL absoluteURL]] ||
      ([defaultFreezerURL isFileURL] && [fileURL isFileURL] && [[defaultFreezerURL path] isEqual:[fileURL path]])) {
    // cannot duplicate to the default freezer
    return NO;
  }
  
  if ([app isWorkspaceOpenForURL:fileURL]) {
    // cannot open already open workspace
    return NO;
  }

  [self duplicateFreezerAtURL:fileURL];
  
  freezerURL = fileURL;
  Apto::String freezer_path([[freezerURL path] cStringUsingEncoding:NSASCIIStringEncoding]);
  freezer = Avida::Viewer::Freezer::CreateWithPath(freezer_path);
  [self setupFreezer];
  [outlineFreezer reloadData];
  [outlineFreezer expandItem:freezerConfigs];
  [outlineFreezer expandItem:freezerGenomes];
  [outlineFreezer expandItem:freezerWorlds];
  
  NSString* workspaceName = [[freezerURL lastPathComponent] stringByDeletingPathExtension];
  [self.window setTitle:[NSString stringWithFormat:@"Avida-ED : %@ Workspace", workspaceName]];
  
  
  // Hide freezer extension
  NSDictionary* fileAttrs = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:NSFileExtensionHidden];
  [fileManager setAttributes:fileAttrs ofItemAtPath:[freezerURL path] error:nil];
  
  isDefaultFreezer = false;

  return YES;
}

- (Avida::Viewer::FreezerPtr) freezer {
  return freezer;
}


- (void) windowDidLoad {
  [mainSplitView replaceSubview:[[mainSplitView subviews] objectAtIndex:1] with:popView];
  [btnRunState setTitle:@"Run"];
  [[app toggleRunMenuItem] setTitle:@"Run"];
  
  
  // Replace NSClipView of mapView's scrollView with a CenteringClipView
  NSClipView* clipView = [[CenteringClipView alloc] initWithFrame:[mapScrollView frame]];
  [clipView setBackgroundColor:[NSColor lightGrayColor]];
  [mapScrollView setContentView:clipView];
  [mapScrollView setDocumentView:mapView];
  [mapScrollView setScrollsDynamically:YES];
  
  [outlineFreezer setDataSource:self];
  [outlineFreezer setDelegate:self];
  [outlineFreezer reloadData];
  [outlineFreezer expandItem:freezerConfigs];
  [outlineFreezer expandItem:freezerGenomes];
  [outlineFreezer expandItem:freezerWorlds];
  [outlineFreezer registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypePopulation, ACPasteboardTypeGenome, nil]];

  [cvAncestors registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, ACPasteboardTypeGenome, nil]];

  [imgAnalyze registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, AvidaPasteboardTypePopulation, nil]];
  [imgOrg registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, ACPasteboardTypeGenome, nil]];
  [imgTrash registerForDraggedTypes:[NSArray arrayWithObject:AvidaPasteboardTypeFreezerID]];

  NSString* workspaceName = [[freezerURL lastPathComponent] stringByDeletingPathExtension];
  [self.window setTitle:[NSString stringWithFormat:@"Avida-ED : %@ Workspace", workspaceName]];
  
  for (Avida::Viewer::Freezer::Iterator it = freezer->EntriesOfType(Avida::Viewer::CONFIG); it.Next();) {
    if (freezer->NameOf(*it.Get()) == "@default") {
      [self loadRunFromFreezer:(*it.Get())];
      break;
    }
  }
  
  [btnRunState becomeFirstResponder];
  
  analyzeCtlr = [[AvidaEDAnalyzeViewController alloc] init];
  [analyzeCtlr setDropDelegate:self];
  
  
  // Create test world for use in the organism viewer
  NSFileManager* fileManager = [NSFileManager defaultManager];
  NSString* runPath = [fileManager createTemporaryDirectory];
  Avida::Viewer::FreezerID default_world(Avida::Viewer::CONFIG, 0);
  
  freezer->InstantiateWorkingDir(default_world, [runPath cStringUsingEncoding:NSASCIIStringEncoding]);
  AvidaRun* testWorld = [[AvidaRun alloc] initWithDirectory:runPath];
  
  orgCtlr = [[AvidaEDOrganismViewController alloc] initWithWorld:testWorld];
  [orgCtlr setDropDelegate:self];
}


- (NSString*) runName {
  return [NSString stringWithFormat:@"%@ at update %d", [txtRun stringValue], [currentRun currentUpdate]];
}


- (IBAction) toggleRunState:(id)sender {
  if ([currentRun willPauseNow]) {
    if ([currentRun numOrganisms] == 0 && ![currentRun hasPendingInjects] && [ancestorArray count] == 0) {
      NSAlert* alert = [[NSAlert alloc] init];
      [alert addButtonWithTitle:@"OK"];
      [alert setMessageText:@"Unable to resume experiment; there is no start organism in the petri dish."];
      [alert setInformativeText:@"Please drag an organism from the freezer into the settings panel or the petri dish."];
      [alert setAlertStyle:NSWarningAlertStyle];
      [alert beginSheetModalForWindow:[self window] modalDelegate:nil didEndSelector:nil contextInfo:nil];
      [sender setState:NSOffState];
      return;
    }
    
    if (runActive == NO) {
      [self activateRun];
    }

    [currentRun resume];
    [self setInterfaceRunning];
  } else {
    [currentRun pause];
    [self setInterfacePaused];
  }
}

- (IBAction) changeMapViewMode:(id)sender {
  if (map) {
    map->SetMode(map_mode_to_color[[mapViewMode indexOfSelectedItem]]);
    [mapView updateState:map];
    [mapScaleView updateState:map];
    [popViewStatView mapViewModeChanged:mapView];
  } else {
    if ([mapViewMode indexOfSelectedItem] == 3) {
      NSMutableArray* tempAncestors = [[NSMutableArray alloc] init];
      for (ACGenome* genome in autoAncestorArray) {
        [tempAncestors addObject:genome.name];
        if (tempAncestors.count == 10) break;
      }
      for (ACGenome* genome in manualAncestorArray) {
        if (tempAncestors.count == 10) break;
        [tempAncestors addObject:genome.name];
      }
      [mapScaleView setTempState:tempAncestors];
    } else {
      [mapScaleView setTempState:nil];
    }
  }
  
}

- (IBAction) changeMapZoom:(id)sender {
  [mapView setZoom:[mapZoom doubleValue]];
}

- (IBAction) changeView:(id)sender {
  
  NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
  if (sender == btnPopView) {
    if (curView != popView) {
      [mainSplitView replaceSubview:curView with:popView];
      
      // Workaround for not correctly recentering clip view.
      [mapView.superview setFrame:[mapView.superview frame]];

      curView = popView;
    }
    [btnPopView setState:NSOnState];
    [btnOrgView setState:NSOffState];
    [btnAnalyzeView setState:NSOffState];
  } else if (sender == btnOrgView) {
    if (curView != orgCtlr.view) {
      [mainSplitView replaceSubview:curView with:orgCtlr.view];
      curView = orgCtlr.view;
    }
    [btnPopView setState:NSOffState];
    [btnOrgView setState:NSOnState];
    [btnAnalyzeView setState:NSOffState];
  } else if (sender == btnAnalyzeView) {
    if (curView != analyzeCtlr.view) {
      [mainSplitView replaceSubview:curView with:analyzeCtlr.view];
      curView = analyzeCtlr.view;
    }
    [btnPopView setState:NSOffState];
    [btnOrgView setState:NSOffState];
    [btnAnalyzeView setState:NSOnState];
  }
}

- (IBAction) freeze:(id)sender {
  


  if (runActive == NO) {
    [self freezeCurrentConfig];
  } else if ([popViewStatView selectedOrgGenome] == nil) {
    NSAlert* alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Population"];
    [alert addButtonWithTitle:@"Configuration"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText:@"What would you like to save to the freezer?"];
    [alert setInformativeText:@"Configuration saves the experiment settings only.\nPopulation saves organisms and experiment history."];
    [alert setAlertStyle:NSWarningAlertStyle];
    sheetActive = YES;
    [alert beginSheetModalForWindow:[self window] modalDelegate:self didEndSelector:@selector(saveRunToFreezerAlertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
  } else {
    NSAlert* alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Population"];
    [alert addButtonWithTitle:@"Organism"];
    [alert addButtonWithTitle:@"Configuration"];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText:@"What would you like to save to the freezer?"];
    [alert setInformativeText:@"Configuration saves the experiment settings only.\n\nPopulation saves organisms and experiment history."];
    [alert setAlertStyle:NSWarningAlertStyle];
    if ([popViewStatView selectedOrgGenome] != nil) {
      [[alert.buttons objectAtIndex:0] setKeyEquivalent:@""];
      [[alert.buttons objectAtIndex:1] setKeyEquivalent:@"\r"];
    }
    sheetActive = YES;
    [alert beginSheetModalForWindow:[self window] modalDelegate:self didEndSelector:@selector(saveAnyToFreezerAlertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
  }
}

- (IBAction) togglePopViewStatView:(id)sender {
  
  if (popSplitViewIsAnimating) return;
  
  if ([sender state] == NSOnState) {
    // uncollapse
    [popSplitView setDividerStyle:NSSplitViewDividerStyleThin];
    CGFloat dividerThickness = [popSplitView dividerThickness];

    NSRect oldPopViewStatViewFrame = popViewStatView.frame;
    oldPopViewStatViewFrame.size.width = 0;
    oldPopViewStatViewFrame.origin.x = popSplitView.frame.size.width;
    [popViewStatView setFrame:oldPopViewStatViewFrame];
    [popViewStatView setHidden:NO];
    
    NSMutableDictionary *expandPopViewDishViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [expandPopViewDishViewAnimationDict setObject:popViewDishView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewDishViewFrame = popViewDishView.frame;
    newPopViewDishViewFrame.size.width =  popSplitView.frame.size.width - lastPopViewStatViewWidth - dividerThickness;
    [expandPopViewDishViewAnimationDict setObject:[NSValue valueWithRect:newPopViewDishViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSMutableDictionary *expandPopViewStatViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [expandPopViewStatViewAnimationDict setObject:popViewStatView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewStatViewFrame = popViewStatView.frame;
    newPopViewStatViewFrame.size.width = lastPopViewStatViewWidth;
    newPopViewStatViewFrame.origin.x = popSplitView.frame.size.width - lastPopViewStatViewWidth;
    [expandPopViewStatViewAnimationDict setObject:[NSValue valueWithRect:newPopViewStatViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSViewAnimation *expandAnimation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects:expandPopViewDishViewAnimationDict, expandPopViewStatViewAnimationDict, nil]];
    [expandAnimation setDuration:0.25f];
    [expandAnimation startAnimation];
    [self performSelector:@selector(popSplitViewAnimationEnd:) withObject:[NSNumber numberWithBool:NO] afterDelay:0.25f];
  } else {
    // collapse
    // Store last width so we can jump back
    lastPopViewStatViewWidth = popViewStatView.frame.size.width;
    if (lastPopViewStatViewWidth < POP_SPLIT_RIGHT_MIN) lastPopViewStatViewWidth = POP_SPLIT_RIGHT_MIN;
    [popSplitView setDividerStyle:NSSplitViewDividerStylePaneSplitter];
    CGFloat dividerThickness = [popSplitView dividerThickness];
    
    NSMutableDictionary *collapseMainAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [collapseMainAnimationDict setObject:popViewDishView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewDishViewFrame = popViewDishView.frame;
    newPopViewDishViewFrame.size.width =  popSplitView.frame.size.width - dividerThickness;
    [collapseMainAnimationDict setObject:[NSValue valueWithRect:newPopViewDishViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSMutableDictionary *collapsePopViewStatViewAnimationDict = [NSMutableDictionary dictionaryWithCapacity:2];
    [collapsePopViewStatViewAnimationDict setObject:popViewStatView forKey:NSViewAnimationTargetKey];
    NSRect newPopViewStatViewFrame = popViewStatView.frame;
//    newPopViewStatViewFrame.size.width = 0.0f;
    newPopViewStatViewFrame.origin.x = popViewStatView.frame.size.width;
    [collapsePopViewStatViewAnimationDict setObject:[NSValue valueWithRect:newPopViewStatViewFrame] forKey:NSViewAnimationEndFrameKey];
    
    NSViewAnimation *collapseAnimation = [[NSViewAnimation alloc] initWithViewAnimations:[NSArray arrayWithObjects:collapseMainAnimationDict, collapsePopViewStatViewAnimationDict, nil]];
    [collapseAnimation setDuration:0.25f];
    [collapseAnimation startAnimation];
    [self performSelector:@selector(popSplitViewAnimationEnd:) withObject:[NSNumber numberWithBool:YES] afterDelay:0.25f];
  }
  popSplitViewIsAnimating = YES;
}


- (IBAction) toggleMapSettingsView:(id)sender {  
  // Hold the shift key to flip the window in slo-mo
  CGFloat flipDuration = 0.5 * (self.window.currentEvent.modifierFlags & NSShiftKeyMask ? 10.0 : 1.0);
  [mapFlipView flip:sender withDuration:flipDuration];
  if ([mapFlipView isCurrentView:[mapFlipView view1]]) {
    [btnMapSettingsFlip setTitle:@"Setup"];
    
    // Workaround for not correctly recentering clip view.  It blinks, but works for now.
    [mapView.superview setFrame:[mapView.superview frame]];
  } else {
    [btnMapSettingsFlip setTitle:@"Map"];
  }
}


- (IBAction) startNewRun:(id)sender {
  if ((!runActive ||[[txtRun stringValue] isEqualToString:@"@default"]) && ![currentRun hasPendingInjects] && [ancestorArray count] == 0) {
    NSAlert* alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:@"Drag an organism to begin."];
    [alert setInformativeText:@"Please drag an organism from the freezer into the settings panel or the petri dish."];
    [alert setAlertStyle:NSWarningAlertStyle];
    [alert beginSheetModalForWindow:[self window] modalDelegate:nil didEndSelector:nil contextInfo:nil];
    [sender setState:NSOffState];
    return;
  } else {
    for (Avida::Viewer::Freezer::Iterator it = freezer->EntriesOfType(Avida::Viewer::CONFIG); it.Next();) {
      if (freezer->NameOf(*it.Get()) == "@default") {
        [self loadRunFromFreezer:(*it.Get())];
        break;
      }
    }
  }
}



- (IBAction) saveCurrentRun:(id)sender {
  [self freezeCurrentRun];
}

- (IBAction) saveCurrentConfig:(id)sender {
  [self freezeCurrentConfig];
}

- (IBAction) saveSelectedOrganism:(id)sender {
  [self freezeGenome:[popViewStatView selectedOrgGenome]];
}


- (IBAction) exportData:(id)sender {
  NSSavePanel* saveDlg = [NSSavePanel savePanel];  
  [saveDlg setCanCreateDirectories:YES];
  [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"public.comma-separated-values-text"]];
  exportAccessoryViewCtlr = [[AvidaEDExportAccessoryController alloc] initWithNibName:@"AvidaED-ExportData" bundle:nil];
  [saveDlg setAccessoryView:[exportAccessoryViewCtlr view]];
  
  void (^completionHandler)(NSInteger) = ^(NSInteger result) {
    if (result == NSOKButton) {
      // Build up list of selected data_ids for output
      NSMutableArray* dataValues = [[NSMutableArray alloc] init];
      NSMatrix* optMat = [exportAccessoryViewCtlr optionMatrix];
      
      assert(optMat != nil);
      if ([[optMat cellWithTag:0] state] == NSOnState) [dataValues addObject:@"core.world.ave_fitness"];
      if ([[optMat cellWithTag:1] state] == NSOnState) [dataValues addObject:@"core.world.ave_gestation_time"];
      if ([[optMat cellWithTag:2] state] == NSOnState) [dataValues addObject:@"core.world.ave_metabolic_rate"];
      if ([[optMat cellWithTag:3] state] == NSOnState) [dataValues addObject:@"core.world.organisms"];
      
      id curView = [[mainSplitView subviews] objectAtIndex:1];
      if (curView == popView) {
        [popViewStatView exportData:dataValues toURL:[saveDlg URL]];
      } else if (curView == analyzeCtlr.view) {
        [analyzeCtlr exportData:dataValues toURL:[saveDlg URL]];
      }
    }
    
    // Cleanup
    exportAccessoryViewCtlr = nil;
  };
  
  // Display the dialog.  If the OK button was pressed, process the files.
  [saveDlg beginSheetModalForWindow:self.window completionHandler:completionHandler];
}



- (IBAction) exportGraphics:(id)sender {
  void (^completionHandler)(NSInteger);
  NSSavePanel* saveDlg = [NSSavePanel savePanel];

  [saveDlg setCanCreateDirectories:YES];
  [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"public.jpeg"]];

  id curView = [[mainSplitView subviews] objectAtIndex:1];
  if (curView == popView) {
    exportAccessoryViewCtlr = [[AvidaEDExportAccessoryController alloc] initWithNibName:@"AvidaED-ExportGraphics-Population" bundle:nil];
    [exportAccessoryViewCtlr setSaveDlg:saveDlg];
    [saveDlg setTitle:@"Export Image(s)"];
    [saveDlg setNameFieldStringValue:[txtRun stringValue]];
    completionHandler = ^(NSInteger result) {
      if (result == NSOKButton) {
        [self exportGraphic:(ExportGraphicsFileFormat)[exportAccessoryViewCtlr selectedFormat] withOption:[exportAccessoryViewCtlr selectedOption] toURL:[saveDlg URL]];
      }
      
      // Cleanup
      exportAccessoryViewCtlr = nil;
    };
    
  } else if (curView == analyzeCtlr.view) {
    exportAccessoryViewCtlr = [[AvidaEDExportAccessoryController alloc] initWithNibName:@"AvidaED-ExportGraphics-Analysis" bundle:nil];
    [exportAccessoryViewCtlr setSaveDlg:saveDlg];
    [saveDlg setTitle:@"Export Image"];
    completionHandler = ^(NSInteger result) {
      if (result == NSOKButton) {
        [analyzeCtlr exportGraphic:(ExportGraphicsFileFormat)[exportAccessoryViewCtlr selectedFormat] toURL:[saveDlg URL]];
      }
      
      // Cleanup
      exportAccessoryViewCtlr = nil;
    };
    
  } else if (curView == orgCtlr.view) {
    exportAccessoryViewCtlr = [[AvidaEDExportAccessoryController alloc] initWithNibName:@"AvidaED-ExportGraphics-Organism" bundle:nil];
    [exportAccessoryViewCtlr setSaveDlg:saveDlg];
    [saveDlg setTitle:@"Export Image(s)"];
    [saveDlg setNameFieldStringValue:[orgCtlr getOrganismName]];
    completionHandler = ^(NSInteger result) {
      if (result == NSOKButton) {
        [orgCtlr exportGraphic:(ExportGraphicsFileFormat)[exportAccessoryViewCtlr selectedFormat] withOptions:[exportAccessoryViewCtlr optionMatrix] toURL:[saveDlg URL]];
      }
      
      // Cleanup
      exportAccessoryViewCtlr = nil;
    };
    
  } else {
    return;
  }
  
  // Set the accessory view, if one is necessary
  if (exportAccessoryViewCtlr != nil) [saveDlg setAccessoryView:[exportAccessoryViewCtlr view]];

  // Display the dialog
  [saveDlg beginSheetModalForWindow:self.window completionHandler:completionHandler];
}


- (IBAction) exportFreezerItem:(id)sender {
  NSSavePanel* saveDlg = [NSSavePanel savePanel];
  [saveDlg setCanCreateDirectories:YES];
  [saveDlg setAllowedFileTypes:[NSArray arrayWithObject:@"org.devosoft.avida.avida-ed-freezer-item"]];
  [saveDlg setNameFieldStringValue:[NSString stringWithAptoString:freezer->NameOf([[outlineFreezer itemAtRow:[outlineFreezer selectedRow]] freezerID])]];
  
  void (^completionHandler)(NSInteger) = ^(NSInteger result) {
    if (result == NSOKButton) {
      Avida::Viewer::FreezerID fid = [[outlineFreezer itemAtRow:[outlineFreezer selectedRow]] freezerID];
      freezer->ExportItem(fid, [[[saveDlg URL] path] UTF8String]);
    }
  };
  
  // Display the dialog.  If the OK button was pressed, process the files.
  [saveDlg beginSheetModalForWindow:self.window completionHandler:completionHandler];
}


- (IBAction) importFreezerItem:(id)sender {
  NSOpenPanel* openDlg = [NSOpenPanel openPanel];
  [openDlg setCanChooseFiles:YES];
  [openDlg setAllowedFileTypes:[NSArray arrayWithObjects:@"org.devosoft.avida.avida-ed-freezer-item", @"avidaedfreezeritem", nil]];
  
  void (^completionHandler)(NSInteger) = ^(NSInteger result) {
    if (result == NSOKButton) {
      NSArray* files = [openDlg URLs];
      NSURL* fileURL = [files objectAtIndex:0];
      Avida::Viewer::FreezerID f = freezer->ImportItem([[fileURL path] UTF8String]);
      
      if (freezer->IsValid(f)) {
        FreezerItem* fi = [[FreezerItem alloc] initWithFreezerID:f];
        switch (f.type) {
          case Avida::Viewer::CONFIG: [freezerConfigs addObject:fi]; break;
          case Avida::Viewer::GENOME: [freezerGenomes addObject:fi]; break;
          case Avida::Viewer::WORLD:  [freezerWorlds addObject:fi]; break;
        }
        [outlineFreezer reloadData];
        [outlineFreezer editColumn:0 row:[outlineFreezer rowForItem:fi] withEvent:nil select:YES];
      }

    }
  };
  
  // Display the dialog.  If the OK button was pressed, process the files.
  [openDlg beginSheetModalForWindow:self.window completionHandler:completionHandler];
}


- (void) exportGraphic:(ExportGraphicsFileFormat)format withOption:(NSInteger)selectedOpt toURL:(NSURL*)url {
  NSRect exportRect;
  switch (selectedOpt) {
    case 0:
      exportRect = mapView.bounds;
      exportRect.size.height += mapScaleView.bounds.size.height;
      break;
    case 1:
      exportRect = popViewStatView.graphView.bounds;
      break;
    case 2:
      exportRect = popView.bounds;
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
      switch (selectedOpt) {
        case 0:
          [self drawMapWithScaleInRect:exportRect inContext:gc];
          break;
        case 1:
          [popViewStatView.graphView displayRectIgnoringOpacity:exportRect inContext:gc];
          break;
        case 2:
          [popView displayRectIgnoringOpacity:exportRect inContext:gc];
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
      
      switch (selectedOpt) {
        case 0:
        {
          // Draw view into graphics pdfContext
          NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:pdfContext flipped:NO];
          [self drawMapWithScaleInRect:exportRect inContext:gc];
        }
          break;
        case 1:
          // The following will draw the graph into the PDF in vector format
          [popViewStatView.graphView.hostedGraph layoutAndRenderInContext:pdfContext];
          break;
          
        case 2:
        {
          // Draw view into graphics pdfContext
          NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:pdfContext flipped:NO];
          [popView displayRectIgnoringOpacity:exportRect inContext:gc];
        }
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



- (IBAction) changeMutationRate:(id)sender {
  double rate;
  if (sender == sldCfgMutRate) {
    rate = [sldCfgMutRate floatValue];
    if (rate < round([sldCfgMutRate minValue])) {
      rate = 0;
    } else {
      rate = pow(10, rate);
    }
  } else {
    rate = [txtCfgMutRate floatValue];
  }
  [sldCfgMutRate setFloatValue:(rate == 0) ? [sldCfgMutRate minValue] : log10(rate)];
  [txtCfgMutRate setFloatValue:rate];
  [currentRun setMutationRate:rate];
}


- (IBAction) changeWorldSize:(id)sender {
  
  if (manualAncestorArray.count > 0) {
    NSAlert* alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:@"Oops! The organism you placed was removed when you resized the dish."];
    [alert setInformativeText:@"Please drag in a new starting organism(s)."];
    [alert setAlertStyle:NSWarningAlertStyle];
    [alert beginSheetModalForWindow:[self window] modalDelegate:nil didEndSelector:nil contextInfo:nil];
    
    for (ACGenome* genome in manualAncestorArray) [ancestorArrayCtlr removeObject:genome];
    [manualAncestorArray removeAllObjects];
  }

  [currentRun setWorldSize:NSMakeSize([txtCfgWorldX intValue], [txtCfgWorldY intValue])];
  [mapView setDimensions:[currentRun worldSize]];
  [mapZoom setDoubleValue:[mapView zoom]];
  
  [self updatePendingInjectColors];
}


- (IBAction) changePlacement:(id)sender {
  if ([[matCfgPlacement selectedCell] tag] == 0) {
    [currentRun setPlacementMode:0]; // neighborhood
  } else {    
    [currentRun setPlacementMode:4]; // well-mixed
  }
}


- (IBAction) changeEnvironment:(id)sender {
  [currentRun setReactionValueOf:"NOT"  to:([[matCfgEnv cellWithTag:0] state] == NSOnState) ? 1.0 : 0.0];
  [currentRun setReactionValueOf:"NAND" to:([[matCfgEnv cellWithTag:1] state] == NSOnState) ? 1.0 : 0.0];
  [currentRun setReactionValueOf:"AND"  to:([[matCfgEnv cellWithTag:2] state] == NSOnState) ? 2.0 : 0.0];
  [currentRun setReactionValueOf:"ORN"  to:([[matCfgEnv cellWithTag:3] state] == NSOnState) ? 2.0 : 0.0];
  [currentRun setReactionValueOf:"OR"   to:([[matCfgEnv cellWithTag:4] state] == NSOnState) ? 3.0 : 0.0];
  [currentRun setReactionValueOf:"ANDN" to:([[matCfgEnv cellWithTag:5] state] == NSOnState) ? 3.0 : 0.0];
  [currentRun setReactionValueOf:"NOR"  to:([[matCfgEnv cellWithTag:6] state] == NSOnState) ? 4.0 : 0.0];
  [currentRun setReactionValueOf:"XOR"  to:([[matCfgEnv cellWithTag:7] state] == NSOnState) ? 4.0 : 0.0];
  [currentRun setReactionValueOf:"EQU"  to:([[matCfgEnv cellWithTag:8] state] == NSOnState) ? 5.0 : 0.0];
}


- (IBAction) changeRepeatability:(id)sender {
  if ([[matCfgRepeatability selectedCell] tag] == 0) {
    [currentRun setRandomSeed:-1]; // full random
  } else {    
    [currentRun setRandomSeed:100]; // fixed seed
  }  
}


- (IBAction) changePauseAt:(id)sender {
  // Make sure pause value is a positive integer
  int pause_value = [txtCfgPauseAt intValue];
  if (sender == stpCfgPauseAt) pause_value = [stpCfgPauseAt intValue];
  if (pause_value < 0) pause_value = 0;
  
  [txtCfgPauseAt setIntValue:pause_value];
  [stpCfgPauseAt setIntValue:pause_value];

  // If pause at selected, set the value on the current run
  if ([[matCfgPauseAt selectedCell] tag] == 1) {
    [currentRun pauseAt:pause_value];
  } else {
    [currentRun pauseAt:-1]; // effectively clear pause at
  }
}



- (void) envActionStateChange:(NSMutableDictionary*)newState
{
  Apto::String enabled_actions;
  
  NSEnumerator *enumerator = [newState keyEnumerator];
  NSString* key;
  
  while ((key = [enumerator nextObject])) {
    if ([[newState objectForKey:key] unsignedIntValue] == NSOnState) {
      if (enabled_actions.GetSize()) enabled_actions += ",";
      if ([key isEqual:@"oro"]) {
        key = @"or";
      } else if ([key isEqual:@"ant"]) {
        key = @"andn";
      } else if ([key isEqual:@"nan"]) {
        key = @"nand";
      }
      enabled_actions += [key UTF8String];
    }
  }
  
  map->SetModeProperty(map->GetTagMode(), "enabled_actions", enabled_actions);
  [mapView updateState:map];
}


- (BOOL) validateMenuItem:(NSMenuItem*)item {
  SEL item_action = [item action];

  if (item_action == @selector(saveCurrentRun:) && runActive == NO) return NO;
  if (item_action == @selector(saveSelectedOrganism:) && [popViewStatView selectedOrgGenome] == nil) return NO;
  
  if (item_action == @selector(exportData:)) {
    // Cannot export data while in the organism view
    NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
    if (curView == orgCtlr.view) return NO;
    if (curView == popView && runActive == NO) return NO;
    if (curView == analyzeCtlr.view && [analyzeCtlr numPops] == 0) return NO;
  }
  
  if (item_action == @selector(exportFreezerItem:)) {
    if ([outlineFreezer numberOfSelectedRows] == 0) return NO;
  }
  
  return YES;
}

- (void) splitView:(NSSplitView*)splitView resizeSubviewsWithOldSize:(NSSize)oldSize {
  if (splitView == mainSplitView) {
    NSView* leftView = [[splitView subviews] objectAtIndex:0];
    NSView* rightView = [[splitView subviews] objectAtIndex:1];
    NSRect newFrame = [splitView frame];
    NSRect leftFrame = [leftView frame];
    NSRect rightFrame = [rightView frame];
    
    float dividerThickness = [splitView dividerThickness];
    
    //int diffWidth = newFrame.size.width - oldSize.width;
    
    if (leftFrame.size.width < MAIN_SPLIT_LEFT_MIN) {
      leftFrame.size.width = MAIN_SPLIT_LEFT_MIN;
      rightFrame.size.width = newFrame.size.width - dividerThickness - MAIN_SPLIT_LEFT_MIN;
    } else if (rightFrame.size.width < MAIN_SPLIT_RIGHT_MIN) {
      leftFrame.size.width = newFrame.size.width - dividerThickness - MAIN_SPLIT_RIGHT_MIN;
      rightFrame.size.width = MAIN_SPLIT_RIGHT_MIN;
    } else {
      rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    }
    
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    rightFrame.size.height = newFrame.size.height;
    rightFrame.origin.x = leftFrame.size.width + dividerThickness;
    
//    printf("outer = %d x %d @ (%d, %d)  right = %d x %d @ (%d, %d)  left = %d x %d @ (%d, %d)\n",
//           (int)newFrame.size.width, (int)newFrame.size.height, (int)newFrame.origin.x, (int)newFrame.origin.y,
//           (int)rightFrame.size.width, (int)rightFrame.size.height, (int)rightFrame.origin.x, (int)rightFrame.origin.y,
//           (int)leftFrame.size.width, (int)leftFrame.size.height, (int)leftFrame.origin.x, (int)leftFrame.origin.y);

    [leftView setFrame:leftFrame];
    [rightView setFrame:rightFrame];
    [mainSplitView adjustSubviews];
  } else if (splitView == popSplitView) {
    NSView* leftView = [[splitView subviews] objectAtIndex:0];
    NSView* rightView = [[splitView subviews] objectAtIndex:1];
    NSRect newFrame = [splitView frame];
    NSRect leftFrame = [leftView frame];
    NSRect rightFrame = [rightView frame];
    
    CGFloat dividerThickness = [splitView dividerThickness];
    
    if ([splitView isSubviewCollapsed:popViewStatView]) {
      leftFrame.size.height = newFrame.size.height;
      leftFrame.origin = NSMakePoint(0, 0);
      leftFrame.size.width = newFrame.size.width - dividerThickness;      
    } else {
      CGFloat diffWidth = floor(newFrame.size.width - oldSize.width);
      
      if (rightFrame.size.width <= POP_SPLIT_RIGHT_MIN && !popSplitViewIsAnimating) {
        leftFrame.size.width = newFrame.size.width - dividerThickness - POP_SPLIT_RIGHT_MIN;
        rightFrame.size.width = POP_SPLIT_RIGHT_MIN;
      } else if (leftFrame.size.width > POP_SPLIT_LEFT_MIN) {
        leftFrame.size.width += floor(diffWidth * POP_SPLIT_LEFT_PROPORTIONAL_RESIZE);
        rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
      }
      
      leftFrame.size.height = newFrame.size.height;
      leftFrame.origin = NSMakePoint(0, 0);
      rightFrame.size.height = newFrame.size.height;
      rightFrame.origin.x = leftFrame.size.width + dividerThickness;
    }
    
    if (rightFrame.size.width) lastPopViewStatViewWidth = rightFrame.size.width;
    
//    printf("outer = %d x %d @ (%d, %d)  right = %d x %d @ (%d, %d)  left = %d x %d @ (%d, %d)\n",
//           (int)newFrame.size.width, (int)newFrame.size.height, (int)newFrame.origin.x, (int)newFrame.origin.y,
//           (int)rightFrame.size.width, (int)rightFrame.size.height, (int)rightFrame.origin.x, (int)rightFrame.origin.y,
//           (int)leftFrame.size.width, (int)leftFrame.size.height, (int)leftFrame.origin.x, (int)leftFrame.origin.y);

    [leftView setFrame:leftFrame];
    [rightView setFrame:rightFrame];
    [splitView adjustSubviews];
  }
}


- (BOOL) splitView:(NSSplitView*)splitView canCollapseSubview:(NSView*)subview {
  if (splitView == mainSplitView && subview == [[splitView subviews] objectAtIndex:0]) return YES;
  else if (splitView == popSplitView && subview == [[splitView subviews] objectAtIndex:1]) return YES;
  
  return NO;
}


- (CGFloat) splitView:(NSSplitView*)splitView constrainMaxCoordinate:(CGFloat)proposedMax ofSubviewAt:(NSInteger)index {
  if (splitView == mainSplitView) return proposedMax - MAIN_SPLIT_RIGHT_MIN;
  else if (splitView == popSplitView) return proposedMax - POP_SPLIT_RIGHT_MIN;
  return proposedMax;
}


- (CGFloat) splitView:(NSSplitView*)splitView constrainMinCoordinate:(CGFloat)proposedMin ofSubviewAt:(NSInteger)index {
  if (splitView == mainSplitView) return proposedMin + MAIN_SPLIT_LEFT_MIN;
  else if (splitView == popSplitView) return proposedMin + POP_SPLIT_LEFT_MIN;
  
  return proposedMin;
}

- (void) splitViewDidResizeSubviews:(NSNotification*)notification {
  if ([notification object] == popSplitView) {
    if (!popSplitViewIsAnimating) {
      if ([popSplitView isSubviewCollapsed:popViewStatView]) {
        [btnTogglePopViewStatView setState:NSOffState];
        [popSplitView setDividerStyle:NSSplitViewDividerStylePaneSplitter];
      } else {
        [btnTogglePopViewStatView setState:NSOnState];
        [popSplitView setDividerStyle:NSSplitViewDividerStyleThin];
      }
      [popSplitView adjustSubviews];
    }
  }
  if ([notification object] == mainSplitView) {
    if ([mainSplitView isSubviewCollapsed:mainSplitViewLeft]) {
      [mainSplitView setDividerStyle:NSSplitViewDividerStylePaneSplitter];
    } else {
      [mainSplitView setDividerStyle:NSSplitViewDividerStyleThin];
    }
    [mainSplitView adjustSubviews];
  }
}

- (void) windowWillClose:(NSNotification*)notification {
  if (currentRun != nil) {
    [currentRun end];
    currentRun = nil;
  }
  freezer = Avida::Viewer::FreezerPtr(NULL); // Force freezer cleanup
  [app removeWindow:self];
}

- (void) windowDidBecomeMain:(NSNotification*)notification {
  [[app toggleRunMenuItem] setTitle:[btnRunState title]];
}


- (BOOL)outlineView:(NSOutlineView*)outlineView shouldEditTableColumn:(NSTableColumn*)tableColumn item:(id)item
{
  if (item == nil || item == freezerConfigs || item == freezerGenomes || item == freezerWorlds) {
    return NO;
  }
  
  if ([item freezerID].identifier == 0) return NO;
  
  return YES;
}


- (BOOL)outlineView:(NSOutlineView*)outlineView shouldSelectItem:(id)item
{
  if (item == nil || item == freezerConfigs || item == freezerGenomes || item == freezerWorlds) {
    return NO;
  }
  
  return YES;
}



- (BOOL) outlineView:(NSOutlineView*)outlineView acceptDrop:(id<NSDraggingInfo>)info item:(id)item childIndex:(NSInteger)index
{
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:AvidaPasteboardTypePopulation]] != nil) {
    switch ([info draggingSourceOperationMask]) {
      case (NSDragOperationCopy | NSDragOperationLink):
      {
        NSAlert* alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle:@"Population"];
        [alert addButtonWithTitle:@"Configation"];
        [alert addButtonWithTitle:@"Cancel"];
        [alert setMessageText:@"What would you like to save to the freezer?"];
        [alert setInformativeText:@"Configation saves the experiment settings only.\nPopulation saves organisms and experiment history."];
        [alert setAlertStyle:NSWarningAlertStyle];
        sheetActive = YES;
        [alert beginSheetModalForWindow:[self window] modalDelegate:self didEndSelector:@selector(saveRunToFreezerAlertDidEnd:returnCode:contextInfo:) contextInfo:NULL];
        return YES;
      }
      case NSDragOperationCopy:
        [self freezeCurrentRun];
        return YES;
      case NSDragOperationLink:
        [self freezeCurrentConfig];
        return YES;
    }
  }
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:ACPasteboardTypeGenome]] != nil) {
    [self freezeGenome:[ACGenome genomeFromPasteboard:[info draggingPasteboard]]];
    return YES;
  }
  return NO;
}


- (NSDragOperation) outlineView:(NSOutlineView*)outlineView validateDrop:(id<NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(NSInteger)index
{
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:AvidaPasteboardTypePopulation]] != nil) {
    [outlineView setDropItem:nil dropChildIndex:NSOutlineViewDropOnItemIndex];
    return [info draggingSourceOperationMask];
  }
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:ACPasteboardTypeGenome]] != nil) {
    [outlineView setDropItem:nil dropChildIndex:NSOutlineViewDropOnItemIndex];
    return NSDragOperationCopy;    
  }
  return NSDragOperationNone;
}


- (id) outlineView:(NSOutlineView*)outlineView child:(NSInteger)index ofItem:(id)item {
  if (item == nil) {
    switch (index) {
      case 0:
        return freezerConfigs;
      case 1:
        return freezerGenomes;
      case 2:
        return freezerWorlds;
      default:
        return nil;
    }
  } else if (item == freezerConfigs || item == freezerGenomes || item == freezerWorlds) {
    return [item objectAtIndex:index];
  }
  
  return nil;
}

- (BOOL) outlineView:(NSOutlineView*)outlineView isItemExpandable:(id)item {
  if (item == nil || item == freezerConfigs || item == freezerGenomes || item == freezerWorlds) {
    return YES;
  }
  
  return NO;
}


- (NSInteger) outlineView:(NSOutlineView*)outlineView numberOfChildrenOfItem:(id)item {
  if (item == nil) return 3; // Top level items
  
  if (item == freezerConfigs || item == freezerGenomes || item == freezerWorlds) return [item count];
  
  return -1;
}

- (id) outlineView:(NSOutlineView*)outlineView objectValueForTableColumn:(NSTableColumn*)tableColumn byItem:(id)item {
  if (item == nil) return @"";
  
  if (item == freezerConfigs) return @"Configured Dishes";
  if (item == freezerWorlds) return @"Populated Dishes";
  if (item == freezerGenomes) return @"Organisms";
  
  return [NSString stringWithAptoString:freezer->NameOf([item freezerID])];
}

- (void) outlineView:(NSOutlineView*)outlineView setObjectValue:(id)object forTableColumn:(NSTableColumn*)tableColumn byItem:(id)item {
  freezer->Rename([item freezerID], Apto::String([object UTF8String]));
  switch ([item freezerID].type) {
    case Avida::Viewer::CONFIG: [freezerConfigs sortUsingFunction:&sortFreezerItems context:&freezer]; break;
    case Avida::Viewer::GENOME: [freezerGenomes sortUsingFunction:&sortFreezerItems context:&freezer]; break;
    case Avida::Viewer::WORLD:  [freezerWorlds sortUsingFunction:&sortFreezerItems context:&freezer]; break;
    default: break;
  }
  [outlineView reloadData];
}


- (BOOL) outlineView:(NSOutlineView*)outlineView writeItems:(NSArray*)items toPasteboard:(NSPasteboard*)pboard {
  int written = 0;
  for (int i = 0; i < [items count]; i++) {
    id item = [items objectAtIndex:i];
    if (item == nil || item == freezerConfigs || item == freezerWorlds || item == freezerGenomes) continue;
    Avida::Viewer::FreezerID fid = [(FreezerItem*)item freezerID];
    [Freezer writeFreezerID:fid toPasteboard:pboard];
    written++;
  }
  return (written) ? YES : NO;
}


- (void) outlineViewDidReceiveDeleteKey:(NSOutlineView*)outlineView {
  Avida::Viewer::FreezerID fid = [[outlineView itemAtRow:[outlineView selectedRow]] freezerID];
  [self removeFromFreezer:fid];
}

- (void) outlineViewDidReceiveEnterOrSpaceKey:(NSOutlineView*)outlineView {
//  Avida::Viewer::FreezerID fid = [[outlineView itemAtRow:[outlineView selectedRow]] freezerID];
//  if (fid.type == Avida::Viewer::CONFIG || fid.type == Avida::Viewer::WORLD) {
//    [self loadRunFromFreezer:fid];
//  } else {
//    // handle genome
//  }
}




- (void) mapView:(MapGridView*)map handleDraggedConfig:(Avida::Viewer::FreezerID)fid
{
  [self loadRunFromFreezer:fid];
}

- (void) mapView:(MapGridView*)map handleDraggedFreezerGenome:(Avida::Viewer::FreezerID)fid atX:(int)x Y:(int)y
{
  Avida::GenomePtr genome_ptr(freezer->InstantiateGenome(fid));
  if (genome_ptr) {
    if (runActive) {
      // Disabled without advanced mode
//      [currentRun injectGenome:genome_ptr atX:x Y:y withName:freezer->NameOf(fid)];
//      [mapView setPendingActionAtX:x Y:y withColor:-2];
    } else {
      ACGenome* genome = [[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:genome_ptr->AsString()] name:[NSString stringWithAptoString:freezer->NameOf(fid)]];
      genome.location = NSMakePoint(x, y);
      genome.name = [self uniqueNameForAncestorWithName:genome.name];
      [ancestorArrayCtlr addObject:genome];
      [manualAncestorArray addObject:genome];
      [self updatePendingInjectColors];
    }
  }
}

- (void) mapView:(MapGridView*)map handleDraggedGenome:(ACGenome*)genome atX:(int)x Y:(int)y
{
  Avida::GenomePtr genome_ptr(new Avida::Genome([[genome genomeStr] UTF8String]));
  if (genome_ptr) {
    if (runActive) {
      // Disabled without advanced mode
//      [currentRun injectGenome:genome_ptr atX:x Y:y withName:[[genome name] UTF8String]];
//      [mapView setPendingActionAtX:x Y:y withColor:-2];
    } else {
      genome.location = NSMakePoint(x,y);
      genome.name = [self uniqueNameForAncestorWithName:genome.name];

      [ancestorArrayCtlr addObject:genome];
      [manualAncestorArray addObject:genome];
      [self updatePendingInjectColors];
    }
  }
}

- (void) mapView:(MapGridView*)map handleDraggedWorld:(Avida::Viewer::FreezerID)fid {
  [self loadRunFromFreezer:fid];
}



- (NSDragOperation) draggingSession:(NSDraggingSession*)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context {
  switch (context) {
    case NSDraggingContextWithinApplication:
      return NSDragOperationCopy | NSDragOperationLink;
      
    case NSDraggingContextOutsideApplication:
    default:
      return NSDragOperationNone;
      break;
  }
}

- (BOOL) ignoreModifierKeysForDraggingSession:(NSDraggingSession*)session
{
  return NO;
}


- (void) draggableImageView:(DraggableImageView*)imageView writeToPasteboard:(NSPasteboard*)pboard {
  [pboard writeObjects:[[NSArray alloc] initWithObjects:[[Population alloc] init], nil]];
}






- (NSDragOperation) draggingEnteredDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
  
  if (destination == imgAnalyze) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::WORLD && (sourceDragMask & NSDragOperationGeneric) && [analyzeCtlr numPops] < 4) {
        return [analyzeCtlr willAcceptPopWithFreezerID:fid] ? NSDragOperationCopy : NSDragOperationNone;
      }
    } else if ([[pboard types] containsObject:AvidaPasteboardTypePopulation]) {
      return NSDragOperationCopy;
    }
  } else if (destination == imgOrg) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
        return NSDragOperationCopy;
      }
    } else if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
      return NSDragOperationCopy;
    }
  } else if (destination == imgTrash) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) return NSDragOperationDelete;
  } else if (curView == analyzeCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::WORLD && (sourceDragMask & NSDragOperationGeneric) && [analyzeCtlr numPops] < 4) {
        return [analyzeCtlr willAcceptPopWithFreezerID:fid] ? NSDragOperationGeneric : NSDragOperationNone;
      }
    }
  } else if (curView == orgCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
        return NSDragOperationGeneric;
      }
    }
  }
  
  return NSDragOperationNone;
}

- (NSDragOperation) draggingUpdatedForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
  
  if (destination == imgAnalyze) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::WORLD && (sourceDragMask & NSDragOperationGeneric) && [analyzeCtlr numPops] < 4) {
        return [analyzeCtlr willAcceptPopWithFreezerID:fid] ? NSDragOperationCopy : NSDragOperationNone;
      }
    } else if ([[pboard types] containsObject:AvidaPasteboardTypePopulation]) {
      return NSDragOperationCopy;
    }
  } else if (destination == imgOrg) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
        return NSDragOperationCopy;
      }
    } else if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
      return NSDragOperationCopy;
    }
  } else if (destination == imgTrash) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) return NSDragOperationDelete;
  } else if (curView == analyzeCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::WORLD && (sourceDragMask & NSDragOperationGeneric) && [analyzeCtlr numPops] < 4) {
        return [analyzeCtlr willAcceptPopWithFreezerID:fid] ? NSDragOperationGeneric : NSDragOperationNone;
      }
    }
  } else if (curView == orgCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
        return NSDragOperationGeneric;
      }
    }
  }
  
  return NSDragOperationNone;  
}

- (BOOL) prepareForDragOperationForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
  NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
  
  if (destination == imgAnalyze) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::WORLD && (sourceDragMask & NSDragOperationGeneric) && [analyzeCtlr numPops] < 4) {
        return [analyzeCtlr willAcceptPopWithFreezerID:fid] ? YES : NO;
      }
    } else if ([[pboard types] containsObject:AvidaPasteboardTypePopulation]) {
      return YES;
    }
  } else if (destination == imgOrg) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
        return YES;
      }
    } else if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
      return YES;
    }
  } else if (destination == imgTrash) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) return YES;
  } else if (curView == analyzeCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::WORLD && (sourceDragMask & NSDragOperationGeneric) && [analyzeCtlr numPops] < 4) {
        return [analyzeCtlr willAcceptPopWithFreezerID:fid] ? YES : NO;
      }
    }
  } else if (curView == orgCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if (fid.type == Avida::Viewer::GENOME && (sourceDragMask & NSDragOperationGeneric)) {
        return YES;
      }
    }
  }
  
  return NO;
}

- (BOOL) performDragOperationForDestination:(id<NSDraggingDestination>)destination sender:(id<NSDraggingInfo>)sender {
  NSPasteboard* pboard = [sender draggingPasteboard];
  NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
  
  if (destination == imgAnalyze) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
            
      [self changeView:btnAnalyzeView];
      if ([analyzeCtlr willAcceptPopWithFreezerID:fid]) {
        AvidaEDAnalyzePopulation* pop = [[AvidaEDAnalyzePopulation alloc] initWithFreezerID:fid fromFreezer:freezer];
        [analyzeCtlr addPop:pop];
      }
    } else if ([[pboard types] containsObject:AvidaPasteboardTypePopulation]) {
      // clear out analyze graphs
      [analyzeCtlr clearAllPops];
      
      [currentRun pause];
      
      // @TODO - fix this ugly busy wait
      while (![currentRun isPaused]);
      
      Avida::Viewer::FreezerID fid = freezer->SaveWorld([currentRun oldworld], [[self runName] UTF8String]);
      if (freezer->IsValid(fid)) {
        // Save ancestor info
        [self saveAncestorsToFreezerID:fid];
        
        // Save plot info
        [popViewStatView saveRunToFreezer:freezer withID:fid];
      }
      freezer->Remove(fid);
      
      [self changeView:btnAnalyzeView];
      AvidaEDAnalyzePopulation* pop = [[AvidaEDAnalyzePopulation alloc] initWithFreezerID:fid fromFreezer:freezer];
      [analyzeCtlr addPop:pop];
    }
  } else if (destination == imgOrg) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      
      // Get genome from freezer
      Avida::GenomePtr genome = freezer->InstantiateGenome(fid);
      
      [self changeView:btnOrgView];
      [orgCtlr setGenome:genome withName:[NSString stringWithAptoString:freezer->NameOf(fid)]];
    } else if ([[pboard types] containsObject:ACPasteboardTypeGenome]) {
      ACGenome* genome = [ACGenome genomeFromPasteboard:pboard];
      
      // Get genome from freezer
      Avida::GenomePtr genome_ptr(new Avida::Genome([[genome genomeStr] UTF8String]));
      
      [self changeView:btnOrgView];
      [orgCtlr setGenome:genome_ptr withName:[genome name]];
    }
  } else if (destination == imgTrash) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      [self removeFromFreezer:fid];
    }
  } else if (curView == analyzeCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];
      if ([analyzeCtlr willAcceptPopWithFreezerID:fid]) {
        AvidaEDAnalyzePopulation* pop = [[AvidaEDAnalyzePopulation alloc] initWithFreezerID:fid fromFreezer:freezer];
        [analyzeCtlr addPop:pop];
      }
    }
  } else if (curView == orgCtlr.view) {
    if ([[pboard types] containsObject:AvidaPasteboardTypeFreezerID]) {
      Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:pboard];    
      
      // Get genome from freezer
      Avida::GenomePtr genome = freezer->InstantiateGenome(fid);
      
      [orgCtlr setGenome:genome withName:[NSString stringWithAptoString:freezer->NameOf(fid)]];
    }
  }
  
  return YES;
}


- (BOOL) collectionView:(NSCollectionView*)collectionView acceptDrop:(id<NSDraggingInfo>)info index:(NSInteger)index dropOperation:(NSCollectionViewDropOperation)dropOperation {
  if (runActive) return NO;
  
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:ACPasteboardTypeGenome]] != nil) {
    ACGenome* genome = [ACGenome genomeFromPasteboard:[info draggingPasteboard]];
    genome.name = [self uniqueNameForAncestorWithName:genome.name];
    [ancestorArrayCtlr insertObject:genome atArrangedObjectIndex:index];
    [autoAncestorArray addObject:genome];
    [self updatePendingInjectColors];
    return YES;
  }
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:AvidaPasteboardTypeFreezerID]] != nil) {
    Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:[info draggingPasteboard]];
    if (fid.type == Avida::Viewer::GENOME) {
      // Get genome from freezer
      Avida::GenomePtr genome = freezer->InstantiateGenome(fid);
      ACGenome* objc_genome = [[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:genome->AsString()] name:[NSString stringWithAptoString:freezer->NameOf(fid)]];
      objc_genome.name = [self uniqueNameForAncestorWithName:objc_genome.name];
      [ancestorArrayCtlr insertObject:objc_genome atArrangedObjectIndex:index];
      [autoAncestorArray addObject:objc_genome];
      [self updatePendingInjectColors];
      return YES;
    }
  }
  
  return NO;
}


- (NSDragOperation) collectionView:(NSCollectionView*)collectionView validateDrop:(id<NSDraggingInfo>)info proposedIndex:(NSInteger*)proposedDropIndex dropOperation:(NSCollectionViewDropOperation*)proposedDropOperation {
  if (runActive) return NSDragOperationNone;
  
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:ACPasteboardTypeGenome]] != nil) {
    return NSDragOperationCopy;
  }
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:AvidaPasteboardTypeFreezerID]] != nil) {
    Avida::Viewer::FreezerID fid = [Freezer freezerIDFromPasteboard:[info draggingPasteboard]];
    if (fid.type == Avida::Viewer::GENOME) return NSDragOperationCopy;
  }
  return NSDragOperationNone;
}

- (BOOL) collectionView:(NSCollectionView*)collectionView writeItemsAtIndexes:(NSIndexSet*)indexes toPasteboard:(NSPasteboard*)pasteboard {
  for (NSUInteger currentIndex = [indexes firstIndex]; currentIndex != NSNotFound; currentIndex = [indexes indexGreaterThanIndex: currentIndex]) {
    [ACGenome writeGenome:[ancestorArray objectAtIndex:currentIndex] toPasteboard:pasteboard];
  }
  return YES;
}


@synthesize listener;
@synthesize mapView;
@synthesize freezerURL;
@synthesize curUpdate;


- (void) handleMap:(ViewerMap*)pkg {
  if (!map) {
    map = [pkg map];
    NSUInteger selectedMode = [mapViewMode indexOfSelectedItem];
    [mapViewMode removeAllItems];
    map_mode_to_color.Clear();
    int idx = 0;
    for (int i = 0; i < map->GetNumModes(); i++) {
      if (!(map->GetModeSupportedTypes(i) & Avida::Viewer::MAP_GRID_VIEW_COLOR)) continue;
      [mapViewMode addItemWithTitle:[NSString stringWithUTF8String:(const char*)map->GetModeName(i)]];
      map_mode_to_color[idx++] = i;
    }
    if (selectedMode < mapViewMode.numberOfItems) {
      [mapViewMode selectItemAtIndex:selectedMode];
      [self changeMapViewMode:mapViewMode];
    } else {
      [mapViewMode selectItemAtIndex:map->GetColorMode()];
    }
  } else {
    map = [pkg map];
  }
  [mapView updateState:map];
  [mapScaleView updateState:map];
  [mapZoom setDoubleValue:[mapView zoom]];
}


- (void) handleUpdate:(ViewerUpdate*)pkg {
  curUpdate = [pkg update];
  if (curUpdate == 1) {
    [txtUpdate setStringValue:@"1 update"];
  } else {
    NSString* str = [NSString stringWithFormat:@"%d updates", curUpdate];
    [txtUpdate setStringValue:str]; 
  }
}

- (void) handleRunPaused:(id)unused {
  if (runActive) [self setInterfacePaused];
}

- (void) handleRunSync:(id)unused {
  if (currentRun) [currentRun sync];
}


@end
