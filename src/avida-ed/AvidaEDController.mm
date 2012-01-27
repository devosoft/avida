//
//  AvidaEDController.mm
//  avida/apps/viewer-macos
//
//  Created by David on 11/30/10.
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

#import <QuartzCore/QuartzCore.h>

#import "AvidaAppDelegate.h"
#import "AvidaRun.h"
#import "CenteringClipView.h"
#import "FlipView.h"
#import "Freezer.h"
#import "Genome.h"
#import "MapGridView.h"
#import "NSFileManager+TemporaryDirectory.h"
#import "NSString+Apto.h"

#import "AvidaEDPopViewStatView.h"

#include "avida/viewer/Map.h"

static const float MAIN_SPLIT_LEFT_MIN = 140.0;
static const float MAIN_SPLIT_RIGHT_MIN = 710.0;
static const float MAIN_SPLIT_LEFT_PROPORTIONAL_RESIZE = 0.5;
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
- (void) popSplitViewAnimationEnd:(NSNumber*)collapsed;
- (void) setupFreezer;
- (void) loadRunFromFreezer:(Avida::Viewer::FreezerID)freezerID;
- (void) loadRunFromFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo;
- (void) saveRunToFreezerAlertDidEnd:(NSAlert*)alert returnCode:(NSInteger)returnCode contextInfo:(void*)contextInfo;
- (void) clearCurrentRun;
- (void) freezeCurrentConfig;
- (void) freezeCurrentRun;
- (void) freezeGenome:(Genome*)genome;
- (void) removeFromFreezer:(Avida::Viewer::FreezerID)freezerID;
- (void) activateRun;
@end

@implementation AvidaEDController (hidden)

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
    [mapView setDimensions:[currentRun worldSize]];
  } else {
    currentRun = [[AvidaRun alloc] initWithDirectory:runPath];
    [self activateRun];
    [txtUpdate setStringValue:[NSString stringWithFormat:@"%d updates", [currentRun currentUpdate]]];
    [currentRun pauseAt:[currentRun currentUpdate] + 1];
    [currentRun resume];
  }
  
  // update interface
  [txtRun setStringValue:[NSString stringWithAptoString:freezer->NameOf(freezerID)]];
  [btnRunState setTitle:@"Run"];
  
  double mutrate = [currentRun mutationRate];
  [sldCfgMutRate setFloatValue:(mutrate == 0) ? [sldCfgMutRate minValue] : log10(mutrate)];
  [txtCfgMutRate setFloatValue:mutrate];
  NSSize worldsize = [currentRun worldSize];
  [txtCfgWorldX setIntValue:worldsize.width];
  [txtCfgWorldY setIntValue:worldsize.height];
  [matCfgPlacement selectCellWithTag:([currentRun placementMode] == 0) ? 0 : 1];
  [matCfgRepeatability selectCellWithTag:([currentRun randomSeed] == 0) ? 0 : 1];
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
}


- (void) clearCurrentRun {
  // Clear main listener
  [currentRun detachListener:self];
  delete listener;
  listener = NULL;
  
  // Clear map view
  [mapView clearMap];
  map = NULL;

  // Clear stats panel
  [popViewStatView clearAvidaRun];
  
  // End run
  [currentRun end];
  currentRun = nil;
}


- (void) freezeCurrentRun {
  [currentRun pause];
  
  // @TODO - fix this ugly busy wait
  while (![currentRun isPaused]);
  

  Apto::String name = freezer->NewUniqueNameForType(Avida::Viewer::WORLD, [[txtRun stringValue] UTF8String]);
  Avida::Viewer::FreezerID f = freezer->SaveWorld([currentRun oldworld], name);
  if (freezer->IsValid(f)) {
    // Save plot info
    [popViewStatView saveRunToFreezer:freezer withID:f];
    
    FreezerItem* fi = [[FreezerItem alloc] initWithFreezerID:f];
    [freezerWorlds addObject:fi];
    [outlineFreezer reloadData];
    [outlineFreezer editColumn:0 row:[outlineFreezer rowForItem:fi] withEvent:nil select:YES];
  }
}

- (void) freezeCurrentConfig {
  Apto::String name = freezer->NewUniqueNameForType(Avida::Viewer::CONFIG, [[txtRun stringValue] UTF8String]);
  Avida::Viewer::FreezerID f = freezer->SaveConfig([currentRun oldworld], name);
  if (freezer->IsValid(f)) {    
    FreezerItem* fi = [[FreezerItem alloc] initWithFreezerID:f];
    [freezerConfigs addObject:fi];
    [outlineFreezer reloadData];
    [outlineFreezer editColumn:0 row:[outlineFreezer rowForItem:fi] withEvent:nil select:YES];
  }
}

