//
//  AvidaEDPopViewStatView.mm
//  avida/apps/viewer-macos
//
//  Created by David Bryson on 4/15/11.
//  Copyright 2011 Michigan State University. All rights reserved.
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

#import "AvidaRun.h"
#import "NSStringAdditions.h"

#include "avida/data/Package.h"
#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Manager.h"
#include "avida/environment/Product.h"

static const float PANEL_MIN_WIDTH = 300.0;

@interface AvidaEDPopViewStatViewEnvActions : NSObject <NSTableViewDataSource, NSTableViewDelegate> {
  NSMutableArray* entries;
  NSMutableDictionary* entrymap;
}
- (id) init;
- (void) addNewEntry:(NSString*)name withDescription:(NSString*)desc;
- (void) updateEntry:(NSString*)name withValue:(NSNumber*)value;
- (void) clearEntries;
- (NSString*) entryAtIndex:(NSUInteger)idx;
- (NSUInteger) entryCount;

- (NSInteger) numberOfRowsInTableView:(NSTableView*)tableView;
- (id) tableView:(NSTableView*)tableView objectValueForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex;

- (void) tableView:(NSTableView*)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex;
@end


@implementation AvidaEDPopViewStatViewEnvActions

- (id) init {
  entries = [[NSMutableArray alloc] init];
  entrymap = [[NSMutableDictionary alloc] init];
  return self;
}

- (void) addNewEntry:(NSString*)name withDescription:(NSString*)desc {
  NSMutableDictionary* entry = [[NSMutableDictionary alloc] init];
  [entry setValue:name forKey:@"Action"];
  [entry setValue:[NSNumber numberWithInt:NSOffState] forKey:@"State"];
  [entry setValue:desc forKey:@"Description"];
  [entry setValue:[NSNumber numberWithInt:0] forKey:@"Orgs Performing"];
  [entries addObject:entry];
  [entrymap setValue:[NSNumber numberWithLong:([entries count] - 1)] forKey:name];
}


- (void) updateEntry:(NSString*)name withValue:(NSNumber*)value {
  [[entries objectAtIndex:[[entrymap valueForKey:name] unsignedIntValue]] setValue:value forKey:@"Orgs Performing"];
}


- (void) clearEntries {
  [entries removeAllObjects];
}

- (NSString*) entryAtIndex:(NSUInteger)idx
{
  return [[entries objectAtIndex:idx] valueForKey:@"Action"];
}
- (NSUInteger) entryCount {
  return [entries count];
}


- (NSInteger) numberOfRowsInTableView:(NSTableView*)tableView {
  return [entries count];
}

- (id) tableView:(NSTableView*)tableView objectValueForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex {
  id entry, value;
  
  entry = [entries objectAtIndex:rowIndex];
  value = [entry objectForKey:[tableColumn identifier]];
  return value;
}


- (void) tableView:(NSTableView*)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex
{
  if ([[tableColumn identifier] isEqualToString:@"State"]) {
    [cell setTitle:[[entries objectAtIndex:rowIndex] objectForKey:@"Action"]];
  }
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


@interface AvidaEDPopViewStatView (hidden)
- (void) setup;  
@property (readonly) AvidaEDPopViewStatViewEnvActions* envActions;
@end

@implementation AvidaEDPopViewStatView (hidden)
@dynamic envActions;
- (AvidaEDPopViewStatViewEnvActions*)envActions {
  return envActions;
}

- (void) setup {
  envActions = [[AvidaEDPopViewStatViewEnvActions alloc] init];
  [tblEnvActions setDataSource:envActions];
  [tblEnvActions setDelegate:envActions];
  [tblEnvActions reloadData];

  NSNumberFormatter* fitFormat = [[NSNumberFormatter alloc] init];
  [fitFormat setNumberStyle:NSNumberFormatterDecimalStyle];
  [fitFormat setMaximumFractionDigits:4];
  [txtFitness setFormatter:fitFormat];
  
  NSNumberFormatter* dec1format = [[NSNumberFormatter alloc] init];
  [dec1format setNumberStyle:NSNumberFormatterDecimalStyle];
  [dec1format setPositiveFormat:@"#0.0"];
  [dec1format setNegativeFormat:@"-#0.0"];
  [txtMetabolicRate setFormatter:dec1format];
  [txtGestation setFormatter:dec1format];
  
  NSNumberFormatter* dec2format = [[NSNumberFormatter alloc] init];
  [dec2format setNumberStyle:NSNumberFormatterDecimalStyle];
  [dec2format setPositiveFormat:@"#0.00"];
  [dec2format setNegativeFormat:@"-#0.00"];
  [txtAge setFormatter:dec2format];
}

@end


@implementation AvidaEDPopViewStatView

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
  [super dealloc];
  if (recorder) {
    assert(run);
    [run detachRecorder:recorder];
  }
}

