#ifndef AVD_N_ANALYZER_LISTVIEW_HH
#define AVD_N_ANALYZER_LISTVIEW_HH

#ifndef QLISTVIEW_H
#include <qlistview.h>
#endif

#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QDragObject;
class QDragEnterEvent;
class QKeyEvent;
class QListViewItem;
class avdAnalyzerListView : public QListView, public cRetainable {
  Q_OBJECT
protected:
  void keyPressEvent(QKeyEvent *e);
  QDragObject *dragObject(void);
  virtual void contentsDragMoveEvent(QDragMoveEvent *e);
  virtual void contentsDropEvent(QDropEvent *e);
public:
  avdAnalyzerListView(QWidget *parent = 0, const char *name = 0, WFlags f = 0);
signals:
  void deletePressed(QListViewItem *);
  void dragStartSig(QListViewItem *);
  void dropEventSig(QListViewItem *);
};

#endif

// arch-tag: header file for analyzer listview object
