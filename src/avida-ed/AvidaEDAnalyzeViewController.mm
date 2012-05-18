//
//  AvidaEDAnalyzeViewController.m
//  viewer-macos
//
//  Created by David Bryson on 5/17/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "AvidaEDAnalyzeViewController.h"

#import "AvidaEDAnalyzePopulation.h"
#import "Freezer.h"

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
  graph.plotAreaFrame.paddingLeft = 50.0;
  graph.plotAreaFrame.paddingTop = 10.0;
  graph.plotAreaFrame.paddingRight = 50.0;
  graph.plotAreaFrame.paddingBottom = 45.0;
  
  
  // Setup scatter plot space
  CPTXYPlotSpace* plotSpace = (CPTXYPlotSpace*)graph.defaultPlotSpace;
  plotSpace.allowsUserInteraction = NO;
  
  // Grid line styles
  CPTMutableLineStyle *majorGridLineStyle = [CPTMutableLineStyle lineStyle];
  majorGridLineStyle.lineWidth = 0.75;
  majorGridLineStyle.lineColor = [[CPTColor colorWithGenericGray:0.2] colorWithAlphaComponent:0.75];
  
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
  titleTextStyle.fontSize = 11;
  
  CPTXYAxis* x = axisSet.xAxis;
  x.titleTextStyle = titleTextStyle;
  x.labelingPolicy = CPTAxisLabelingPolicyAutomatic;
  x.orthogonalCoordinateDecimal = CPTDecimalFromUnsignedInteger(0);
  x.minorTicksPerInterval = 5;
  x.preferredNumberOfMajorTicks = 5;
  x.labelOffset = 5.0;
  x.labelTextStyle = textStyle;
  //  x.majorGridLineStyle = majorGridLineStyle;
  //  x.minorGridLineStyle = minorGridLineStyle;
  
	x.title = @"Updates";
	x.titleOffset = 25.0;
  
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
  
	y.axisConstraints = [CPTConstraints constraintWithLowerOffset:0.0];
  
  
  
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
  //	y2.axisConstraints = [CPTConstraints constraintWithLowerOffset:0.0];
  secondaryYAxis = y2;
  
  [graph addPlotSpace:secondaryPlotSpace];
  
  
  // Set axes
  graph.axisSet.axes = [NSArray arrayWithObjects:x, y, y2, nil];
  
  
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
        [graph addPlot:[[popArray objectAtIndex:i] secondaryPlot] toPlotSpace:secondaryPlotSpace];
        [[popArray objectAtIndex:i] setSecondaryPlotData:mode - 1];
      } else {
        [graph removePlot:[[popArray objectAtIndex:i] secondaryPlot]];
      }
    }
  }
  
  [self rescaleGraph];
}


- (void) rescaleGraph {
  NSInteger primarySelected = [btnGraphSelectLeft indexOfSelectedItem];
  NSInteger secondarySelected = [btnGraphSelectRight indexOfSelectedItem];
  
  // Auto scale the plot space to fit the plot data
  NSMutableArray* activePrimaryPlots = [[[NSMutableArray alloc] initWithCapacity:[popArray count]] autorelease];
  NSMutableArray* activeSecondaryPlots = [[[NSMutableArray alloc] initWithCapacity:[popArray count]] autorelease];
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

@end
