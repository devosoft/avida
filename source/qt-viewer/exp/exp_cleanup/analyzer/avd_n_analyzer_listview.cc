#ifndef QDRAGOBJECT_H
#include <qdragobject.h>
#endif
#ifndef QEVENT_H
#include <qevent.h>
#endif
#ifndef QHEADER_H
#include <qheader.h>
#endif

#ifndef AVD_N_ANALYZER_DBG_MSGS_HH
#include "analyzer/avd_n_analyzer_dbg_msgs.hh"
#endif
#ifndef AVD_N_ANALYZER_LISTVIEW_HH
#include "analyzer/avd_n_analyzer_listview.hh"
#endif


avdAnalyzerListView::avdAnalyzerListView(
  QWidget *parent,
  const char *name,
  WFlags f
):QListView(parent, name, f)
{}

/*
This provides deleting current listviewitem using delete or backspace
key.
*/
void avdAnalyzerListView::keyPressEvent(QKeyEvent *e)
{ 
  QListView::keyPressEvent(e);

  switch( e->key() ) {
  case Key_Backspace:
  case Key_Delete:
    e->ignore();

    if(!currentItem()
    || !(currentItem()->isEnabled()
      && currentItem()->isSelected())
    ) break;

    QListViewItem *next_selection = currentItem()->itemBelow();
    if(!next_selection) next_selection = currentItem()->itemAbove();

    emit deletePressed(currentItem());

    if(viewport()->acceptDrops()
    && next_selection
    && next_selection->isEnabled()
    && next_selection->isSelectable()
    ) setSelected(next_selection, true);
  }
}

QDragObject *avdAnalyzerListView::dragObject(void){
  Debug << "entered.";
  if(currentItem() && currentItem()->isEnabled() && currentItem()->isSelected()){
    emit dragStartSig(currentItem());
    QTextDrag *text_drag = new QTextDrag(
      currentItem()->text(header()->mapToIndex(0)),
      this, "<avdAnalyzerListView::dragObject(test_drag)>");
    text_drag->setSubtype("avd_analyzer_list_view_format_choice");
    /*
    as currently used, a avdAnalyzerListView object can move items
    within itself only if it accepts drops; if it doesn't accept drops
    then items will be copied from it to another avdAnalyzerListView
    instance.
    */
    if(viewport()->acceptDrops()) text_drag->dragMove();
    else text_drag->dragCopy();

    return text_drag;
  } else return 0;
  Debug << "done.";
}

void avdAnalyzerListView::contentsDragMoveEvent(QDragMoveEvent *e){
  if(e->provides("text/avd_analyzer_list_view_format_choice")){
    e->accept();
  }
}
void avdAnalyzerListView::contentsDropEvent(QDropEvent *e){
  if(e->provides("text/avd_analyzer_list_view_format_choice")){
    e->accept();
    emit dropEventSig(itemAt(contentsToViewport(e->pos())));
  }
}

// arch-tag: implementation file for analyzer listview object
