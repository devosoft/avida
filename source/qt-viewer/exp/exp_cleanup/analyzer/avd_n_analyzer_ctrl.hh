#ifndef AVD_N_ANALYZER_CTRL_HH
#define AVD_N_ANALYZER_CTRL_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif

#ifndef AVD_N_ANALYZER_DATA_ENTRY_HH
#include "analyzer/avd_n_analyzer_data_entry.hh"
#endif
#ifndef TRETAINABLE_LIST_HH
#include "containers/tRetainable_list.hh"
#endif
#ifndef ANALYZE_HH
#include "main/analyze.hh"
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif


class QListViewItem;
class QMenuBar;
class avdAnalyzerBatchListItem;
class avdAnalyzerGenotypeSelView;
class avdAnalyzerMdl;
class avdAnalyzerMenuBarHandler;
class avdAnalyzerOutFmtSel;
class avdAnalyzerView;
class avdAnalyzerCtrl : public QObject, public cRetainable {
  Q_OBJECT
protected:
  avdAnalyzerMdl *m_mdl;
  QGuardedPtr<avdAnalyzerGenotypeSelView> m_gentype_sel;
  QGuardedPtr<avdAnalyzerView> m_view;
  QGuardedPtr<avdAnalyzerMenuBarHandler> m_local_menubar_hdlr;
  QGuardedPtr<avdAnalyzerOutFmtSel> m_columns_selector;
  tRetainableList<avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > m_data_entry_cmd_list;

  unsigned int m_batch_id_ctr;
  int m_free_batch_list_idx;
  int m_free_batch_list[MAX_BATCHES];
protected:
  void setGenotypeSelView(avdAnalyzerGenotypeSelView *view);
  void setView(avdAnalyzerView *view);
  void setMdl(avdAnalyzerMdl *mdl);
  void setLocalMenuBarHdlr(avdAnalyzerMenuBarHandler *local_menubar_hdlr);
  void setOutFmtSel(avdAnalyzerOutFmtSel *columns_selector);
  avdAnalyzerGenotypeSelView *getGenotypeSelView(){ return m_gentype_sel; }
  avdAnalyzerView *getView(){ return m_view; }
  avdAnalyzerMdl *getMdl(){ return m_mdl; }
  avdAnalyzerMenuBarHandler *getLocalMenuBarHdlr(){ return m_local_menubar_hdlr; }
  avdAnalyzerOutFmtSel *getOutFmtSel(){ return m_columns_selector; }

  QString verifyGenotypeSelView();
public:
  avdAnalyzerCtrl(QObject *parent = 0, const char *name = 0);
  ~avdAnalyzerCtrl();
  bool setup();
public slots:
  // menu responders
    //
  void loadCurrentGenotypesSlot();
  void loadHistoricGenotypesSlot(); //
  void openSlot(); //
    //
  void deleteBatchesSlot(); //
  void mergeBatchesSlot();
  void duplicateBatchesSlot();
    //
  void setupGenotypeSelView();
  void findGenotypeSlot();
  void findLineageSlot();
  void findCladeSlot();

  QString verifyGenotypeSelView(bool &verifies_ok);
  void doFindGenotypeSlot();
  void doFindLineageSlot();
  void doFindCladeSlot();

  void sampleOrganismsSlot();
  void sampleGenotypesSlot();
  void invertSlot();
    //
  void genotypePhenotypeMapSlot();
  // batch listview responders
  void batchListItemRenamedSlot(QListViewItem *lvi, int col, const QString &text); //
  void batchListSelectionChangedSlot(void); //
  // columns-selector responders
  void chooseColumnsSlot(); //
  void updateColumnsSlot(); //
  void selectorCancelBtnClickedSlot(); //
  void selectorApplyBtnClickedSlot(); //
  void selectorOkayBtnClickedSlot(); //
signals:
  void doResetMenuBar(QMenuBar *);
};

#endif

// arch-tag: header file for analyzer gui controller
