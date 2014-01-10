//
//  AvidaEDPopViewStatView.mm
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 4/15/11.
//  Copyright 2011-2012 Michigan State University. All rights reserved.
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

#import "AvidaEDPopViewStatView.h"

#import <AvidaCore/AvidaCore.h>

#import "AvidaRun.h"
#import "AvidaEDController.h"
#import "AvidaEDEnvActionsDataSource.h"

#include "avida/core/Properties.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Manager.h"
#include "avida/environment/Product.h"

#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"

#include "avida/viewer/ClassificationInfo.h"

#include <fstream>

static const float PANEL_MIN_WIDTH = 360.0;
static const int MAX_GRAPH_POINTS = 1000;



@interface AvidaEDPopViewStatViewGraphData : NSObject <CPTPlotDataSource> {
  Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject> recorder;
}

- (Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>) recorder;
- (void) setRecorder:(Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>)in_recorder;
@end


@implementation AvidaEDPopViewStatViewGraphData
- (NSUInteger) numberOfRecordsForPlot:(CPTPlot*)plot
{
  if (recorder) {
    int num_points = recorder->NumPoints();
    int div_factor = 1;
    if (num_points > MAX_GRAPH_POINTS) div_factor = num_points / MAX_GRAPH_POINTS;
    
    return num_points / div_factor;
  }
  return 0;
}

- (NSNumber*) numberForPlot:(CPTPlot*)plot field:(NSUInteger)fieldEnum recordIndex:(NSUInteger)index
{
  assert(recorder);
  
  int num_points = recorder->NumPoints();
  int div_factor = 1;
  if (num_points > MAX_GRAPH_POINTS) div_factor = num_points / MAX_GRAPH_POINTS;
  
  NSNumber* num;
  if (fieldEnum == CPTScatterPlotFieldX) {
    num = [NSNumber numberWithDouble:recorder->DataTime(static_cast<int>(index) * div_factor)];
  } else if (fieldEnum == CPTScatterPlotFieldY) {
    num = [NSNumber numberWithDouble:recorder->DataPoint(static_cast<int>(index) * div_factor)];
  }

  return num;
}

- (Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>) recorder {
  return recorder;
}

- (void) setRecorder:(Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>)in_recorder
{
  recorder = in_recorder;
}
@end




@interface AvidaEDPopViewStatViewValues : NSObject {
@public
  int organisms;
  double ave_fitness;
  double ave_metabolic_rate;
  double ave_gestation_time;
  double ave_age;
}
@end;
@implementation AvidaEDPopViewStatViewValues
@end


@interface AvidaEDPopViewStatViewOrgValues : NSObject {
@public
  Avida::Update update;
  int genotype_id;
  int clade_id;
  int x;
  int y;
}
@end;
@implementation AvidaEDPopViewStatViewOrgValues
@end



@interface AvidaEDPopViewStatView (hidden) <NSTableViewDelegate>
- (void) setup;

- (void) rescalePlot;

- (void) tableView:(NSTableView*)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex;
- (void) envActionStateChange:(NSMutableDictionary*)newState;

@property (readonly) AvidaEDEnvActionsDataSource* envActions;
@end

@implementation AvidaEDPopViewStatView (hidden)
@dynamic envActions;
- (AvidaEDEnvActionsDataSource*)envActions {
  return envActions;
}

