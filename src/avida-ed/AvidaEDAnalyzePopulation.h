//
//  AvidaEDAnalyzePopulation.h
//  viewer-macos
//
//  Created by David Michael Bryson on 1/31/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CorePlot/CorePlot.h>

#include "avida/viewer.h"

@interface AvidaEDAnalyzePopulation : NSObject <CPTPlotDataSource> {
  int freezerIdentifier;
  NSString* name;
  NSInteger colorIndex;
  
  NSMutableArray* xData;
  NSMutableArray* yData;
  
  NSInteger primaryPlotData;
  CPTScatterPlot* primaryPlot;
  
  NSInteger secondaryPlotData;
  CPTScatterPlot* secondaryPlot;
}

- (AvidaEDAnalyzePopulation*) initWithFreezerID:(Avida::Viewer::FreezerID)fid fromFreezer:(Avida::Viewer::FreezerPtr)freezer;

@property (readonly) int freezerIdentifier;
@property (readonly) NSString* name;
@property (readwrite) NSInteger colorIndex;

@property (readonly) CPTPlot* primaryPlot;
@property (readonly) CPTPlot* secondaryPlot;

@property (readwrite, retain) CPTMutableLineStyle* primaryPlotLineStyle;
@property (readwrite, retain) CPTMutableLineStyle* secondaryPlotLineStyle;

@property (readwrite) NSInteger primaryPlotData;
@property (readwrite) NSInteger secondaryPlotData;

@end
