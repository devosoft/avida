#ifndef AVD_ANALYZER_CONTROLLER_DATA_HH
#define AVD_ANALYZER_CONTROLLER_DATA_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QLISTVIEW_H
#include <qlistview.h>
#endif
#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef ANALYZE_HH
#include "main/analyze.hh"
#endif
#ifndef DATA_ENTRY_HH
#include "data_entry.hh"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

#ifndef AVD_ANALYZER_DATA_ENTRY_HH
#include "avd_analyzer_data_entry.hh"
#endif


class avd_o_AnalyzerBatchListItem;
class avd_o_AnalyzerMenuBarHandler;
class avd_o_AnalyzerModel;
class avd_o_AnalyzerView;
class avd_o_AnalyzerViewOutputFmtSelector;
class avd_o_AnalyzerCtrlData : public QObject {
  Q_OBJECT
public:
  avd_o_AnalyzerModel *m_analyzer_model;
  QGuardedPtr<avd_o_AnalyzerMenuBarHandler> m_analyzer_menubar_handler;
  QGuardedPtr<avd_o_AnalyzerView> m_analyzer_view;
  QGuardedPtr<avd_o_AnalyzerViewOutputFmtSelector> m_columns_selector;

  tList<avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> > m_data_entry_cmd_list;

  unsigned int m_batch_id_ctr;
  int m_free_batch_list_idx;
  int m_free_batch_list[MAX_BATCHES];

public:
  avd_o_AnalyzerCtrlData();
  ~avd_o_AnalyzerCtrlData();

// menu responders
public slots:
  void loadCurrentGenotypesSlot(); //
  void loadHistoricGenotypesSlot(); //
  void openSlot(); //

  void deleteBatchesSlot(){ qDebug("<avd_o_AnalyzerCtrlData::deleteBatchesSlot>."); }
  void mergeBatchesSlot(){ qDebug("<avd_o_AnalyzerCtrlData::mergeBatchesSlot>."); }
  void duplicateBatchesSlot(){ qDebug("<avd_o_AnalyzerCtrlData::duplicateBatchesSlot>."); }

  void findGenotypeSlot(){ qDebug("<avd_o_AnalyzerCtrlData::findGenotypeSlot>."); }
  void findLineageSlot(){ qDebug("<avd_o_AnalyzerCtrlData::findLineageSlot>."); }
  void findCladeSlot(){ qDebug("<avd_o_AnalyzerCtrlData::findCladeSlot>."); }
  void sampleOrganismsSlot(){ qDebug("<avd_o_AnalyzerCtrlData::sampleOrganismsSlot>."); }
  void sampleGenotypesSlot(){ qDebug("<avd_o_AnalyzerCtrlData::sampleGenotypesSlot>."); }
  void invertSlot(){ qDebug("<avd_o_AnalyzerCtrlData::invertSlot>."); }

  void genotypePhenotypeMapSlot(){ qDebug("<avd_o_AnalyzerCtrlData::genotypePhenotypeMapSlot>."); }

// batch listview responders
public slots:
  void batchListItemRenamedSlot(QListViewItem *lvi, int col, const QString &text); //
  void batchListSelectionChangedSlot(void); //

public slots:
  void chooseColumnsSlot();
  void updateColumnsSlot();

  void selectorCancelButtonClickedSlot();
  void selectorApplyButtonClickedSlot();
  void selectorOkayButtonClickedSlot();
};


#endif

// arch-tag: header file for old analyzer gui controller private data
