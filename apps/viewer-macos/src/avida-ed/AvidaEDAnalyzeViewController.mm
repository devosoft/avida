//
//  AvidaEDAnalyzeViewController.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 5/17/12.
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

#import "AvidaEDAnalyzeViewController.h"

#import "AvidaEDAnalyzePopulation.h"

#include <fstream>


@interface AvidaEDAnalyzeViewController ()
- (void) viewDidLoad;

- (void) rescaleGraph;
@end

@implementation AvidaEDAnalyzeViewController

- (id) init {
  self = [super initWithNibName:@"AvidaED-AnalyzeView" bundle:nil];
  if (self) {
    popArray = [[NSMutableArray alloc] init];
  }
  return self;
}


- (void) loadView {
  [super loadView];
  [self viewDidLoad];
}


- (void) viewDidLoad {
  
  graphView.dropDelegate = dropDelegate;
  
  // Setup Graph
  graph = [[CPTXYGraph alloc] initWithFrame:NSRectToCGRect(graphView.bounds)];
	CPTTheme* theme = [CPTTheme themeNamed:kCPTPlainWhiteTheme];
  [graph applyTheme:theme];
  
  graph.fill = nil;
	graph.plotAreaFrame.fill = nil; 
	graph.plotAreaFrame.borderLineStyle = nil;
	
  graphView.hostedGraph = graph;
  
  graph.paddingLeft = 0.0;
  graph.paddingTop = 0.0;
  graph.paddingRight = 0.0;
  graph.paddingBottom = 0.0;
  graph.plotAreaFrame.paddingLeft = 65.0;
  graph.plotAreaFrame.paddingTop = 15.0;
  graph.plotAreaFrame.paddingRight = 65.0;
  graph.plotAreaFrame.paddingBottom = 45.0;
  
  
  // Setup scatter plot space
  CPTXYPlotSpace* plotSpace = (CPTXYPlotSpace*)graph.defaultPlotSpace;
  plotSpace.allowsUserInteraction = NO;
  
  // Grid line styles
  CPTMutableLineStyle *majorGridLineStyle = [CPTMutableLineStyle lineStyle];
  majorGridLineStyle.lineWidth = 0.75;
  majorGridLineStyle.lineColor = [[CPTColor colorWithGenericGray:0.2] colorWithAlphaComponent:0.65];
  
  CPTMutableLineStyle *minorGridLineStyle = [CPTMutableLineStyle lineStyle];
  minorGridLineStyle.lineWidth = 0.25;
  minorGridLineStyle.lineColor = [[CPTColor whiteColor] colorWithAlphaComponent:0.1];    
  
  
  primaryPlotSpace = (CPTXYPlotSpace*)graph.defaultPlotSpace;
  
  // Axes
  
  // Label x axis with a fixed interval policy
	CPTXYAxisSet* axisSet = (CPTXYAxisSet*)graph.axisSet;
  CPTMutableTextStyle* textStyle = [CPTMutableTextStyle textStyle];
  textStyle.fontName = @"Helvetica";
  textStyle.fontSize = 10;
  CPTMutableTextStyle* titleTextStyle = [CPTMutableTextStyle textStyle];
  titleTextStyle.fontName = @"Helvetica";
  titleTextStyle.fontSize = 12;
  
  CPTXYAxis* x = axisSet.xAxis;
  x.titleTextStyle = titleTextStyle;
  x.labelingPolicy = CPTAxisLabelingPolicyAutomatic;
  x.orthogonalCoordinateDecimal = CPTDecimalFromUnsignedInteger(0);
  x.minorTicksPerInterval = 5;
  x.preferredNumberOfMajorTicks = 5;
  x.labelOffset = 5.0;
  x.labelTextStyle = textStyle;
  x.majorGridLineStyle = majorGridLineStyle;
  //  x.minorGridLineStyle = minorGridLineStyle;
  
	x.title = @"Time (updates)";
	x.titleOffset = 25.0;
  [x.labelFormatter setMaximumFractionDigits:0];
  
  x.axisConstraints = [CPTConstraints constraintWithLowerOffset:0.0];
  
  
	// Label y with an automatic label policy. 
  CPTXYAxis *y = axisSet.yAxis;
  y.titleTextStyle = titleTextStyle;
  y.labelingPolicy = CPTAxisLabelingPolicyAutomatic;
  y.orthogonalCoordinateDecimal = CPTDecimalFromUnsignedInteger(0);
  y.minorTicksPerInterval = 2;
  y.preferredNumberOfMajorTicks = 8;
  y.labelOffset = 5.0;
  y.labelTextStyle = textStyle;
  y.majorGridLineStyle = majorGridLineStyle;

  
  y.title = @"Average Fitness";
  y.titleOffset = 50.0;
  [y.labelFormatter setMaximumFractionDigits:2];
  [y.labelFormatter setMinimumSignificantDigits:2];
  
	y.axisConstraints = [CPTConstraints constraintWithLowerOffset:0.0];
  primaryYAxis = y;
  
  
  secondaryPlotSpace = [[CPTXYPlotSpace alloc] init];
  
  CPTXYAxis* y2 = [[CPTXYAxis alloc] init];
  y2.titleTextStyle = titleTextStyle;
  y2.labelingPolicy = CPTAxisLabelingPolicyAutomatic;
  y2.coordinate = CPTCoordinateY;
  y2.plotSpace = secondaryPlotSpace;
  y2.minorTicksPerInterval = 2;
  y2.preferredNumberOfMajorTicks = 8;
  y2.labelOffset = -50.0;
  y2.labelTextStyle = textStyle;
  
  y2.titleOffset = -65.0;
  [y2.labelFormatter setMaximumFractionDigits:2];
  [y2.labelFormatter setMinimumSignificantDigits:2];
  
  // y2.axisConstraints = [CPTConstraints constraintWithLowerOffset:0.0];
  secondaryYAxis = y2;
  
  // [graph addPlotSpace:secondaryPlotSpace];
  
  
  // Set axes
  //graph.axisSet.axes = [NSArray arrayWithObjects:x, y, y2, nil];
  
  
  [btnGraphSelectLeft removeAllItems];
  [btnGraphSelectLeft setEnabled:NO];
  [btnGraphSelectLeft addItemWithTitle:@"Data Unavailable"];
  [btnGraphSelectRight removeAllItems];
  [btnGraphSelectRight setEnabled:NO];
  [btnGraphSelectRight addItemWithTitle:@"Data Unavailable"];
  
  [graphView registerForDraggedTypes:[NSArray arrayWithObjects:AvidaPasteboardTypeFreezerID, nil]];
}


