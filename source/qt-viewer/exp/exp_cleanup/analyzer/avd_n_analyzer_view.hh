#ifndef AVD_N_ANALYZER_VIEW_HH
#define AVD_N_ANALYZER_VIEW_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QListView;
class avdAnalyzerView : public QMainWindow, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<QListView> m_batch_lv, m_genotype_lv;
public:
  avdAnalyzerView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  QListView *getBatchesListView(){ return m_batch_lv; }
  QListView *getGenotypesListView(){ return m_genotype_lv; }
};

#endif

// arch-tag: header file for analyzer gui view