- (void) freezeGenome:(Genome*)genome {
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


- (void) activateRun {
  runActive = YES;
  [popViewStatView setAvidaRun:currentRun fromFreezer:freezer withID:Avida::Viewer::FreezerID()];
  [matCfgRepeatability setEnabled:NO];
  [txtCfgWorldX setEnabled:NO];
  [txtCfgWorldY setEnabled:NO];
  [txtUpdate setTextColor:[NSColor controlTextColor]];

}

@end

@implementation AvidaEDController

- (id) initWithAppDelegate:(AvidaAppDelegate*)delegate {
  self = [super initWithWindowNibName:@"Avida-ED-MainWindow"];
  
  if (self != nil) {
    app = delegate;

    currentRun = nil;
    listener = NULL;
    map = NULL;
        
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSArray* urls = [fileManager URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask];
    
    if ([urls count] == 0) return nil;
    
    NSURL* userDocumentsURL = [urls objectAtIndex:0];
    freezerURL = [NSURL URLWithString:@"default.avidaedworkspace" relativeToURL:userDocumentsURL];
    
    Apto::String freezer_path([[freezerURL path] cStringUsingEncoding:NSASCIIStringEncoding]);
    freezer = Avida::Viewer::FreezerPtr(new Avida::Viewer::Freezer(freezer_path));
    [self setupFreezer];

    // Hide freezer extension
    NSDictionary* fileAttrs = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:NSFileExtensionHidden];
    [fileManager setAttributes:fileAttrs ofItemAtPath:[freezerURL path] error:nil];
    
    
    [self showWindow:self];
  }
  
  return self;
}

- (id) initWithAppDelegate:(AvidaAppDelegate*)delegate inWorkspace:(NSURL*)dir {
  self = [super initWithWindowNibName:@"Avida-ED-MainWindow"];
  
  if (self != nil) {
    app = delegate;
    
    currentRun = nil;
    listener = NULL;
    map = NULL;
    
    freezerURL = dir;

    Apto::String freezer_path([[freezerURL path] cStringUsingEncoding:NSASCIIStringEncoding]);
    freezer = Avida::Viewer::FreezerPtr(new Avida::Viewer::Freezer(freezer_path));
    [self setupFreezer];
    
    // Hide freezer extension
    NSDictionary* fileAttrs = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:NSFileExtensionHidden];
    NSFileManager* fileManager = [NSFileManager defaultManager];
    [fileManager setAttributes:fileAttrs ofItemAtPath:[freezerURL path] error:nil];

    [self showWindow:self];
  }
  
  return self;
}


- (void) dealloc {
  delete listener;
  listener = NULL;
  [super dealloc];
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



- (void) windowDidLoad {
  [mainSplitView replaceSubview:[[mainSplitView subviews] objectAtIndex:1] with:popView];
  [btnRunState setTitle:@"Run"];
  
  
  // Replace NSClipView of mapView's scrollView with a CenteringClipView
  NSClipView* clipView = [[CenteringClipView alloc] initWithFrame:[mapScrollView frame]];
  [clipView setBackgroundColor:[NSColor darkGrayColor]];
  [mapScrollView setContentView:clipView];
  [mapScrollView setDocumentView:mapView];
  [mapScrollView setScrollsDynamically:YES];
  
  [outlineFreezer setDataSource:self];
  [outlineFreezer setDelegate:self];
  [outlineFreezer reloadData];
  [outlineFreezer expandItem:freezerConfigs];
  [outlineFreezer expandItem:freezerGenomes];
  [outlineFreezer expandItem:freezerWorlds];
  [outlineFreezer registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypePopulation, AvidaPasteboardTypeGenome, nil]];

  [pathWorkspace setURL:freezerURL];
  
  for (Avida::Viewer::Freezer::Iterator it = freezer->EntriesOfType(Avida::Viewer::CONFIG); it.Next();) {
    if (freezer->NameOf(*it.Get()) == "@default") {
      [self loadRunFromFreezer:(*it.Get())];
      break;
    }
  }
  
  [btnRunState becomeFirstResponder];
}