- (void) setDropDelegate:(id<DropDelegate>)delegate {
  dropDelegate = delegate;
  if (graphView) [graphView setDropDelegate:delegate];
}


- (IBAction) changeGraphMode:(id)sender {
  NSInteger mode = [sender indexOfSelectedItem];
  bool primary = (sender == btnGraphSelectLeft);
  if (!primary && mode && ![[graph allPlotSpaces] containsObject:secondaryPlotSpace]) [graph addPlotSpace:secondaryPlotSpace];
  for (int i = 0; i < [popArray count]; i++) {
    if (primary) {
      if (mode) {
        [graph addPlot:[[popArray objectAtIndex:i] primaryPlot] toPlotSpace:primaryPlotSpace];
        [[popArray objectAtIndex:i] setPrimaryPlotData:mode - 1];
      } else {
        [graph removePlot:[[popArray objectAtIndex:i] primaryPlot]];
      }
    } else {
      if (mode) {
        graph.axisSet.axes = [NSArray arrayWithObjects:[(CPTXYAxisSet*)graph.axisSet xAxis], [(CPTXYAxisSet*)graph.axisSet yAxis], secondaryYAxis, nil];
        [graph addPlot:[[popArray objectAtIndex:i] secondaryPlot] toPlotSpace:secondaryPlotSpace];
        [[popArray objectAtIndex:i] setSecondaryPlotData:mode - 1];
      } else {
        [graph removePlot:[[popArray objectAtIndex:i] secondaryPlot]];
        graph.axisSet.axes = [NSArray arrayWithObjects:[(CPTXYAxisSet*)graph.axisSet xAxis], [(CPTXYAxisSet*)graph.axisSet yAxis], nil];
      }
    }
  }
  if (!primary && !mode) [graph removePlotSpace:secondaryPlotSpace];
  
  CPTXYAxis* axis = (primary) ? primaryYAxis : secondaryYAxis;
  switch (mode) {
    case 1: axis.title = @"Average Fitness"; break;
    case 2: axis.title = @"Average Gestation Time"; break;
    case 3: axis.title = @"Average Metabolic Rate"; break;
    case 4: axis.title = @"Number of Organism in the Population"; break;
    default: axis.title = @""; break;
  }

  
  [self rescaleGraph];
}


