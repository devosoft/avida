#ifndef QDRAGOBJECT_H
#include <qdragobject.h>
#endif
#ifndef QEVENT_H
#include <qevent.h>
#endif
#ifndef QHEADER_H
#include <qheader.h>
#endif

#ifndef AVD_ANALYZER_UTILS_HH
#include "avd_analyzer_utils.hh"
#endif

#ifndef AVD_ANALYZER_LISTVIEW_HH
#include "avd_analyzer_listview.hh"
#endif

avd_o_AnalyzerListView::avd_o_AnalyzerListView(
  QWidget *parent,
  const char *name,
  WFlags f
):QListView(parent, name, f)
{}

/*
This provides deleting current listviewitem using delete or backspace
key.
*/
void avd_o_AnalyzerListView::keyPressEvent(QKeyEvent *e)
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

QDragObject *avd_o_AnalyzerListView::dragObject(void){
  if(currentItem() && currentItem()->isEnabled() && currentItem()->isSelected()){
    emit dragStartSig(currentItem());
    QTextDrag *text_drag = new QTextDrag(
      currentItem()->text(header()->mapToIndex(0)),
      this, "<avd_o_AnalyzerListView::dragObject(test_drag)>");
    text_drag->setSubtype("avd_o_AnalyzerListView_format_choice");
    /*
    as currently used, a avd_o_AnalyzerListView object can move items
    within itself only if it accepts drops; if it doesn't accept drops
    then items will be copied from it to another avd_o_AnalyzerListView
    instance.
    */
    if(viewport()->acceptDrops()) text_drag->dragMove();
    else text_drag->dragCopy();

    return text_drag;
  } else return 0;
}

void avd_o_AnalyzerListView::contentsDragMoveEvent(QDragMoveEvent *e){
  e->accept(e->provides("text/avd_analyzerlistview_format_choice"));
}
void avd_o_AnalyzerListView::contentsDropEvent(QDropEvent *e){
  if(e->provides("text/avd_analyzerlistview_format_choice")){
    e->accept();
    emit dropEventSig(itemAt(contentsToViewport(e->pos())));
  }
}

// arch-tag: implementation file for old analyzer list view object
