#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QHEADER_H
#include <qheader.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QSPLITTER_H
#include <qsplitter.h>
#endif


#ifndef AVD_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_HH
#include "avd_analyzer_data_entry_command_list_item.hh"
#endif
#ifndef AVD_ANALYZER_LISTVIEW_HH
#include "avd_analyzer_listview.hh"
#endif
#ifndef AVD_ANALYZER_UTILS_HH
#include "avd_analyzer_utils.hh"
#endif

#ifndef AVD_ANALYZER_OUTPUT_FMT_SELECTOR_HH
#include "avd_analyzer_output_fmt_selector.hh"
#endif


struct avd_o_AnalyzerViewOutputFmtSelectorData {
  avd_o_AnalyzerListView *m_format_menu_lv;
  avd_o_AnalyzerListView *m_formats_chosen_lv;
  avd_o_AnalyzerDataEntryCommandListItem *m_source_drag_lvi;
  QPushButton *m_cancel_button;
  QPushButton *m_apply_button;
  QPushButton *m_okay_button;
};

QListView *avd_o_AnalyzerViewOutputFmtSelector::getMenuListView()
{ return d->m_format_menu_lv; }
QListView *avd_o_AnalyzerViewOutputFmtSelector::getChoiceListView()
{ return d->m_formats_chosen_lv; }
QPushButton *avd_o_AnalyzerViewOutputFmtSelector::getCancelButton()
{ return d->m_cancel_button; }
QPushButton *avd_o_AnalyzerViewOutputFmtSelector::getApplyButton()
{ return d->m_apply_button; }
QPushButton *avd_o_AnalyzerViewOutputFmtSelector::getOkayButton()
{ return d->m_okay_button; }

void avd_o_AnalyzerViewOutputFmtSelector::deletePressedSlot(QListViewItem *lvi){
  if(lvi) delete lvi;
}

/*
Two methods supporting drag-and-drop.
*/
void avd_o_AnalyzerViewOutputFmtSelector::dragStartSlot(QListViewItem *lvi){
  d->m_source_drag_lvi =
    static_cast<avd_o_AnalyzerDataEntryCommandListItem *>(lvi);
}
void avd_o_AnalyzerViewOutputFmtSelector::dropEventSlot(QListViewItem *lvi){
  AnGuiDebug << "entered.";
  /*
  check whether this just a move within the formats-chosen listview.
  */
  if(d->m_formats_chosen_lv == d->m_source_drag_lvi->listView()){
    AnGuiDebug << "move.";
    /*
    moving the source format item within the chosen list.
    decide whether to place source immediately before or after
    target.
    */
    bool place_source_after_target = true;
    QListViewItemIterator it(lvi);
    while((++it).current()){
      if(d->m_source_drag_lvi == it.current()){
        place_source_after_target = false;
        break;
    } }
    // place source after target.
    d->m_source_drag_lvi->moveItem(lvi);
    if(!place_source_after_target)
      // swap if we need to do so.
      lvi->moveItem(d->m_source_drag_lvi);
  } else {
    /*
    This is a copy from the format-choices listview to the
    formats-chosen listview.
    */
    AnGuiDebug << "copy.";
    /*
    see whether the source already exists in the formats-chosen
    listview.
    */
    QListViewItem *duplicate = d->m_formats_chosen_lv->findItem(
      d->m_source_drag_lvi->getName(),
      d->m_formats_chosen_lv->header()->mapToIndex(0));
    if(duplicate){
      duplicate->moveItem(lvi);
      d->m_formats_chosen_lv->setSelected(duplicate, true);
    } else {
      /*
      source isn't duplicated, so we can make a new one.
      */
      avd_o_AnalyzerDataEntryCommandListItem *adec_lvi =
        new avd_o_AnalyzerDataEntryCommandListItem(
          d->m_source_drag_lvi->getDataEntryCommand(),
          d->m_formats_chosen_lv);
      adec_lvi->setDragEnabled(true);
      adec_lvi->setDropEnabled(true);
      adec_lvi->moveItem(lvi);
      d->m_formats_chosen_lv->setSelected(adec_lvi, true);
    }
  }
  AnGuiDebug << "done.";
}
void avd_o_AnalyzerViewOutputFmtSelector::copyButtonSlot(){
  AnGuiDebug << "entered.";
  if(d->m_format_menu_lv->currentItem()
  && d->m_format_menu_lv->currentItem()->isEnabled()
  && d->m_format_menu_lv->currentItem()->isSelected()
  ){
    /*
    see whether the source already exists in the formats-chosen
    listview.
    */
    avd_o_AnalyzerDataEntryCommandListItem *src_lvi =
      static_cast<avd_o_AnalyzerDataEntryCommandListItem *>(
        d->m_format_menu_lv->currentItem());
    QListViewItem *duplicate = d->m_formats_chosen_lv->findItem(
      src_lvi->getName(),
      0);
      //d->m_formats_chosen_lv->header()->mapToIndex(0));
    if(duplicate){
      /*
      move the duplicate up to the top of the formats-chosen listview.
      */
      d->m_formats_chosen_lv->takeItem(duplicate);
      d->m_formats_chosen_lv->insertItem(duplicate);
      d->m_formats_chosen_lv->setSelected(duplicate, true);
    } else {
      /*
      source isn't duplicated, so we can make a new one.
      */
      avd_o_AnalyzerDataEntryCommandListItem *new_lvi =
        new avd_o_AnalyzerDataEntryCommandListItem(
          src_lvi->getDataEntryCommand(),
          d->m_formats_chosen_lv);
      new_lvi->setDragEnabled(true);
      new_lvi->setDropEnabled(true);
      d->m_formats_chosen_lv->setSelected(new_lvi, true);
    }
  }
  AnGuiDebug << "done.";
}

