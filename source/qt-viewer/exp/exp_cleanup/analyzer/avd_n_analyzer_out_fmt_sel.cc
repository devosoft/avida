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

#ifndef AVD_N_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_HH
#include "analyzer/avd_n_analyzer_data_entry_command_list_item.hh"
#endif
#ifndef AVD_N_ANALYZER_DBG_MSGS_HH
#include "analyzer/avd_n_analyzer_dbg_msgs.hh"
#endif
#ifndef AVD_N_ANALYZER_LISTVIEW_HH
#include "analyzer/avd_n_analyzer_listview.hh"
#endif

#ifndef AVD_N_ANALYZER_OUT_FMT_SEL_HH
#include "analyzer/avd_n_analyzer_out_fmt_sel.hh"
#endif


void avdAnalyzerOutFmtSel::setMenuListView(avdAnalyzerListView *format_menu_lv)
{ SETretainable(m_format_menu_lv, format_menu_lv); }
void avdAnalyzerOutFmtSel::setChoiceListView(avdAnalyzerListView *formats_chosen_lv)
{ SETretainable(m_formats_chosen_lv, formats_chosen_lv); }

QListView *avdAnalyzerOutFmtSel::getMenuListView()
{ return m_format_menu_lv; }
QListView *avdAnalyzerOutFmtSel::getChoiceListView()
{ return m_formats_chosen_lv; }
QPushButton *avdAnalyzerOutFmtSel::getCancelButton()
{ return m_cancel_button; }
QPushButton *avdAnalyzerOutFmtSel::getApplyButton()
{ return m_apply_button; }
QPushButton *avdAnalyzerOutFmtSel::getOkayButton()
{ return m_okay_button; }

void avdAnalyzerOutFmtSel::deletePressedSlot(QListViewItem *lvi){
  if(lvi) delete lvi;
}

/*
Two methods supporting drag-and-drop.
*/
void avdAnalyzerOutFmtSel::dragStartSlot(QListViewItem *lvi){
  m_source_drag_lvi =
    static_cast<avdAnalyzerDataEntryCommandListItem *>(lvi);
}
void avdAnalyzerOutFmtSel::dropEventSlot(QListViewItem *lvi){
  Debug << "entered.";
  if(!getChoiceListView()){
    Error << "null value in m_formats_chosen_lv.";
    return;
  }
  /*
  check whether this just a move within the formats-chosen listview.
  */
  if((getChoiceListView() == m_source_drag_lvi->listView())){
    Debug << "move.";
    /*
    moving the source format item within the chosen list.
    decide whether to place source immediately before or after
    target.
    */
    bool place_source_after_target = true;
    QListViewItemIterator it(lvi);
    while((++it).current()){
      if(m_source_drag_lvi == it.current()){
        place_source_after_target = false;
        break;
    } }
    // place source after target.
    m_source_drag_lvi->moveItem(lvi);
    if(!place_source_after_target)
      // swap if we need to do so.
      lvi->moveItem(m_source_drag_lvi);
  } else {
    /*
    This is a copy from the format-choices listview to the
    formats-chosen listview.
    */
    Debug << "copy.";
    /*
    see whether the source already exists in the formats-chosen
    listview.
    */
    Debug << "src_lvi->getName(): " << m_source_drag_lvi->getName();
    QListViewItem *duplicate = getChoiceListView()->findItem(m_source_drag_lvi->getName(), 0);
    if(duplicate){
      Debug << "duplicate.";
      if(lvi) duplicate->moveItem(lvi);
      else duplicate->moveItem(getChoiceListView()->lastItem());
      getChoiceListView()->setSelected(duplicate, true);
    } else {
      Debug << "non-duplicate.";
      /*
      source isn't duplicated, so we can make a new one.
      */
      avdAnalyzerDataEntryCommandListItem *adec_lvi =
        new avdAnalyzerDataEntryCommandListItem(getChoiceListView());
      adec_lvi->setDataEntryCommand(m_source_drag_lvi->getDataEntryCommand());
      adec_lvi->nameWatched("<avdAnalyzerOutFmtSel::dropEventSlot(adec_lvi)>");
      adec_lvi->setDragEnabled(true);
      adec_lvi->setDropEnabled(true);
      if(lvi) adec_lvi->moveItem(lvi);
      else adec_lvi->moveItem(getChoiceListView()->lastItem());
      getChoiceListView()->setSelected(getChoiceListView()->selectedItem(), false);
      getChoiceListView()->setSelected(adec_lvi, true);
      getChoiceListView()->setCurrentItem(adec_lvi);
    }
  }
  Debug << "done.";
}
void avdAnalyzerOutFmtSel::copyButtonSlot(){
  Debug << "entered.";
  if(getMenuListView()->currentItem()
  && getMenuListView()->currentItem()->isEnabled()
  && getMenuListView()->currentItem()->isSelected()
  ){
    /*
    see whether the source already exists in the formats-chosen
    listview.
    */
    avdAnalyzerDataEntryCommandListItem *src_lvi =
      static_cast<avdAnalyzerDataEntryCommandListItem *>(
        getMenuListView()->currentItem());
    Debug << "src_lvi->getName(): " << src_lvi->getName();
    QListViewItem *duplicate = getChoiceListView()->findItem(
      src_lvi->getName(),
      0);
      //getChoiceListView()->header()->mapToIndex(0));
    if(duplicate){
      /*
      move the duplicate up to the top of the formats-chosen listview.
      */
      getChoiceListView()->takeItem(duplicate);
      getChoiceListView()->insertItem(duplicate);
      getChoiceListView()->setSelected(duplicate, true);
    } else {
      /*
      source isn't duplicated, so we can make a new one.
      */
      avdAnalyzerDataEntryCommandListItem *new_lvi =
        new avdAnalyzerDataEntryCommandListItem(getChoiceListView());
      new_lvi->setDataEntryCommand(src_lvi->getDataEntryCommand());
      new_lvi->nameWatched("<avdAnalyzerOutFmtSel::copyButtonSlot(adec_lvi)>");
      new_lvi->setDragEnabled(true);
      new_lvi->setDropEnabled(true);
      getChoiceListView()->setSelected(getChoiceListView()->selectedItem(), false);
      getChoiceListView()->setSelected(new_lvi, true);
      getChoiceListView()->setCurrentItem(new_lvi);
    }
  }
  Debug << "done.";
}

