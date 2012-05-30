//
//  AvidaEDAnalyzeViewController.h
//  viewer-macos
//
//  Created by David Bryson on 5/17/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CorePlot/CorePlot.h>

#import "Definitions.h"
#import "DroppableGraphHostingView.h"
#import "Freezer.h"

@class AvidaEDAnalyzePopulation;


@interface AvidaEDAnalyzeViewController : NSViewController {
  NSMutableArray* popArray;
  
  IBOutlet NSArrayController* popArrayCtlr;
  
  IBOutlet NSPopUpButton* btnGraphSelectLeft;
  IBOutlet NSPopUpButton* btnGraphSelectRight;
  
  IBOutlet DroppableGraphHostingView* graphView;
  
  CPTXYGraph* graph;
  CPTXYPlotSpace* primaryPlotSpace;
  CPTXYPlotSpace* secondaryPlotSpace;
  CPTXYAxis* secondaryYAxis;
  
  id<DropDelegate> dropDelegate;
}

- (id) init;

- (void) setDropDelegate:(id<DropDelegate>)delegate;

- (IBAction) changeGraphMode:(id)sender;

- (void) addPop:(AvidaEDAnalyzePopulation*)pop;
- (void) removePop:(id)pop;
- (NSInteger) numPops;
- (BOOL) willAcceptPopWithFreezerID:(Avida::Viewer::FreezerID)fid;

- (void) exportData:(NSArray*)dataValues toURL:(NSURL*)url;
- (void) exportGraphic:(ExportGraphicsFileFormat)format toURL:(NSURL*)url;

@end
