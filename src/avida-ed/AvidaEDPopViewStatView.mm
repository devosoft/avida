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
#import "AvidaEDController.h"
#import "NSStringAdditions.h"

#include "avida/core/Properties.h"
#include "avida/data/Package.h"
#include "avida/environment/ActionTrigger.h"
#include "avida/environment/Manager.h"
#include "avida/environment/Product.h"

#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Group.h"
#include "avida/systematics/Manager.h"

#include "avida/viewer-core/ClassificationInfo.h"


static const float PANEL_MIN_WIDTH = 300.0;

@interface AvidaEDPopViewStatViewEnvActions : NSObject <NSTableViewDataSource> {
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
- (void) tableView:(NSTableView*)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex;
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

- (void) tableView:(NSTableView*)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex;
{
  if ([[tableColumn identifier] isEqualToString:@"State"]) {
    id entry = [entries objectAtIndex:rowIndex];
    [entry setValue:object forKey:@"State"];
    if ([[tableView delegate] respondsToSelector:@selector(envActionStateChange:)]) {
      NSMutableDictionary* states = [[NSMutableDictionary alloc] init];
      for (int i = 0; i < [entries count]; i++) {
        NSMutableDictionary* cur_entry = [entries objectAtIndex:i];
        [states setValue:[cur_entry valueForKey:@"State"] forKey:[cur_entry valueForKey:@"Action"]];
      }
      [[tableView delegate] performSelector:@selector(envActionStateChange:) withObject:states];
    }
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


@interface AvidaEDPopViewStatViewOrgValues : NSObject {
@public
  Avida::Update update;
  int genotype_id;
  int x;
  int y;
}
@end;
@implementation AvidaEDPopViewStatViewOrgValues
@end



@interface AvidaEDPopViewStatView (hidden) <NSTableViewDelegate>
- (void) setup;

- (void) tableView:(NSTableView*)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex;
- (void) envActionStateChange:(NSMutableDictionary*)newState;

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
  [tblEnvActions setDelegate:self];
  [tblEnvActions reloadData];

  orgEnvActions = [[AvidaEDPopViewStatViewEnvActions alloc] init];
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
}

- (void) envActionStateChange:(NSMutableDictionary*)newState;
{
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
    NSString* entryName = [NSString stringWithAptoString:action->GetID()];
    NSString* entryDesc = [NSString stringWithAptoString:action->GetDescription()];
    [envActions addNewEntry:entryName withDescription:entryDesc];
    [orgEnvActions addNewEntry:entryName withDescription:entryDesc];
  }
  [tblEnvActions reloadData];
  [tblOrgEnvActions reloadData];

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
  
  [self clearSelectedOrg];
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
}


- (BOOL) mapView:(MapGridView*)mapView shouldSelectObjectAtPoint:(NSPoint)point {
  return YES;
}


- (void) mapViewSelectionChanged:(MapGridView*)mapView {
  if (org_recorder) {
    [run detachRecorder:org_recorder];
  } else {
    org_recorder = Apto::SmartPtr<AvidaEDPopViewStatViewOrgRecorder, Apto::ThreadSafeRefCount>(new AvidaEDPopViewStatViewOrgRecorder(self));
  }
  org_recorder->SetCoords([mapView selectedObject].x, [mapView selectedObject].y);
  [run attachRecorder:org_recorder];
}



- (void) handleData:(AvidaEDPopViewStatViewValues*)values {
  [txtPopSize setIntegerValue:values->organisms];
  [txtFitness setDoubleValue:values->ave_fitness];
  [txtMetabolicRate setDoubleValue:values->ave_metabolic_rate];
  [txtGestation setDoubleValue:values->ave_gestation_time];
  [txtAge setDoubleValue:values->ave_age];
  [tblEnvActions reloadData];
}

- (void) handleOrgData:(AvidaEDPopViewStatViewOrgValues*)values {
  
  Avida::World* world = [run world];
  Avida::Systematics::ArbiterPtr g_arb = Avida::Systematics::Manager::Of(world)->ArbiterForRole("genotype");
  Avida::Systematics::GroupPtr genotype = g_arb->Group(values->genotype_id);

  if (!genotype) {
    [self clearSelectedOrg];
    return;
  }
  
  Apto::String foo = genotype->Properties().Get("ave_fitness");

  [txtOrgName setStringValue:[NSString stringWithAptoString:genotype->Properties().Get("name")]];
  [txtOrgFitness setDoubleValue:Apto::StrAs(genotype->Properties().Get("ave_fitness"))];
  [txtOrgMetabolicRate setDoubleValue:Apto::StrAs(genotype->Properties().Get("ave_metabolic_rate"))];
  [txtOrgGestation setDoubleValue:Apto::StrAs(genotype->Properties().Get("ave_gestation_time"))];
  [txtOrgAge setIntValue:(values->update - (int)Apto::StrAs(genotype->Properties().Get("update_born")))];
  
  Apto::String parents(genotype->Properties().Get("parents").Value());
  parents = parents.Pop(',');
  if (parents.GetSize()) {
    Avida::Systematics::GroupPtr parent_genotype = g_arb->Group(Apto::StrAs(parents));
    [txtOrgAncestor setStringValue:[NSString stringWithAptoString:parent_genotype->Properties().Get("name")]];
  } else {
    [txtOrgAncestor setStringValue:@"-"];
  }
  
  [boxOrgColor setColor:[[ctlr mapView] colorOfX:values->x Y:values->y]];
}

@end


Avida::Data::ConstDataSetPtr AvidaEDPopViewStatViewRecorder::GetRequested() const
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



AvidaEDPopViewStatViewOrgRecorder::AvidaEDPopViewStatViewOrgRecorder(AvidaEDPopViewStatView* view)
  : m_view(view), m_x(-1), m_y(-1), m_requested(new Avida::Data::DataSet)
{
}


Avida::Data::ConstDataSetPtr AvidaEDPopViewStatViewOrgRecorder::GetRequested() const
{
  return m_requested;
}

void AvidaEDPopViewStatViewOrgRecorder::NotifyData(Avida::Update update, Avida::Data::DataRetrievalFunctor retrieve_data)
{
  if (m_data_id == "") return;
  
  
  AvidaEDPopViewStatViewOrgValues* values = [[AvidaEDPopViewStatViewOrgValues alloc] init];
  
  Avida::Data::PackagePtr package = retrieve_data(m_data_id);
  if (package) {
    values->genotype_id = package->IntValue();
  } else {
    values->genotype_id = -1;
  }
  values->update = update;
  values->x = m_x;
  values->y = m_y;
  
  [m_view performSelectorOnMainThread:@selector(handleOrgData:) withObject:values waitUntilDone:NO];
}

void AvidaEDPopViewStatViewOrgRecorder::SetCoords(int x, int y)
{
  m_x = x;
  m_y = y;
  
  m_data_id = "core.population.group_id[genotype@";
  m_data_id += Apto::AsStr(x);
  m_data_id += ",";
  m_data_id += Apto::AsStr(y);
  m_data_id += "]";
  m_requested->Clear();
  m_requested->Insert(m_data_id);

  printf("%s\n", (const char*)m_data_id);
}
