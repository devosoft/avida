//
//  AvidaEDAnalyzePopulation.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 1/31/12.
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

#import "AvidaEDAnalyzePopulation.h"

#import <AvidaCore/AvidaCore.h>


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
  
  primaryPlot = [[CPTScatterPlot alloc] init];
  primaryPlot.identifier = [NSString stringWithFormat:@"p_fid%d", fid.identifier];
	primaryPlot.cachePrecision = CPTPlotCachePrecisionDouble;
  
  CPTMutableLineStyle* primaryPlotLineStyle = [primaryPlot.dataLineStyle mutableCopy];
	primaryPlotLineStyle.miterLimit = 1.0;
	primaryPlotLineStyle.lineWidth = 3.0;
	primaryPlotLineStyle.lineColor = [CPTColor redColor];
  primaryPlot.dataLineStyle = primaryPlotLineStyle;
  primaryPlot.dataSource = self;

  
  secondaryPlot = [[CPTScatterPlot alloc] init];
  secondaryPlot.identifier = [NSString stringWithFormat:@"s_fid%d", fid.identifier];
	secondaryPlot.cachePrecision = CPTPlotCachePrecisionDouble;
  
  CPTMutableLineStyle* secondaryPlotLineStyle = [secondaryPlot.dataLineStyle mutableCopy];
	secondaryPlotLineStyle.miterLimit = 1.0;
	secondaryPlotLineStyle.lineWidth = 1.0;
	secondaryPlotLineStyle.lineColor = [CPTColor redColor];
  secondaryPlotLineStyle.dashPattern = [NSArray arrayWithObjects:[NSNumber numberWithFloat:3.0f], [NSNumber numberWithFloat:5.0f], nil];
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
  
  CPTMutableLineStyle* primaryPlotLineStyle = [primaryPlot.dataLineStyle mutableCopy];
  primaryPlotLineStyle.lineColor = color;
  primaryPlot.dataLineStyle = primaryPlotLineStyle;
  CPTMutableLineStyle* secondaryPlotLineStyle = [secondaryPlot.dataLineStyle mutableCopy];
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

- (NSArray*) xValuesForData:(NSInteger)dataIndex {
  return [xData objectAtIndex:dataIndex];
}

- (NSArray*) yValuesForData:(NSInteger)dataIndex {
  return [yData objectAtIndex:dataIndex];
}

@end