- (void) rescaleGraph {
  NSInteger primarySelected = [btnGraphSelectLeft indexOfSelectedItem];
  NSInteger secondarySelected = [btnGraphSelectRight indexOfSelectedItem];
  
  // Auto scale the plot space to fit the plot data
  NSMutableArray* activePrimaryPlots = [[NSMutableArray alloc] initWithCapacity:[popArray count]];
  NSMutableArray* activeSecondaryPlots = [[NSMutableArray alloc] initWithCapacity:[popArray count]];
  for (int i = 0; i < [popArray count]; i++) {
    if (primarySelected) [activePrimaryPlots addObject:[[popArray objectAtIndex:i] primaryPlot]];
    if (secondarySelected) [activeSecondaryPlots addObject:[[popArray objectAtIndex:i] secondaryPlot]];
  }
  [primaryPlotSpace scaleToFitPlots:activePrimaryPlots];
  [secondaryPlotSpace scaleToFitPlots:activeSecondaryPlots];
  
  NSDecimal end = [[primaryPlotSpace xRange] end];
  secondaryYAxis.orthogonalCoordinateDecimal = end;
}


- (void) addPop:(AvidaEDAnalyzePopulation*)pop {
  
  NSUInteger num_pops = [popArray count];
  if (num_pops == 0) {
    // Initialize menus
    [btnGraphSelectLeft removeAllItems];
    [btnGraphSelectLeft setEnabled:YES];
    [btnGraphSelectLeft addItemWithTitle:@"None"];
    [btnGraphSelectLeft addItemWithTitle:@"Average Fitness"];
    [btnGraphSelectLeft addItemWithTitle:@"Average Gestation Time"];
    [btnGraphSelectLeft addItemWithTitle:@"Average Metabolic Rate"];
    [btnGraphSelectLeft addItemWithTitle:@"Number of Organisms"];
    [btnGraphSelectLeft selectItemAtIndex:1];
    
    [btnGraphSelectRight removeAllItems];
    [btnGraphSelectRight setEnabled:YES];
    [btnGraphSelectRight addItemWithTitle:@"None"];
    [btnGraphSelectRight addItemWithTitle:@"Average Fitness"];
    [btnGraphSelectRight addItemWithTitle:@"Average Gestation Time"];
    [btnGraphSelectRight addItemWithTitle:@"Average Metabolic Rate"];
    [btnGraphSelectRight addItemWithTitle:@"Number of Organisms"];
  }
  
  // Initialize the color of the item to an unused color
  int color = 0;
  bool colorAvail[8] = { true, true, true, true, true, true, true, true };
  for (int i = 0; i < num_pops; i++) {
    colorAvail[[(AvidaEDAnalyzePopulation*)[popArray objectAtIndex:i] colorIndex]] = false;
  }
  for (int i = 0; i < 8; i++) {
    if (colorAvail[i]) {
      color = i; break;
    }
  }
  [pop setColorIndex:color];
  
  // Add population to the view via the array controller
  [popArrayCtlr addObject:pop];
  
  
  // Determine currently selected plots
  NSInteger primarySelected = [btnGraphSelectLeft indexOfSelectedItem];
  NSInteger secondarySelected = [btnGraphSelectRight indexOfSelectedItem];
  
  // Add appropriate plots
  if (primarySelected > 0) {
    [pop setPrimaryPlotData:primarySelected - 1];
    [graph addPlot:[pop primaryPlot] toPlotSpace:primaryPlotSpace];
  }
  if (secondarySelected > 0) {
    [pop setSecondaryPlotData:secondarySelected - 1];
    [graph addPlot:[pop secondaryPlot] toPlotSpace:secondaryPlotSpace];
  }
  
  [self rescaleGraph];
}

- (NSInteger) numPops {
  return [popArray count];
}

- (void) removePop:(id)pop {
  if ([btnGraphSelectLeft indexOfSelectedItem]) [graph removePlot:[pop primaryPlot]];
  if ([btnGraphSelectRight indexOfSelectedItem]) [graph removePlot:[pop secondaryPlot]];
  [popArrayCtlr removeObject:pop];
  
  if ([popArray count] == 0) {
    [btnGraphSelectLeft removeAllItems];
    [btnGraphSelectLeft setEnabled:NO];
    [btnGraphSelectLeft addItemWithTitle:@"Data Unavailable"];
    [btnGraphSelectRight removeAllItems];
    [btnGraphSelectRight setEnabled:NO];
    [btnGraphSelectRight addItemWithTitle:@"Data Unavailable"];
  }
  
  [self rescaleGraph];
}

- (BOOL) willAcceptPopWithFreezerID:(Avida::Viewer::FreezerID)fid {
  if ([popArray count] >= 3) return NO;
  
  for (int i = 0; i < [popArray count]; i++) {
    if ([[popArray objectAtIndex:i] freezerIdentifier] == fid.identifier) return NO;
  }
  
  return YES;
}


