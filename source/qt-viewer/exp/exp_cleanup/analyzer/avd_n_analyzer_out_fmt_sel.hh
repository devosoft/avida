#ifndef AVD_N_ANALYZER_OUT_FMT_SEL_HH
#define AVD_N_ANALYZER_OUT_FMT_SEL_HH

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
class QListViewItem;
class QPushButton;
class avdAnalyzerDataEntryCommandListItem;
class avdAnalyzerListView;
class avdAnalyzerOutFmtSel : public QMainWindow, public cRetainable {
  Q_OBJECT
protected:
  QGuardedPtr<avdAnalyzerListView> m_format_menu_lv;
  QGuardedPtr<avdAnalyzerListView> m_formats_chosen_lv;
  avdAnalyzerDataEntryCommandListItem *m_source_drag_lvi;
  QPushButton *m_cancel_button;
  QPushButton *m_apply_button;
  QPushButton *m_okay_button;
public:
  avdAnalyzerOutFmtSel(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  ~avdAnalyzerOutFmtSel();

  void setMenuListView(avdAnalyzerListView *format_menu_lv);
  void setChoiceListView(avdAnalyzerListView *format_chosen_lv);

  QListView *getMenuListView();
  QListView *getChoiceListView();
  QPushButton *getCancelButton();
  QPushButton *getApplyButton();
  QPushButton *getOkayButton();
protected slots:
  void deletePressedSlot(QListViewItem *lvi);
  void dragStartSlot(QListViewItem *);
  void dropEventSlot(QListViewItem *);
  void copyButtonSlot();
};

#endif

// arch-tag: header file for analyzer output format selector gui