avd_o_AnalyzerViewOutputFmtSelector::~avd_o_AnalyzerViewOutputFmtSelector(){
  AnGuiDebug << "...";
}
avd_o_AnalyzerViewOutputFmtSelector::avd_o_AnalyzerViewOutputFmtSelector(
  QWidget *parent,
  const char *name,
  WFlags f
):QMainWindow(parent, name, f)
{
  AnGuiDebug << "constructor.";

  d = new avd_o_AnalyzerViewOutputFmtSelectorData;
  if(!d){
    AnGuiFatal
    << "Couldn't allocate new avd_o_AnalyzerViewOutputFmtSelectorData! "
    << "Out of memory?";
  }

  QWidget *central_widget = new QWidget(
    this, "<avd_o_AnalyzerViewOutputFmtSelector(central_widget)>");
  QVBoxLayout *top_layout = new QVBoxLayout(
    central_widget, 10, -1,
    "<avd_o_AnalyzerViewOutputFmtSelector(top_layout)>");
  setCentralWidget(central_widget);

    QSplitter *horizontal_splitter = new QSplitter(
      Qt::Horizontal, central_widget,
      "<avd_o_AnalyzerViewOutputFmtSelector(horizontal_splitter)>");
    horizontal_splitter->setOpaqueResize(true);
    top_layout->addWidget(horizontal_splitter, 1);

      QWidget *fmt_menu_layout_widget = new QWidget(
        horizontal_splitter,
        "<avd_o_AnalyzerViewOutputFmtSelector(fmt_menu_layout_widget)>");
      QHBoxLayout *fmt_menu_layout = new QHBoxLayout(
        fmt_menu_layout_widget, 10, -1,
        "<avd_o_AnalyzerViewOutputFmtSelector(selector_layout)>");
      horizontal_splitter->moveToFirst(fmt_menu_layout_widget);

        d->m_format_menu_lv = new avd_o_AnalyzerListView(
          fmt_menu_layout_widget,
          "<avd_o_AnalyzerViewOutputFmtSelector(m_format_menu_lv)>");
        d->m_format_menu_lv->setSizePolicy(
          QSizePolicy::Preferred, QSizePolicy::Preferred,
          d->m_format_menu_lv->sizePolicy().hasHeightForWidth());
        d->m_format_menu_lv->setSorting(-1);
        d->m_format_menu_lv->setAllColumnsShowFocus(true);
        d->m_format_menu_lv->viewport()->setAcceptDrops(false);
        d->m_format_menu_lv->addColumn("Name");
        d->m_format_menu_lv->addColumn("Description");
        fmt_menu_layout->addWidget(d->m_format_menu_lv);

      QWidget *selector_layout_widget = new QWidget(
        horizontal_splitter,
        "<avd_o_AnalyzerViewOutputFmtSelector(selector_layout_widget)>");
      QHBoxLayout *selector_layout = new QHBoxLayout(
        selector_layout_widget, 10, -1,
        "<avd_o_AnalyzerViewOutputFmtSelector(selector_layout)>");
      horizontal_splitter->moveToLast(selector_layout_widget);

        QVBoxLayout *button_layout = new QVBoxLayout(
          selector_layout, -1,
          "<avd_o_AnalyzerViewOutputFmtSelector(button_layout)>");

          QPushButton *copy_btn = new QPushButton(
            "  ->  ", selector_layout_widget);
          button_layout->addWidget(copy_btn);
          copy_btn->setMaximumWidth(copy_btn->sizeHint().width());

        QVBoxLayout *chosen_layout = new QVBoxLayout(
          selector_layout, -1,
          "<avd_o_AnalyzerViewOutputFmtSelector(chosen_layout)>");

          d->m_formats_chosen_lv = new avd_o_AnalyzerListView(
            selector_layout_widget,
            "<avd_o_AnalyzerViewOutputFmtSelector(m_formats_chosen_lv)>");
          d->m_formats_chosen_lv->setSizePolicy(
            QSizePolicy::Preferred, QSizePolicy::Preferred,
            d->m_formats_chosen_lv->sizePolicy().hasHeightForWidth());
          d->m_formats_chosen_lv->setSorting(-1);
          d->m_formats_chosen_lv->setAllColumnsShowFocus(true);
          d->m_formats_chosen_lv->viewport()->setAcceptDrops(true);
          d->m_formats_chosen_lv->setDragAutoScroll(true);
          d->m_formats_chosen_lv->addColumn("Name");
          d->m_formats_chosen_lv->addColumn("Description");
          chosen_layout->addWidget(d->m_formats_chosen_lv);

    QWidget *finishbuttons_layout_widget = new QWidget(
      central_widget,
      "<avd_o_AnalyzerViewOutputFmtSelector(finishbuttons_layout_widget)");
    QHBoxLayout *finishbuttons_layout = new QHBoxLayout(
      finishbuttons_layout_widget, 10, -1,
      "<avd_o_AnalyzerViewOutputFmtSelector(finishbuttons_layout)>");
    top_layout->addWidget(finishbuttons_layout_widget);

      d->m_cancel_button = new QPushButton(
        "Cancel", finishbuttons_layout_widget,
        "<avd_o_AnalyzerViewOutputFmtSelector(cancel_button)>");
      finishbuttons_layout->addWidget(d->m_cancel_button);

      QSpacerItem *finishbuttons_spacer = new QSpacerItem(
        0, 0, QSizePolicy::MinimumExpanding
      );
      finishbuttons_layout->addItem(finishbuttons_spacer);

      d->m_apply_button = new QPushButton(
        "Apply", finishbuttons_layout_widget,
        "<avd_o_AnalyzerViewOutputFmtSelector(apply_button)>");
      finishbuttons_layout->addWidget(d->m_apply_button);

      d->m_okay_button = new QPushButton(
        "Okay", finishbuttons_layout_widget,
        "<avd_o_AnalyzerViewOutputFmtSelector(okay_button)>");
      finishbuttons_layout->addWidget(d->m_okay_button);

      connect(
        d->m_format_menu_lv,
        SIGNAL(dragStartSig(QListViewItem *)),
        this, SLOT(dragStartSlot(QListViewItem *)));
      connect(
        d->m_formats_chosen_lv,
        SIGNAL(dragStartSig(QListViewItem *)),
        this, SLOT(dragStartSlot(QListViewItem *)));
      connect(
        d->m_formats_chosen_lv,
        SIGNAL(deletePressed(QListViewItem *)),
        this, SLOT(deletePressedSlot(QListViewItem *)));
      connect(
        copy_btn, SIGNAL(clicked()),
        this, SLOT(copyButtonSlot()));
      connect(
        d->m_formats_chosen_lv,
        SIGNAL(dropEventSig(QListViewItem *)),
        this, SLOT(dropEventSlot(QListViewItem *)));
}

// arch-tag: implementation file for old analyzer view output format selector