- (void)finalize
{
  [super dealloc];
  if (recorder) {
    assert(run);
    [run detachRecorder:recorder];
  }
}


- (BOOL)isFlipped {
  return YES;
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldBoundsSize {
  NSRect bounds = [self bounds];
  
  if (bounds.size.width != oldBoundsSize.width && bounds.size.width >= PANEL_MIN_WIDTH) {
    const CGFloat spacing = 7.0;
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
}


- (void) setAvidaRun:(AvidaRun*)avidarun {
  [self clearAvidaRun];
  run = avidarun;

  Avida::Environment::ManagerPtr env = Avida::Environment::Manager::Of([avidarun world]);
  Avida::Environment::ConstActionTriggerIDSetPtr trigger_ids = env->GetActionTriggerIDs();
  for (Avida::Environment::ConstActionTriggerIDSetIterator it = trigger_ids->Begin(); it.Next();) {
    Avida::Environment::ConstActionTriggerPtr action = env->GetActionTrigger(*it.Get());
    [envActions addNewEntry:[NSString stringWithAptoString:action->GetID()] withDescription:[NSString stringWithAptoString:action->GetDescription()]];
  }
  [tblEnvActions reloadData];

  recorder = Avida::Data::RecorderPtr(new AvidaEDPopViewStatViewRecorder(self));
  [run attachRecorder:recorder];
}

- (void) clearAvidaRun {
  if (recorder) {
    assert(run);
    [run detachRecorder:recorder];
    recorder = Avida::Data::RecorderPtr(NULL);
  }
  run = nil;
  
  NSString* empty_str = @"-";
  [txtPopSize setStringValue:empty_str];
  [txtFitness setStringValue:empty_str];
  [txtMetabolicRate setStringValue:empty_str];
  [txtGestation setStringValue:empty_str];
  [txtAge setStringValue:empty_str];
  [envActions clearEntries];
  [tblEnvActions reloadData];
}


- (void) handleData:(AvidaEDPopViewStatViewValues*)values {
  [txtPopSize setIntegerValue:values->organisms];
  [txtFitness setDoubleValue:values->ave_fitness];
  [txtMetabolicRate setDoubleValue:values->ave_metabolic_rate];
  [txtGestation setDoubleValue:values->ave_gestation_time];
  [txtAge setDoubleValue:values->ave_age];
  [tblEnvActions reloadData];
}

@end


Avida::Data::ConstDataSetPtr AvidaEDPopViewStatViewRecorder::GetRequested()
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
      data_id += [[m_view.envActions entryAtIndex:i] UTF8String];
      data_id += ".organisms";
      ds->Insert(data_id);
    }
    m_requested = ds;
  }
  
  return m_requested;
}

void AvidaEDPopViewStatViewRecorder::NotifyData(Avida::Update, Avida::Data::DataRetrievalFunctor retrieve_data)
{
  AvidaEDPopViewStatViewValues* values = [[AvidaEDPopViewStatViewValues alloc] init];
  
  values->organisms = retrieve_data("core.world.organisms")->IntValue();
  values->ave_fitness = retrieve_data("core.world.ave_fitness")->DoubleValue();
  values->ave_metabolic_rate = retrieve_data("core.world.ave_metabolic_rate")->DoubleValue();
  values->ave_gestation_time = retrieve_data("core.world.ave_gestation_time")->DoubleValue();
  values->ave_age = retrieve_data("core.world.ave_age")->DoubleValue();

  for (NSUInteger i = 0; i < [m_view.envActions entryCount]; i++) {
    NSString* entry_name = [m_view.envActions entryAtIndex:i];
    Apto::String data_id("core.environment.triggers.");
    data_id += [entry_name UTF8String];
    data_id += ".organisms";
    int count = retrieve_data(data_id)->IntValue();
    [m_view.envActions updateEntry:entry_name withValue:[NSNumber numberWithInt:count]];
  }

  [m_view performSelectorOnMainThread:@selector(handleData:) withObject:values waitUntilDone:NO];
}