- (void) setup {
  envActions = [[AvidaEDEnvActionsDataSource alloc] init];
  [tblEnvActions setDataSource:envActions];
  [tblEnvActions setDelegate:self];
  [tblEnvActions reloadData];

  orgEnvActions = [[AvidaEDEnvActionsDataSource alloc] init];
  [tblOrgEnvActions setDataSource:orgEnvActions];
  [tblOrgEnvActions setDelegate:self];
  [tblOrgEnvActions reloadData];

  NSNumberFormatter* fitFormat = [[NSNumberFormatter alloc] init];
  [fitFormat setNumberStyle:NSNumberFormatterDecimalStyle];
  [fitFormat setMaximumFractionDigits:4];
  [txtFitness setFormatter:fitFormat];
  [txtOrgFitness setFormatter:fitFormat];
  
  NSNumberFormatter* dec1format = [[NSNumberFormatter alloc] init];
  [dec1format setNumberStyle:NSNumberFormatterDecimalStyle];
  [dec1format setPositiveFormat:@"#0.0"];
  [dec1format setNegativeFormat:@"-#0.0"];
  [txtMetabolicRate setFormatter:dec1format];
  [txtGestation setFormatter:dec1format];
  [txtOrgMetabolicRate setFormatter:dec1format];
  [txtOrgGestation setFormatter:dec1format];
  
  NSNumberFormatter* dec2format = [[NSNumberFormatter alloc] init];
  [dec2format setNumberStyle:NSNumberFormatterDecimalStyle];
  [dec2format setPositiveFormat:@"#0.00"];
  [dec2format setNegativeFormat:@"-#0.00"];
  [txtAge setFormatter:dec2format];
  [txtOrgAge setFormatter:dec2format];
  
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
  graph.plotAreaFrame.paddingLeft = 60.0;
  graph.plotAreaFrame.paddingTop = 5.0;
  graph.plotAreaFrame.paddingRight = 15.0;
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
  
  
  // Axes

  // Label x axis with a fixed interval policy
	CPTXYAxisSet* axisSet = (CPTXYAxisSet *)graph.axisSet;
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
  x.preferredNumberOfMajorTicks = 6;
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
  y.majorGridLineStyle = majorGridLineStyle;
  y.preferredNumberOfMajorTicks = 8;
  y.labelOffset = 5.0;
  y.labelTextStyle = textStyle;
  [y.labelFormatter setMaximumFractionDigits:2];
  [y.labelFormatter setMinimumSignificantDigits:2];

	y.axisConstraints = [CPTConstraints constraintWithLowerOffset:0.0];
  
  // Set axes
  graph.axisSet.axes = [NSArray arrayWithObjects:x, y, nil];
  
  
  
  graphData = [[AvidaEDPopViewStatViewGraphData alloc] init];
  
 
  // Create plot that uses the graphData data source
	CPTScatterPlot *dataSourceLinePlot = [[CPTScatterPlot alloc] init];
  dataSourceLinePlot.identifier = @"graph";
	dataSourceLinePlot.cachePrecision = CPTPlotCachePrecisionDouble;
  
  CPTMutableLineStyle* lineStyle = [dataSourceLinePlot.dataLineStyle mutableCopy];
	lineStyle.miterLimit = 1.0;
	lineStyle.lineWidth = 3.0;
	lineStyle.lineColor = [CPTColor blueColor];
  dataSourceLinePlot.dataLineStyle = lineStyle;
  
  dataSourceLinePlot.dataSource = graphData;
  
  [graph addPlot:dataSourceLinePlot];
    
  // Put an area gradient under the plot above
  CPTColor* areaColor = [CPTColor colorWithComponentRed:0.3 green:0.3 blue:0.3 alpha:0.8];
  CPTGradient* areaGradient = [CPTGradient gradientWithBeginningColor:areaColor endingColor:[CPTColor clearColor]];
  areaGradient.angle = -90.0;
  CPTFill* areaGradientFill = [CPTFill fillWithGradient:areaGradient];
  dataSourceLinePlot.areaFill = areaGradientFill;
  dataSourceLinePlot.areaBaseValue = CPTDecimalFromString(@"0");
	
  
  [self rescalePlot];
  
  [btnGraphSelect removeAllItems];
  [btnGraphSelect setEnabled:YES];
  [btnGraphSelect addItemWithTitle:@"Average Fitness"];
  [btnGraphSelect addItemWithTitle:@"Average Gestation Time"];
  [btnGraphSelect addItemWithTitle:@"Average Metabolic Rate"];
  [btnGraphSelect addItemWithTitle:@"Number of Organisms"];
}


