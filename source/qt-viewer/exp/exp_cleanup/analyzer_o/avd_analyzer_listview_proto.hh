#ifndef AVD_ANALYZER_LISTVIEW_PROTO_HH
#define AVD_ANALYZER_LISTVIEW_PROTO_HH

#ifndef QLISTVIEW_H
#include <qlistview.h>
#endif

class QDragObject;
class QDragEnterEvent;
class QKeyEvent;
class QListViewItem;
class avd_o_AnalyzerListView : public QListView {
  Q_OBJECT
protected:
  void keyPressEvent(QKeyEvent *e);
  QDragObject *dragObject(void);
  virtual void contentsDragMoveEvent(QDragMoveEvent *e);
  virtual void contentsDropEvent(QDropEvent *e);
public:
  avd_o_AnalyzerListView(
    QWidget *parent = 0, const char *name = 0, WFlags f = 0);
signals:
  void deletePressed(QListViewItem *);
  void dragStartSig(QListViewItem *);
  void dropEventSig(QListViewItem *);
};

#endif

// arch-tag: proto file for old analyzer list view object
