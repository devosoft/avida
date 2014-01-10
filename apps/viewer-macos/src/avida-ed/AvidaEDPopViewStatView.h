//
//  AvidaEDPopViewStatView.h
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

#import <Cocoa/Cocoa.h>
#import <CorePlot/CorePlot.h>

#include "avida/data/Recorder.h"
#include "avida/data/TimeSeriesRecorder.h"

#import "MapGridView.h"
#import "OrgColorBox.h"

@class AvidaRun;
@class AvidaEDController;
class AvidaEDPopViewStatViewRecorder;
class AvidaEDPopViewStatViewOrgRecorder;
class AvidaEDPopViewStatViewTimeRecorder;
@class AvidaEDEnvActionsDataSource;
@class AvidaEDPopViewStatViewGraphData;


@interface AvidaEDPopViewStatView : NSView <MapSelectionDelegate, NSDraggingSource> {
  IBOutlet AvidaEDController* ctlr;
  
  IBOutlet NSView* popStatsView;
  IBOutlet NSTextField* txtPopSize;
  IBOutlet NSTextField* txtFitness;
  IBOutlet NSTextField* txtMetabolicRate;
  IBOutlet NSTextField* txtGestation;
  IBOutlet NSTextField* txtAge;
  IBOutlet NSTableView* tblEnvActions;
  
  IBOutlet NSView* orgStatsView;
  IBOutlet NSTextField* txtOrgName;
  IBOutlet OrgColorBox* boxOrgColor;
  IBOutlet NSTextField* txtOrgFitness;
  IBOutlet NSTextField* txtOrgMetabolicRate;
  IBOutlet NSTextField* txtOrgGestation;
  IBOutlet NSTextField* txtOrgAge;
  IBOutlet NSTextField* txtOrgAncestor;
  IBOutlet NSTableView* tblOrgEnvActions;
  Apto::String genome;
  NSPoint selectedOrgCoords;
  
  AvidaRun* run;
  Avida::Data::RecorderPtr recorder;
  Apto::SmartPtr<AvidaEDPopViewStatViewOrgRecorder, Apto::InternalRCObject> org_recorder;
  AvidaEDEnvActionsDataSource* envActions;
  AvidaEDEnvActionsDataSource* orgEnvActions;
  IBOutlet id envActionChangeDelegate;
  
  IBOutlet NSPopUpButton* btnGraphSelect;
  IBOutlet CPTGraphHostingView* graphView;
  CPTXYGraph* graph;
  AvidaEDPopViewStatViewGraphData* graphData;
  
  Apto::Array<Apto::SmartPtr<AvidaEDPopViewStatViewTimeRecorder, Apto::InternalRCObject> > timeRecorders;
}

- (id) initWithFrame:(NSRect)frame;
- (void) awakeFromNib;

- (void) dealloc;
- (void) finalize;

- (BOOL) isFlipped;

- (void) resizeSubviewsWithOldSize:(NSSize)oldBoundsSize;

- (void) setAvidaRun:(AvidaRun*)avidarun fromFreezer:(Avida::Viewer::FreezerPtr)freezer withID:(Avida::Viewer::FreezerID)fid;
- (void) saveRunToFreezer:(Avida::Viewer::FreezerPtr)freezer withID:(Avida::Viewer::FreezerID)fid;
- (void) clearAvidaRun;

- (void) clearSelectedOrg;

- (BOOL) mapView:(MapGridView*)mapView shouldSelectObjectAtPoint:(NSPoint)point;
- (void) mapViewSelectionChanged:(MapGridView*)mapView;
- (void) mapViewModeChanged:(MapGridView*)mapView;
- (BOOL) mapView:(MapGridView*)mapView writeSelectionToPasteboard:(NSPasteboard*)pboard;

- (IBAction) changeGraph:(id)sender;
- (void) handleNewGraphData;

- (ACGenome*) selectedOrgGenome;

@property (readonly) CPTGraphHostingView* graphView;

- (void) exportData:(NSArray*)dataValues toURL:(NSURL*)url;


// NSDraggingSource
- (NSDragOperation) draggingSession:(NSDraggingSession*)session sourceOperationMaskForDraggingContext:(NSDraggingContext)context;
- (BOOL) ignoreModifierKeysForDraggingSession:(NSDraggingSession*)session;

@end


class AvidaEDPopViewStatViewRecorder : public Avida::Data::Recorder
{
private:
  mutable Avida::Data::ConstDataSetPtr m_requested;
  AvidaEDPopViewStatView* m_view;
  
public:
  AvidaEDPopViewStatViewRecorder(AvidaEDPopViewStatView* view) : m_view(view) { ; }
  
  Avida::Data::ConstDataSetPtr RequestedData() const;
  void NotifyData(Avida::Update, Avida::Data::DataRetrievalFunctor retrieve_data);
};

class AvidaEDPopViewStatViewOrgRecorder : public Avida::Data::Recorder
{
private:
  AvidaEDPopViewStatView* m_view;

  int m_x;
  int m_y;
  Avida::Data::DataID m_genotype_data_id;
  Avida::Data::DataID m_clade_data_id;
  mutable Avida::Data::DataSetPtr m_requested;
  
public:
  AvidaEDPopViewStatViewOrgRecorder(AvidaEDPopViewStatView* view);
  
  Avida::Data::ConstDataSetPtr RequestedData() const;
  LIB_EXPORT void NotifyData(Avida::Update, Avida::Data::DataRetrievalFunctor retreive_data);
  
  void SetCoords(int x, int y);
};


class AvidaEDPopViewStatViewTimeRecorder : public Avida::Data::TimeSeriesRecorder<double>
{
private:
  AvidaEDPopViewStatView* m_view;
  bool m_active;
  Apto::Mutex m_mutex;
  
public:
  AvidaEDPopViewStatViewTimeRecorder(AvidaEDPopViewStatView* view, const Avida::Data::DataID& data_id, const Apto::String& loaded_data);
  
  inline void SetActive() { m_active = true; }
  inline void SetInactive() { m_active = false; }
  
  inline void LockForDisplay() { m_mutex.Lock(); }
  inline void Unlock() { m_mutex.Unlock(); }

protected:
  bool shouldRecordValue(Avida::Update update);
  void didRecordValue();
};