- (void) rescalePlot {
  
  Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject> curRecorder = graphData.recorder;
  
  const Avida::Update x_scale_constant = 50;
  
  CPTMutablePlotRange* xrange = [[(CPTXYPlotSpace*)graph.defaultPlotSpace xRange] mutableCopy];
  int min_x = run.initialUpdate;
  Avida::Update max_x = min_x + x_scale_constant;
  if (curRecorder && curRecorder->NumPoints() > 0) {
    max_x = curRecorder->DataTime(timeRecorders[0]->NumPoints() - 1);
  }
  int new_length = (max_x / x_scale_constant);
  if ((max_x % x_scale_constant) != 0) new_length++;
  new_length *= x_scale_constant;
  
  
  NSDecimalNumber* xloc = [[NSDecimalNumber alloc] initWithInt:min_x];
  [xrange setLocation:[xloc decimalValue]];
  NSDecimalNumber* xlen = [[NSDecimalNumber alloc] initWithInt:new_length];
  [xrange setLength:[xlen decimalValue]];
  [graph.defaultPlotSpace setPlotRange:xrange forCoordinate:CPTCoordinateX];
  
  CPTMutablePlotRange* yrange = [[(CPTXYPlotSpace*)graph.defaultPlotSpace yRange] mutableCopy];
  double max_y = 1.0;
  if (curRecorder) {
    for (int i = 0; i < curRecorder->NumPoints(); i++) {
      if (max_y < curRecorder->DataPoint(i)) {
        while (max_y < curRecorder->DataPoint(i)) max_y *= 2.0;
      }
    }
  }
  [yrange setLength:[[[NSDecimalNumber alloc] initWithDouble:max_y] decimalValue]];
  [graph.defaultPlotSpace setPlotRange:yrange forCoordinate:CPTCoordinateY];
  
  //[graph.defaultPlotSpace scaleToFitPlots:[NSArray arrayWithObjects:plot, nil]];  
}

- (void) envActionStateChange:(NSMutableDictionary*)newState {
  if ([envActionChangeDelegate respondsToSelector:@selector(envActionStateChange:)]) {
    [envActionChangeDelegate performSelector:@selector(envActionStateChange:) withObject:newState];
  }
}



- (void) tableView:(NSTableView*)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex
{
  if (tableView == tblEnvActions && [[tableColumn identifier] isEqualToString:@"State"]) {
    [cell setTitle:[envActions entryAtIndex:rowIndex]];
  }
}

@end


@implementation AvidaEDPopViewStatView

@synthesize graphView;

- (id)initWithFrame:(NSRect)frame
{
  self = [super initWithFrame:frame];
  if (self) {
    [self resizeSubviewsWithOldSize:frame.size];
    [self setup];
  }

  
  return self;
}

- (void)awakeFromNib {
  NSSize bounds_size = [self bounds].size;
  bounds_size.width++;
  
  [self resizeSubviewsWithOldSize:bounds_size];
  [self setup];
}

- (void)dealloc
{
  if (recorder) {
    assert(run);
    [run detachRecorder:recorder];
  }
}

- (void)finalize
{
  [super finalize];
  if (recorder) {
    assert(run);
    [run detachRecorder:recorder];
  }
}


