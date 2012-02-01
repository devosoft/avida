//
//  AvidaEDAnalyzePopulation.m
//  viewer-macos
//
//  Created by David Michael Bryson on 1/31/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "AvidaEDAnalyzePopulation.h"

#import "NSString+Apto.h"

@implementation AvidaEDAnalyzePopulation

- (AvidaEDAnalyzePopulation*) initWithFreezerID:(Avida::Viewer::FreezerID)fid fromFreezer:(Avida::Viewer::FreezerPtr)freezer {
  freezerIdentifier = fid.identifier;
  name = [NSString stringWithAptoString:freezer->NameOf(fid)];
  colorIndex = 0;
  
  xData = [[NSMutableArray alloc] initWithCapacity:4];
  yData = [[NSMutableArray alloc] initWithCapacity:4];
  
  for (int i = 0; i < 4; i++) {
    Apto::String loaded_data(freezer->LoadAttachment(fid, Apto::FormatStr("tr%d", i)));
    NSMutableArray* updates = [[NSMutableArray alloc] init];
    NSMutableArray* values = [[NSMutableArray alloc] init];
    while (loaded_data.GetSize()) {
      Apto::String entry_str = loaded_data.Pop(',');
      int update = Apto::StrAs(entry_str.Pop(':'));
      [updates addObject:[NSNumber numberWithInt:update]];
      double value = Apto::StrAs(entry_str);
      [values addObject:[NSNumber numberWithDouble:value]];
    }
    [xData addObject:updates];
    [yData addObject:values];
  }
  
  primaryPlot = [[[CPTScatterPlot alloc] init] autorelease];
  primaryPlot.identifier = [NSString stringWithFormat:@"p_fid%d", fid.identifier];
	primaryPlot.cachePrecision = CPTPlotCachePrecisionDouble;
  
  CPTMutableLineStyle* primaryPlotLineStyle = [[primaryPlot.dataLineStyle mutableCopy] autorelease];
	primaryPlotLineStyle.miterLimit = 1.0;
	primaryPlotLineStyle.lineWidth = 3.0;
	primaryPlotLineStyle.lineColor = [CPTColor redColor];
  primaryPlot.dataLineStyle = primaryPlotLineStyle;
  primaryPlot.dataSource = self;

  
  secondaryPlot = [[[CPTScatterPlot alloc] init] autorelease];
  secondaryPlot.identifier = [NSString stringWithFormat:@"s_fid%d", fid.identifier];
	secondaryPlot.cachePrecision = CPTPlotCachePrecisionDouble;
  
  CPTMutableLineStyle* secondaryPlotLineStyle = [[secondaryPlot.dataLineStyle mutableCopy] autorelease];
	secondaryPlotLineStyle.miterLimit = 1.0;
	secondaryPlotLineStyle.lineWidth = 3.0;
	secondaryPlotLineStyle.lineColor = [CPTColor redColor];
	secondaryPlotLineStyle.dashPattern = [NSArray arrayWithObjects:[NSNumber numberWithFloat:5.0f], [NSNumber numberWithFloat:5.0f], nil];
  secondaryPlot.dataLineStyle = secondaryPlotLineStyle;
  secondaryPlot.dataSource = self;
  
  return self;
}


@synthesize freezerIdentifier;
@synthesize name;
@synthesize primaryPlot;
@synthesize secondaryPlot;

- (NSInteger) colorIndex {
  return colorIndex;
}

- (void) setColorIndex:(NSInteger)_color {
  CPTColor* color = nil;
  switch (_color) {
    case 0: color = [CPTColor redColor]; break;
    case 1: color = [CPTColor blueColor]; break;
    case 2: color = [CPTColor greenColor]; break;
    case 3: color = [CPTColor cyanColor]; break;
    case 4: color = [CPTColor magentaColor]; break;
    case 5: color = [CPTColor purpleColor]; break;
    case 6: color = [CPTColor orangeColor]; break;
    case 7: color = [CPTColor brownColor]; break;
  }
  
  if (!color) return;

  colorIndex = _color;
  
  CPTMutableLineStyle* primaryPlotLineStyle = [[primaryPlot.dataLineStyle mutableCopy] autorelease];
  primaryPlotLineStyle.lineColor = color;
  primaryPlot.dataLineStyle = primaryPlotLineStyle;
  CPTMutableLineStyle* secondaryPlotLineStyle = [[secondaryPlot.dataLineStyle mutableCopy] autorelease];
	secondaryPlotLineStyle.lineColor = color;
  secondaryPlot.dataLineStyle = secondaryPlotLineStyle;
}


- (CPTMutableLineStyle*) primaryPlotLineStyle {
  return [primaryPlot.dataLineStyle mutableCopy];
}

- (void) setPrimaryPlotLineStyle:(CPTMutableLineStyle*)linestyle {
  primaryPlot.dataLineStyle = linestyle;
}


- (CPTMutableLineStyle*) secondaryPlotLineStyle {
  return [secondaryPlot.dataLineStyle mutableCopy];
}

- (void) setSecondaryPlotLineStyle:(CPTMutableLineStyle*)linestyle {
  secondaryPlot.dataLineStyle = linestyle;
}


- (NSInteger) primaryPlotData {
  return primaryPlotData;
}

- (void) setPrimaryPlotData:(NSInteger)ppd {
  primaryPlotData = ppd;
  [primaryPlot reloadData];
}


- (NSInteger) secondaryPlotData {
  return secondaryPlotData;
}

- (void) setSecondaryPlotData:(NSInteger)spd {
  secondaryPlotData = spd;
  [secondaryPlot reloadData];
}


- (NSUInteger) numberOfRecordsForPlot:(CPTPlot*)plot
{
  return [[xData objectAtIndex:(plot == primaryPlot) ? primaryPlotData : secondaryPlotData] count];
}

- (NSNumber*) numberForPlot:(CPTPlot*)plot field:(NSUInteger)fieldEnum recordIndex:(NSUInteger)index
{
  NSNumber* num;
  if (fieldEnum == CPTScatterPlotFieldX) {
    num = [[xData objectAtIndex:(plot == primaryPlot) ? primaryPlotData : secondaryPlotData] objectAtIndex:index];
  } else if (fieldEnum == CPTScatterPlotFieldY) {
    num = [[yData objectAtIndex:(plot == primaryPlot) ? primaryPlotData : secondaryPlotData] objectAtIndex:index];
  }
  
  return num;
}

@end
