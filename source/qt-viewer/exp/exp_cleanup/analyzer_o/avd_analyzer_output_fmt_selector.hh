#ifndef AVD_ANALYZER_OUTPUT_FMT_SELECTOR_HH
#define AVD_ANALYZER_OUTPUT_FMT_SELECTOR_HH

#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif


class QListView;
class QListViewItem;
class QPushButton;
class avd_o_AnalyzerViewOutputFmtSelectorData;
class avd_o_AnalyzerViewOutputFmtSelector : public QMainWindow {
  Q_OBJECT
protected:
  avd_o_AnalyzerViewOutputFmtSelectorData *d;
public:
  avd_o_AnalyzerViewOutputFmtSelector(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  ~avd_o_AnalyzerViewOutputFmtSelector();
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

// arch-tag: header file for old analyzer view output format selector