- (IBAction) toggleRunState:(id)sender {
  if ([currentRun willPause]) {
    if ([currentRun numOrganisms] == 0 && ![currentRun hasPendingInjects]) {
      NSAlert* alert = [[NSAlert alloc] init];
      [alert addButtonWithTitle:@"OK"];
      [alert setMessageText:@"Unable to resume experiment, the petri dish has not been inoculated."];
      [alert setInformativeText:@"Please drag an organisms from the freezer to inoculate the petri dish."];
      [alert setAlertStyle:NSWarningAlertStyle];
      [alert beginSheetModalForWindow:[sender window] modalDelegate:nil didEndSelector:nil contextInfo:nil];
      [sender setState:NSOffState];
      return;
    }
    
    if (runActive == NO) {
      [self activateRun];
    }

    [currentRun resume];
    [btnRunState setTitle:@"Pause"];
  } else {
    [currentRun pause];
    [btnRunState setTitle:@"Run"];
  }
}

- (IBAction) changeMapViewMode:(id)sender {
  if (map) {
    map->SetMode(map_mode_to_color[[mapViewMode indexOfSelectedItem]]);
    [mapView updateState:map];
    [mapScaleView updateState:map];
  }
  
}

- (IBAction) changeMapZoom:(id)sender {
  if (map) {
    [mapView setZoom:[mapZoom doubleValue]];
  }
}

- (IBAction) changeView:(id)sender {
  
  NSView* curView = [[mainSplitView subviews] objectAtIndex:1];
  if (sender == btnPopView) {
    if (curView != popView) {
      [mainSplitView replaceSubview:curView with:popView];
      curView = popView;
    }
    [btnPopView setState:NSOnState];
    [btnOrgView setState:NSOffState];
    [btnAnalyzeView setState:NSOffState];
  } else if (sender == btnOrgView) {
    if (curView != orgView) {
      [mainSplitView replaceSubview:curView with:orgView];
      curView = orgView;
    }
    [btnPopView setState:NSOffState];
    [btnOrgView setState:NSOnState];
    [btnAnalyzeView setState:NSOffState];
  } else if (sender == btnAnalyzeView) {
    if (curView != analyzeView) {
      [mainSplitView replaceSubview:curView with:analyzeView];
      curView = analyzeView;
    }
    [btnPopView setState:NSOffState];
    [btnOrgView setState:NSOffState];
    [btnAnalyzeView setState:NSOnState];
  }
}

- (IBAction) togglePopViewStatView:(id)sender {
  CGFloat dividerThickness = [popSplitView dividerThickness];
  
  if (popSplitViewIsAnimating) return;
  
  if ([sender state] == NSOnState) {
    // uncollapse
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
  } else {
    [btnMapSettingsFlip setTitle:@"Map"];
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
  [currentRun setWorldSize:NSMakeSize([txtCfgWorldX intValue], [txtCfgWorldY intValue])];
  [mapView setDimensions:[currentRun worldSize]];
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
    [currentRun setRandomSeed:0]; // full random
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
      enabled_actions += [key UTF8String];
    }
  }
  
  map->SetModeProperty(map->GetTagMode(), "enabled_actions", enabled_actions);
  [mapView updateState:map];
}