- (BOOL)isFlipped {
  return YES;
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldBoundsSize {
  const CGFloat spacing = 7.0;
  NSRect bounds = [self bounds];
  
  if (bounds.size.width != oldBoundsSize.width && bounds.size.width >= PANEL_MIN_WIDTH) {
    CGFloat stat_panel_width = floor((bounds.size.width - 3 * spacing) / 2.0);

    NSRect panel_bounds;
    panel_bounds.size.width = stat_panel_width;
    panel_bounds.size.height = 322.0;
    panel_bounds.origin.x = spacing;
    panel_bounds.origin.y = spacing;
    [orgStatsView setFrame:panel_bounds];
    [orgStatsView setNeedsDisplay:YES];
    
    panel_bounds.size.width = stat_panel_width;
    panel_bounds.origin.x = 2.0 * spacing + stat_panel_width;
    [popStatsView setFrame:panel_bounds];
    [popStatsView setNeedsDisplay:YES];
  }
  
  if (bounds.size.width != oldBoundsSize.width || bounds.size.height != oldBoundsSize.height) {
    NSRect panel_bounds;
    
    CGFloat graph_height = floor(bounds.size.height - 322.0 - (2 * spacing) - 22.0);
    panel_bounds.size = [btnGraphSelect bounds].size;
    panel_bounds.origin.x = floor((bounds.size.width - panel_bounds.size.width) / 2.0) + 17.0;
    panel_bounds.origin.y = 322 + spacing + graph_height;
    [btnGraphSelect setFrame:panel_bounds];
    [btnGraphSelect setNeedsDisplay:YES];
    
    panel_bounds.size.width = floor(bounds.size.width - 2 * spacing);
    panel_bounds.size.height = graph_height;
    panel_bounds.origin.x = spacing;
    panel_bounds.origin.y = 322.0 + spacing;
    [graphView setFrame:panel_bounds];
    [graphView setNeedsDisplay:YES];
  }
}

- (void) setAvidaRun:(AvidaRun*)avidarun fromFreezer:(Avida::Viewer::FreezerPtr)freezer withID:(Avida::Viewer::FreezerID)fid {
  [self clearAvidaRun];
  run = avidarun;

  Avida::Environment::ManagerPtr env = Avida::Environment::Manager::Of([avidarun world]);
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
    [orgEnvActions addNewEntry:entryName withDescription:entryDesc withOrder:action->TempOrdering()];
  }
  [tblEnvActions reloadData];
  [tblOrgEnvActions reloadData];

  recorder = Avida::Data::RecorderPtr(new AvidaEDPopViewStatViewRecorder(self));
  [run attachRecorder:recorder];
  
  
  Apto::String loaded_data;
  timeRecorders.ResizeClear(4);
//  loaded_data = freezer->LoadAttachment(fid, "tr0");
  timeRecorders[0] = Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>(new AvidaEDPopViewStatViewTimeRecorder(self, "core.world.ave_fitness", loaded_data));
//  loaded_data = freezer->LoadAttachment(fid, "tr1");
  timeRecorders[1] = Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>(new AvidaEDPopViewStatViewTimeRecorder(self, "core.world.ave_gestation_time", loaded_data));
//  loaded_data = freezer->LoadAttachment(fid, "tr2");
  timeRecorders[2] = Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>(new AvidaEDPopViewStatViewTimeRecorder(self, "core.world.ave_metabolic_rate", loaded_data));
//  loaded_data = freezer->LoadAttachment(fid, "tr3");
  timeRecorders[3] = Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>(new AvidaEDPopViewStatViewTimeRecorder(self, "core.world.organisms", loaded_data));
  
  for (int i = 0; i < timeRecorders.GetSize(); i++) [run attachRecorder:timeRecorders[i]];
  
  [self changeGraph:self];
}


- (void) saveRunToFreezer:(Avida::Viewer::FreezerPtr)freezer withID:(Avida::Viewer::FreezerID)fid
{
  freezer->SaveAttachment(fid, "tr0", timeRecorders[0]->AsString());
  freezer->SaveAttachment(fid, "tr1", timeRecorders[1]->AsString());
  freezer->SaveAttachment(fid, "tr2", timeRecorders[2]->AsString());
  freezer->SaveAttachment(fid, "tr3", timeRecorders[3]->AsString());  
}


- (void) clearAvidaRun {
  if (recorder) {
    assert(run);
    [run detachRecorder:recorder];
    recorder = Avida::Data::RecorderPtr(NULL);
  }
  for (int i = 0; i < timeRecorders.GetSize(); i++) {
    [run detachRecorder:timeRecorders[i]];
    timeRecorders[i] = Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>(NULL);
  }
  timeRecorders.Resize(0);
  run = nil;
  
  NSString* empty_str = @"-";
  [txtPopSize setStringValue:empty_str];
  [txtFitness setStringValue:empty_str];
  [txtMetabolicRate setStringValue:empty_str];
  [txtGestation setStringValue:empty_str];
  [txtAge setStringValue:empty_str];
  [envActions clearEntries];
  [tblEnvActions reloadData];
  
  [self clearSelectedOrg];
  
  [graphData setRecorder:Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject>(NULL)];
  [[graph plotWithIdentifier:@"graph"] reloadData];
}