avdAnalyzerOutFmtSel::~avdAnalyzerOutFmtSel(){
  Debug << "...";
  setMenuListView(0);
  setChoiceListView(0);
}
avdAnalyzerOutFmtSel::avdAnalyzerOutFmtSel(
  QWidget *parent,
  const char *name,
  WFlags f
):QMainWindow(parent, name, f), m_format_menu_lv(0), m_formats_chosen_lv(0)
{
  Debug << "constructor.";

  QWidget *central_widget = new QWidget(
    this, "<avdAnalyzerOutFmtSel(central_widget)>");
  QVBoxLayout *top_layout = new QVBoxLayout(
    central_widget, 10, -1,
    "<avdAnalyzerOutFmtSel(top_layout)>");
  setCentralWidget(central_widget);

    QSplitter *horizontal_splitter = new QSplitter(
      Qt::Horizontal, central_widget,
      "<avdAnalyzerOutFmtSel(horizontal_splitter)>");
    horizontal_splitter->setOpaqueResize(true);
    top_layout->addWidget(horizontal_splitter, 1);

      QWidget *fmt_menu_layout_widget = new QWidget(
        horizontal_splitter,
        "<avdAnalyzerOutFmtSel(fmt_menu_layout_widget)>");
      QHBoxLayout *fmt_menu_layout = new QHBoxLayout(
        fmt_menu_layout_widget, 10, -1,
        "<avdAnalyzerOutFmtSel(selector_layout)>");
      horizontal_splitter->moveToFirst(fmt_menu_layout_widget);

        m_format_menu_lv = new avdAnalyzerListView(
          fmt_menu_layout_widget,
          "<avdAnalyzerOutFmtSel(m_format_menu_lv)>");
        m_format_menu_lv->nameWatched(
          "<avdAnalyzerOutFmtSel::m_format_menu_lv>");
        m_format_menu_lv->setSizePolicy(
          QSizePolicy::Preferred, QSizePolicy::Preferred,
          m_format_menu_lv->sizePolicy().hasHeightForWidth());
        m_format_menu_lv->setSorting(-1);
        m_format_menu_lv->setAllColumnsShowFocus(true);
        m_format_menu_lv->viewport()->setAcceptDrops(false);
        m_format_menu_lv->addColumn("Name");
        m_format_menu_lv->addColumn("Description");
        fmt_menu_layout->addWidget(m_format_menu_lv);

      QWidget *selector_layout_widget = new QWidget(
        horizontal_splitter,
        "<avdAnalyzerOutFmtSel(selector_layout_widget)>");
      QHBoxLayout *selector_layout = new QHBoxLayout(
        selector_layout_widget, 10, -1,
        "<avdAnalyzerOutFmtSel(selector_layout)>");
      horizontal_splitter->moveToLast(selector_layout_widget);

        QVBoxLayout *button_layout = new QVBoxLayout(
          selector_layout, -1,
          "<avdAnalyzerOutFmtSel(button_layout)>");

          QPushButton *copy_btn = new QPushButton(
            "  ->  ", selector_layout_widget);
          button_layout->addWidget(copy_btn);
          copy_btn->setMaximumWidth(copy_btn->sizeHint().width());

        QVBoxLayout *chosen_layout = new QVBoxLayout(
          selector_layout, -1,
          "<avdAnalyzerOutFmtSel(chosen_layout)>");

          m_formats_chosen_lv = new avdAnalyzerListView(
            selector_layout_widget,
            "<avdAnalyzerOutFmtSel(m_formats_chosen_lv)>");
          m_formats_chosen_lv->setSizePolicy(
            QSizePolicy::Preferred, QSizePolicy::Preferred,
            m_formats_chosen_lv->sizePolicy().hasHeightForWidth());
          m_formats_chosen_lv->nameWatched(
            "<avdAnalyzerOutFmtSel::m_formats_chosen_lv>");
          m_formats_chosen_lv->setSorting(-1);
          m_formats_chosen_lv->setAllColumnsShowFocus(true);
          m_formats_chosen_lv->viewport()->setAcceptDrops(true);
          m_formats_chosen_lv->setDragAutoScroll(true);
          m_formats_chosen_lv->addColumn("Name");
          m_formats_chosen_lv->addColumn("Description");
          m_formats_chosen_lv->setAcceptDrops(TRUE);
          // Grr... can't disable multi selection...
          m_formats_chosen_lv->setSelectionMode(QListView::Single);
          m_formats_chosen_lv->setMultiSelection(false);
          chosen_layout->addWidget(m_formats_chosen_lv);

    QWidget *finishbuttons_layout_widget = new QWidget(
      central_widget,
      "<avdAnalyzerOutFmtSel(finishbuttons_layout_widget)");
    QHBoxLayout *finishbuttons_layout = new QHBoxLayout(
      finishbuttons_layout_widget, 10, -1,
      "<avdAnalyzerOutFmtSel(finishbuttons_layout)>");
    top_layout->addWidget(finishbuttons_layout_widget);

      m_cancel_button = new QPushButton(
        "Cancel", finishbuttons_layout_widget,
        "<avdAnalyzerOutFmtSel(cancel_button)>");
      finishbuttons_layout->addWidget(m_cancel_button);

      QSpacerItem *finishbuttons_spacer = new QSpacerItem(
        0, 0, QSizePolicy::MinimumExpanding
      );
      finishbuttons_layout->addItem(finishbuttons_spacer);

      m_apply_button = new QPushButton(
        "Apply", finishbuttons_layout_widget,
        "<avdAnalyzerOutFmtSel(apply_button)>");
      finishbuttons_layout->addWidget(m_apply_button);

      m_okay_button = new QPushButton(
        "Okay", finishbuttons_layout_widget,
        "<avdAnalyzerOutFmtSel(okay_button)>");
      finishbuttons_layout->addWidget(m_okay_button);

      connect(
        m_format_menu_lv,
        SIGNAL(dragStartSig(QListViewItem *)),
        this, SLOT(dragStartSlot(QListViewItem *)));
      connect(
        m_formats_chosen_lv,
        SIGNAL(dragStartSig(QListViewItem *)),
        this, SLOT(dragStartSlot(QListViewItem *)));
      connect(
        m_formats_chosen_lv,
        SIGNAL(deletePressed(QListViewItem *)),
        this, SLOT(deletePressedSlot(QListViewItem *)));
      connect(
        copy_btn, SIGNAL(clicked()),
        this, SLOT(copyButtonSlot()));
      connect(
        m_formats_chosen_lv,
        SIGNAL(dropEventSig(QListViewItem *)),
        this, SLOT(dropEventSlot(QListViewItem *)));
  Debug << "done.";
}

// arch-tag: implementation file for analyzer output format selector gui
