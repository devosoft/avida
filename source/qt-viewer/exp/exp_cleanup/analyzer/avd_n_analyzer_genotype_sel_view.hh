#ifndef AVD_N_ANALYZER_GENOTYPE_SEL_VIEW_HH
#define AVD_N_ANALYZER_GENOTYPE_SEL_VIEW_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QDIALOG_H
#include <qdialog.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QCheckBox;
class QListViewItem;
class QPushButton;
class avdAnalyzerListView;
class avdAnalyzerGenotypeSelView : public QDialog, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<QCheckBox> m_num_cpus;;
  QGuardedPtr<QCheckBox> m_total_cpus;;
  QGuardedPtr<QCheckBox> m_fitness;;
  QGuardedPtr<QCheckBox> m_merit;;
  QGuardedPtr<avdAnalyzerListView> m_list_view;
  QGuardedPtr<QPushButton> m_add_btn;
  QGuardedPtr<QPushButton> m_okay_btn;
  QGuardedPtr<QPushButton> m_cancel_btn;
protected:
  void setListView(avdAnalyzerListView *lv);
public:
  avdAnalyzerGenotypeSelView(
    QWidget *parent = 0,
    const char *name = 0,
    bool modal = false,
    WFlags f = 0
  );
  QCheckBox *chooseNumCPUs(){ return m_num_cpus; }
  QCheckBox *chooseTotalCPUs(){ return m_total_cpus; }
  QCheckBox *chooseFitness(){ return m_fitness; }
  QCheckBox *chooseMerit(){ return m_merit; }
  avdAnalyzerListView *getListView(){ return m_list_view; }
  QPushButton *getAddBtn(){ return m_add_btn; }
  QPushButton *getOkayBtn(){ return m_okay_btn; }
  QPushButton *getCancelBtn(){ return m_cancel_btn; }
protected slots:
  void deletePressedSlot(QListViewItem *lvi);
  void itemRenamed(QListViewItem *item, int col, const QString &text);
  void addItem();
};

#endif

// arch-tag: header file for analyzer genotype selector gui view