- (void) clearSelectedOrg {
  NSString* empty_str = @"-";
  [txtOrgName setStringValue:empty_str];
  [txtOrgFitness setStringValue:empty_str];
  [txtOrgMetabolicRate setStringValue:empty_str];
  [txtOrgGestation setStringValue:empty_str];
  [txtOrgAge setStringValue:empty_str];
  [txtOrgAncestor setStringValue:empty_str];  
  [orgEnvActions clearEntries];
  [tblOrgEnvActions reloadData];
  [boxOrgColor reset];
  genome = "";
}


- (BOOL) mapView:(MapGridView*)mapView shouldSelectObjectAtPoint:(NSPoint)point {
  return YES;
}


- (void) mapViewSelectionChanged:(MapGridView*)mapView {
  if (org_recorder) {
    [run detachRecorder:org_recorder];
  } else {
    org_recorder = Apto::SmartPtr<AvidaEDPopViewStatViewOrgRecorder, Apto::InternalRCObject>(new AvidaEDPopViewStatViewOrgRecorder(self));
  }
  org_recorder->SetCoords([mapView selectedObject].x, [mapView selectedObject].y);
  [run attachRecorder:org_recorder concurrentUpdate:YES];
}

- (void) mapViewModeChanged:(MapGridView*)mapView {
  if (genome != "") {
    [boxOrgColor setColor:[mapView colorOfX:selectedOrgCoords.x Y:selectedOrgCoords.y]];
  }
}


- (BOOL) mapView:(MapGridView*)mapView writeSelectionToPasteboard:(NSPasteboard*)pboard {
  if (genome == "") return NO;
  
  [ACGenome writeGenome:[[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:genome] name:[txtOrgName stringValue]] toPasteboard:pboard];
  
  return YES;
}