- (void) clearAllPops {
  NSArray* allPops = [popArrayCtlr arrangedObjects];
  for (AvidaEDAnalyzePopulation* pop in allPops) [self removePop:pop];
}



- (void) exportData:(NSArray*)dataValues toURL:(NSURL*)url {
  
  // Set up active recorder mapping
  Apto::Array<int> active_export_recorders;
  for (NSUInteger i = 0; i < [dataValues count]; i++) {
    NSString* dvstr = (NSString*)[dataValues objectAtIndex:i];
    if ([dvstr isEqualToString:@"core.world.ave_fitness"]) active_export_recorders.Push(0);
    else if ([dvstr isEqualToString:@"core.world.ave_gestation_time"]) active_export_recorders.Push(1);
    else if ([dvstr isEqualToString:@"core.world.ave_metabolic_rate"]) active_export_recorders.Push(2);
    else if ([dvstr isEqualToString:@"core.world.organisms"]) active_export_recorders.Push(3);
  }
  
  // Open file for writing
  std::ofstream ofile([[url path] cStringUsingEncoding:NSASCIIStringEncoding]);
  if (!ofile.good()) return;
  
  // Print header columns
  ofile << "Update";
  for (int pop_idx = 0; pop_idx < [popArray count]; pop_idx++) {
    for (int i = 0; i < active_export_recorders.GetSize(); i++) {
      ofile << "," << [[(AvidaEDAnalyzePopulation*)[popArray objectAtIndex:pop_idx] name] UTF8String] << " ";
      switch (active_export_recorders[i]) {
        case 0: ofile << "Average Fitness"; break;
        case 1: ofile << "Average Gestation Time"; break;
        case 2: ofile << "Average Metabolic Rate"; break;
        case 3: ofile << "Number of Organism in the Population"; break;
      }
    }
  }
  ofile << std::endl;
  
  // Print data rows
  NSArray* xValues = [(AvidaEDAnalyzePopulation*)[popArray objectAtIndex:0] xValuesForData:0];
  for (int i = 0; i < [xValues count]; i++) {
    ofile << [(NSNumber*)[xValues objectAtIndex:i] intValue];
    
    for (int pop_idx = 0; pop_idx < [popArray count]; pop_idx++) {
      AvidaEDAnalyzePopulation* cur_pop = [popArray objectAtIndex:pop_idx];
      for (int recorder_idx = 0; recorder_idx < active_export_recorders.GetSize(); recorder_idx++) {
        NSNumber* y_value = [[cur_pop yValuesForData:active_export_recorders[recorder_idx]] objectAtIndex:i];
        ofile << "," << [y_value doubleValue];
      }
    }
    ofile << std::endl;
  }
  
  // Close file
  ofile.close();

}


- (void) exportGraphic:(ExportGraphicsFileFormat)format toURL:(NSURL*)url {
  
  switch (format) {
    case EXPORT_GRAPHICS_JPEG:
    case EXPORT_GRAPHICS_PNG:
    {
      // Create imgContext
      NSRect exportRect = [self.view bounds];
      int bitmapBytesPerRow = 4 * exportRect.size.width;
      CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
      CGContextRef imgContext = CGBitmapContextCreate(NULL, exportRect.size.width, exportRect.size.height, 8, bitmapBytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);
      CGColorSpaceRelease(colorSpace);
      
      // Draw view into graphics imgContext
      NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:imgContext flipped:NO];
      [self.view displayRectIgnoringOpacity:[self.view bounds] inContext:gc];
      
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
      const CGRect pdfMediaBox = [self.view bounds];
      CGContextRef pdfContext = CGPDFContextCreate(dataConsumer, &pdfMediaBox, NULL);
      CGContextBeginPage(pdfContext, &pdfMediaBox);
      
      
      // Draw view into graphics pdfContext
      NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:pdfContext flipped:NO];
      [self.view displayRectIgnoringOpacity:[self.view bounds] inContext:gc];
      
      
      // The following will draw it into the PDF in vector format, but it isn't centered right and draws on top of the
      // bitmapped copy drawn above.  Needs work.
      
      // Draw graph into pdfContext
//      NSRect graphRect = [graph bounds];
//      
//      CGContextSaveGState(pdfContext);
//      
//      CGContextTranslateCTM(pdfContext, 0, pdfMediaBox.size.height - graphRect.size.height );
//      [graph layoutAndRenderInContext:pdfContext];
//      
//      CGContextRestoreGState(pdfContext);
      
      
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
  }
}

@end