- (BOOL) validateMenuItem:(NSMenuItem*)item {
  if ([item action] == @selector(saveCurrentRun:) && runActive == NO) return NO;
  if ([item action] == @selector(saveSelectedOrganism:) && [popViewStatView selectedOrgGenome] == nil) return NO;
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
    
    int diffWidth = newFrame.size.width - oldSize.width;
    
    if ((leftFrame.size.width <= MAIN_SPLIT_LEFT_MIN && diffWidth < 0) || diffWidth > 0) {
      rightFrame.size.width += diffWidth;
      leftFrame.size.width = newFrame.size.width - rightFrame.size.width - dividerThickness;
    } else if (rightFrame.size.width <= MAIN_SPLIT_RIGHT_MIN && diffWidth < 0) {
      leftFrame.size.width += diffWidth;
      rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    } else {
      leftFrame.size.width += diffWidth * MAIN_SPLIT_LEFT_PROPORTIONAL_RESIZE;
      rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    }
    
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    rightFrame.size.height = newFrame.size.height;
    rightFrame.origin.x = leftFrame.size.width + dividerThickness;
    
    [leftView setFrame:leftFrame];
    [rightView setFrame:rightFrame];
  } else if (splitView == popSplitView) {
    NSView* leftView = [[splitView subviews] objectAtIndex:0];
    NSView* rightView = [[splitView subviews] objectAtIndex:1];
    NSRect newFrame = [splitView frame];
    NSRect leftFrame = [leftView frame];
    NSRect rightFrame = [rightView frame];
    
    CGFloat dividerThickness = [splitView dividerThickness];
    
    CGFloat diffWidth = floor(newFrame.size.width - oldSize.width);
    
    leftFrame.size.height = newFrame.size.height;
    leftFrame.origin = NSMakePoint(0, 0);
    if ((rightFrame.size.width <= POP_SPLIT_RIGHT_MIN && diffWidth < 0) || diffWidth > 0) {
      leftFrame.size.width += diffWidth;
    } else if (leftFrame.size.width > POP_SPLIT_LEFT_MIN) {
      leftFrame.size.width += floor(diffWidth * POP_SPLIT_LEFT_PROPORTIONAL_RESIZE);
    }
    
    rightFrame.size.width = newFrame.size.width - leftFrame.size.width - dividerThickness;
    rightFrame.size.height = newFrame.size.height;
    rightFrame.origin.x = leftFrame.size.width + dividerThickness;
    
    [leftView setFrame:leftFrame];
    [rightView setFrame:rightFrame];
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
  if (!popSplitViewIsAnimating) {
    if ([popSplitView isSubviewCollapsed:popViewStatView]) {
      [btnTogglePopViewStatView setState:NSOffState];
    } else {
      [btnTogglePopViewStatView setState:NSOnState];
    }
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
        [alert addButtonWithTitle:@"Config"];
        [alert addButtonWithTitle:@"Cancel"];
        [alert setMessageText:@"What would you like to save to the freezer?"];
        [alert setInformativeText:@"Population saves organisms and experiment history.\nConfig saves the experiment settings only."];
        [alert setAlertStyle:NSWarningAlertStyle];
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
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:AvidaPasteboardTypeGenome]] != nil) {
    [self freezeGenome:[Genome genomeFromPasteboard:[info draggingPasteboard]]];
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
  if ([[info draggingPasteboard] availableTypeFromArray:[NSArray arrayWithObject:AvidaPasteboardTypeGenome]] != nil) {
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
//    // @TODO handle genome
//  }
}




- (void) mapView:(MapGridView*)map handleDraggedConfig:(Avida::Viewer::FreezerID)fid
{
  [self loadRunFromFreezer:fid];
}

- (void) mapView:(MapGridView*)map handleDraggedFreezerGenome:(Avida::Viewer::FreezerID)fid atX:(int)x Y:(int)y
{
  Avida::GenomePtr genome(freezer->InstantiateGenome(fid));
  if (genome) {
    [currentRun injectGenome:genome atX:x Y:y];
    [mapView setPendingActionColorAtX:x Y:y];
  }
}

- (void) mapView:(MapGridView*)map handleDraggedGenome:(Genome*)genome atX:(int)x Y:(int)y
{
  Avida::GenomePtr genome_ptr(new Avida::Genome([[genome genomeStr] UTF8String]));
  if (genome_ptr) {
    [currentRun injectGenome:genome_ptr atX:x Y:y];
    [mapView setPendingActionColorAtX:x Y:y];
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




@synthesize listener;
@synthesize mapView;


- (void) handleMap:(ViewerMap*)pkg {
  if (!map) {
    map = [pkg map];
    [mapViewMode removeAllItems];
    map_mode_to_color.Clear();
    int idx = 0;
    for (int i = 0; i < map->GetNumModes(); i++) {
      if (!(map->GetModeSupportedTypes(i) & Avida::Viewer::MAP_GRID_VIEW_COLOR)) continue;
      [mapViewMode addItemWithTitle:[NSString stringWithUTF8String:(const char*)map->GetModeName(i)]];
      map_mode_to_color[idx++] = i;
    }
    [mapViewMode selectItemAtIndex:map->GetColorMode()];
    [mapViewMode setEnabled:TRUE];
    [mapZoom setEnabled:TRUE];
  } else {
    map = [pkg map];
  }
  [mapView updateState:map];
  [mapScaleView updateState:map];
  [mapZoom setDoubleValue:[mapView zoom]];
}


- (void) handleUpdate:(ViewerUpdate*)pkg {
  int update = [pkg update];
  if (update == 1) {
    [txtUpdate setStringValue:@"1 update"];
  } else {
    NSString* str = [NSString stringWithFormat:@"%d updates", update];
    [txtUpdate setStringValue:str]; 
  }
}


@end