- (NSDragOperation) draggingSession:(NSDraggingSession*)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context {
  switch (context) {
    case NSDraggingContextWithinApplication:
      return NSDragOperationCopy;
      
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



- (void) handleData:(AvidaEDPopViewStatViewValues*)values {
  [txtPopSize setIntegerValue:values->organisms];
  [txtFitness setStringValue:[NSString stringWithFormat:@"%0.2f", values->ave_fitness]];
  [txtMetabolicRate setDoubleValue:values->ave_metabolic_rate];
  [txtGestation setDoubleValue:values->ave_gestation_time];
  [txtAge setDoubleValue:values->ave_age];
  [tblEnvActions reloadData];
}

- (void) handleOrgData:(AvidaEDPopViewStatViewOrgValues*)values {
  if (run == nil) return;
  
  Avida::World* world = [run world];
  Avida::Systematics::ArbiterPtr g_arb = Avida::Systematics::Manager::Of(world)->ArbiterForRole("genotype");
  Avida::Systematics::GroupPtr genotype = g_arb->Group(values->genotype_id);

  if (!genotype) {
    NSString* empty_str = @"-";
    [txtOrgName setStringValue:empty_str];
    [txtOrgFitness setStringValue:empty_str];
    [txtOrgMetabolicRate setStringValue:empty_str];
    [txtOrgGestation setStringValue:empty_str];
    [txtOrgAge setStringValue:empty_str];
    [txtOrgAncestor setStringValue:empty_str];  
    for (NSUInteger i = 0; i < [orgEnvActions entryCount]; i++) {
      [orgEnvActions updateEntry:[orgEnvActions entryAtIndex:i] withValue:[NSNumber numberWithInt:0]];
    }
    [tblOrgEnvActions reloadData];
    [boxOrgColor reset];
    genome = "";
    return;
  }
  
  [txtOrgName setStringValue:[NSString stringWithAptoString:((Apto::String)genotype->Properties().Get("name")).Substring(4)]];
  
  if (genotype->Properties().Get("total_gestation_count").IntValue() > 0) {
    [txtOrgFitness setStringValue:[NSString stringWithFormat:@"%0.2f",(double)genotype->Properties().Get("ave_fitness")]];
    [txtOrgMetabolicRate setDoubleValue:genotype->Properties().Get("ave_metabolic_rate")];
    [txtOrgGestation setDoubleValue:genotype->Properties().Get("ave_gestation_time")];
  } else {
    [txtOrgFitness setStringValue:[NSString stringWithFormat:@"est. %0.2f",[run testFitnessOfGroup:genotype]]];
    [txtOrgMetabolicRate setStringValue:[NSString stringWithFormat:@"est. %0.1f",[run testMetabolicRateOfGroup:genotype]]];
    [txtOrgGestation setStringValue:[NSString stringWithFormat:@"est. %0.1f",[run testGestationTimeOfGroup:genotype]]];
  }
  Avida::Update cur_update = values->update;
  if (cur_update == Avida::UPDATE_CONCURRENT) {
    cur_update = ctlr.curUpdate;
  }
  [txtOrgAge setIntValue:(cur_update - genotype->Properties().Get("update_born").IntValue())];
  
  // Set the name of the parent genotype
  Avida::Systematics::ArbiterPtr c_arb = Avida::Systematics::Manager::Of(world)->ArbiterForRole("clade");
  Avida::Systematics::GroupPtr clade = c_arb->Group(values->clade_id);
  if (clade) {
    [txtOrgAncestor setStringValue:[NSString stringWithAptoString:(clade->Properties().Get("name").StringValue())]];
  } else {
    [txtOrgAncestor setStringValue:@"-"];
  }
  
//  Apto::String parents(genotype->Properties().Get("parents").StringValue());
//  parents = parents.Pop(','); // extracts the first parent only
//  if (parents.GetSize()) {
//    Avida::Systematics::GroupPtr parent_genotype = g_arb->Group(Apto::StrAs(parents));
//    [txtOrgAncestor setStringValue:[NSString stringWithAptoString:((Apto::String)parent_genotype->Properties().Get("name")).Substring(4)]];
//  } else {
//    [txtOrgAncestor setStringValue:@"-"];
//  }
  
  // Set box color based on color from the map object
  selectedOrgCoords.x = values->x;
  selectedOrgCoords.y = values->y;
  [boxOrgColor setColor:[[ctlr mapView] colorOfX:values->x Y:values->y]];

  // Update the data source for the organism environment actions
  for (NSUInteger i = 0; i < [orgEnvActions entryCount]; i++) {
//    NSString* entry_name = [orgEnvActions entryAtIndex:i];
//    Apto::String data_id("environment.triggers.");
//    data_id += [entry_name UTF8String];
//    data_id += ".average";
//    double count = genotype->Properties().Get(data_id);
//    [orgEnvActions updateEntry:entry_name withValue:[NSNumber numberWithInt:round(count)]];
    NSString* entry_name = [orgEnvActions entryAtIndex:i];
    NSString* local_name = entry_name;
    if ([entry_name isEqual:@"oro"]) {
      entry_name = @"or";
    } else if ([entry_name isEqual:@"ant"]) {
      entry_name = @"andn";
    } else if ([entry_name isEqual:@"nan"]) {
      entry_name = @"nand";
    }
    int count = [run testEnvironmentTriggerCountFor:entry_name ofGroup:genotype];
    [orgEnvActions updateEntry:local_name withValue:[NSNumber numberWithInt:count]];
  }
  [tblOrgEnvActions reloadData];
  
  genome = genotype->Properties().Get("genome");
}


- (IBAction) changeGraph:(id)sender {
  if (graphData.recorder) graphData.recorder->SetInactive();
  if (timeRecorders.GetSize()) {
    [graphData setRecorder:timeRecorders[static_cast<int>([btnGraphSelect indexOfSelectedItem])]];
    graphData.recorder->SetActive();
    
    CPTPlot* plot = [graph plotWithIdentifier:@"graph"];
    [plot reloadData];
    [self rescalePlot];
  }
}

- (void) handleNewGraphData {
  graphData.recorder->LockForDisplay();
  CPTPlot* plot = [graph plotWithIdentifier:@"graph"];
  [plot reloadData];
  graphData.recorder->Unlock();
  [self rescalePlot];
}


- (ACGenome*) selectedOrgGenome {
  if (genome == "") return nil;
  return [[ACGenome alloc] initWithGenome:[NSString stringWithAptoString:genome] name:[txtOrgName stringValue]];
}


- (void) exportData:(NSArray*)dataValues toURL:(NSURL*)url {
  
  // Set up active recorder mapping
  Apto::Array<int> active_export_recorders;
  for (NSUInteger i = 0; i < [dataValues count]; i++) {
    Avida::Data::DataID data_id([(NSString*)[dataValues objectAtIndex:i] UTF8String]);
    for (int j = 0; j < timeRecorders.GetSize(); j++) {
      if (timeRecorders[j]->RecordedDataID() == data_id) {
        active_export_recorders.Push(j);
        break;
      }
    }
  }
  
  // Open file for writing
  std::ofstream ofile([[url path] cStringUsingEncoding:NSASCIIStringEncoding]);
  if (!ofile.good()) return;
  
  // Print header columns
  ofile << "Update";
  const char* runName = [[ctlr runName] UTF8String];
  Avida::Data::ManagerPtr datamgr = Avida::Data::Manager::Of([run world]);
  for (int i = 0; i < active_export_recorders.GetSize(); i++) {
    ofile << "," << runName << " " << datamgr->Describe(timeRecorders[active_export_recorders[i]]->RecordedDataID());
  }
  ofile << std::endl;
  
  // Print data rows
  for (int i = 0; i < timeRecorders[0]->NumPoints(); i++) {
    ofile << timeRecorders[0]->DataTime(i);
    
    for (int recorder_idx = 0; recorder_idx < active_export_recorders.GetSize(); recorder_idx++) {
      ofile << "," << timeRecorders[active_export_recorders[recorder_idx]]->DataPoint(i);
    }
    ofile << std::endl;
  }
  
  // Close file
  ofile.close();
}


@end


Avida::Data::ConstDataSetPtr AvidaEDPopViewStatViewRecorder::RequestedData() const
{
  if (!m_requested) {
    Avida::Data::DataSetPtr ds(new Avida::Data::DataSet);
    ds->Insert("core.world.organisms");
    ds->Insert("core.world.ave_fitness");
    ds->Insert("core.world.ave_metabolic_rate");
    ds->Insert("core.world.ave_gestation_time");
    ds->Insert("core.world.ave_age");
    
    for (NSUInteger i = 0; i < [m_view.envActions entryCount]; i++) {
      Apto::String data_id("core.environment.triggers.");
      NSString* entry_name = [m_view.envActions entryAtIndex:i];
      if ([entry_name isEqual:@"oro"]) {
        entry_name = @"or";
      } else if ([entry_name isEqual:@"ant"]) {
        entry_name = @"andn";
      } else if ([entry_name isEqual:@"nan"]) {
        entry_name = @"nand";
      }
      data_id += [entry_name UTF8String];
      data_id += ".test_organisms";
      ds->Insert(data_id);
    }
    m_requested = ds;
  }
  
  return m_requested;
}

void AvidaEDPopViewStatViewRecorder::NotifyData(Avida::Update, Avida::Data::DataRetrievalFunctor retrieve_data)
{
  @autoreleasepool {
    AvidaEDPopViewStatViewValues* values = [[AvidaEDPopViewStatViewValues alloc] init];
    
    values->organisms = retrieve_data("core.world.organisms")->IntValue();
    values->ave_fitness = retrieve_data("core.world.ave_fitness")->DoubleValue();
    values->ave_metabolic_rate = retrieve_data("core.world.ave_metabolic_rate")->DoubleValue();
    values->ave_gestation_time = retrieve_data("core.world.ave_gestation_time")->DoubleValue();
    values->ave_age = retrieve_data("core.world.ave_age")->DoubleValue();

    for (NSUInteger i = 0; i < [m_view.envActions entryCount]; i++) {
      NSString* entry_name = [m_view.envActions entryAtIndex:i];
      NSString* local_name = entry_name;
      if ([entry_name isEqual:@"oro"]) {
        entry_name = @"or";
      } else if ([entry_name isEqual:@"ant"]) {
        entry_name = @"andn";
      } else if ([entry_name isEqual:@"nan"]) {
        entry_name = @"nand";
      }
      Apto::String data_id("core.environment.triggers.");
      data_id += [entry_name UTF8String];
      data_id += ".test_organisms";
      int count = retrieve_data(data_id)->IntValue();
      [m_view.envActions updateEntry:local_name withValue:[NSNumber numberWithInt:count]];
    }

    [m_view performSelectorOnMainThread:@selector(handleData:) withObject:values waitUntilDone:NO];
  }
}



AvidaEDPopViewStatViewOrgRecorder::AvidaEDPopViewStatViewOrgRecorder(AvidaEDPopViewStatView* view)
  : m_view(view), m_x(-1), m_y(-1), m_requested(new Avida::Data::DataSet)
{
}


Avida::Data::ConstDataSetPtr AvidaEDPopViewStatViewOrgRecorder::RequestedData() const
{
  return m_requested;
}

void AvidaEDPopViewStatViewOrgRecorder::NotifyData(Avida::Update update, Avida::Data::DataRetrievalFunctor retrieve_data)
{
  @autoreleasepool {
    if (m_genotype_data_id.GetSize() == 0) return;
    
    
    AvidaEDPopViewStatViewOrgValues* values = [[AvidaEDPopViewStatViewOrgValues alloc] init];
    
    Avida::Data::PackagePtr package = retrieve_data(m_genotype_data_id);
    if (package) {
      values->genotype_id = package->IntValue();
    } else {
      values->genotype_id = -1;
    }
    package = retrieve_data(m_clade_data_id);
    if (package) {
      values->clade_id = package->IntValue();
    } else {
      values->clade_id = -1;
    }
    values->update = update;
    values->x = m_x;
    values->y = m_y;
    
    [m_view performSelectorOnMainThread:@selector(handleOrgData:) withObject:values waitUntilDone:NO];
  }
}

void AvidaEDPopViewStatViewOrgRecorder::SetCoords(int x, int y)
{
  m_x = x;
  m_y = y;
  
  m_requested->Clear();
  
  m_genotype_data_id = "core.population.group_id[genotype@";
  m_genotype_data_id += Apto::AsStr(x);
  m_genotype_data_id += ",";
  m_genotype_data_id += Apto::AsStr(y);
  m_genotype_data_id += "]";
  m_requested->Insert(m_genotype_data_id);
  
  m_clade_data_id = "core.population.group_id[clade@";
  m_clade_data_id += Apto::AsStr(x);
  m_clade_data_id += ",";
  m_clade_data_id += Apto::AsStr(y);
  m_clade_data_id += "]";
  m_requested->Insert(m_clade_data_id);
}


AvidaEDPopViewStatViewTimeRecorder::AvidaEDPopViewStatViewTimeRecorder(AvidaEDPopViewStatView* view, const Avida::Data::DataID& data_id,
                                                                       const Apto::String& loaded_data)
  : Avida::Data::TimeSeriesRecorder<double>(data_id, loaded_data), m_view(view), m_active(false)
{
}

bool AvidaEDPopViewStatViewTimeRecorder::shouldRecordValue(Avida::Update update)
{
//  return ((update % 10) == 0);
  (void)update;
  m_mutex.Lock(); // need to acquire object lock in order to allow update
  return true;
}

void AvidaEDPopViewStatViewTimeRecorder::didRecordValue()
{
  m_mutex.Unlock(); // release object lock for use on main thread
  @autoreleasepool {
    if (m_active) [m_view performSelectorOnMainThread:@selector(handleNewGraphData) withObject:nil waitUntilDone:NO];
  }
}
