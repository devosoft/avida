#ifndef QFILEDIALOG_H
#include <qfiledialog.h>
#endif
#ifndef QHEADER_H
#include <qheader.h>
#endif
#ifndef QMESSAGEBOX_H
#include <qmessagebox.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif

#ifndef ANALYZE_HH
#include "main/analyze.hh"
#endif
#ifndef STRING_LIST_HH
#include "string_list.hh"
#endif

#ifndef AVD_ANALYZER_BATCHLIST_ITEM_HH
#include "avd_analyzer_batchlist_item.hh"
#endif
#ifndef AVD_ANALYZER_DATA_ENTRY_HH
#include "avd_analyzer_data_entry.hh"
#endif
#ifndef AVD_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_HH
#include "avd_analyzer_data_entry_command_list_item.hh"
#endif
#ifndef AVD_ANALYZER_GENOTYPE_LIST_ITEM_HH
#include "avd_analyzer_genotype_list_item.hh"
#endif
#ifndef AVD_ANALYZER_MODEL_HH
#include "avd_analyzer_model.hh"
#endif
#ifndef AVD_ANALYZER_UTILS_HH
#include "avd_analyzer_utils.hh"
#endif
#ifndef AVD_ANALYZER_VIEW_HH
#include "avd_analyzer_view.hh"
#endif
#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif
#ifndef AVD_ANALYZER_MENUBAR_HANDLER_HH
#include "avd_analyzer_menubar_handler.hh"
#endif
#ifndef AVD_ANALYZER_OUTPUT_FMT_SELECTOR_HH
#include "avd_analyzer_output_fmt_selector.hh"
#endif

#ifndef AVD_ANALYZER_CONTROLLER_DATA_HH
#include "avd_analyzer_controller_data.hh"
#endif


avd_o_AnalyzerCtrlData::avd_o_AnalyzerCtrlData(){
  AnGuiDebug << "entered.";

  //m_data_entry_cmd_list.nameWatched(
  //  "<avd_o_AnalyzerCtrlData(m_data_entry_cmd_list)>");

  m_analyzer_model = new avd_o_AnalyzerModel;
  if(!m_analyzer_model){
    AnGuiFatal << "Couldn't create new avd_o_AnalyzerModel! Out of memory?"; }
  m_analyzer_menubar_handler = new avd_o_AnalyzerMenuBarHandler(
    this, "<avd_o_AnalyzerCtrlData::m_analyzer_menubar_handler>");
  m_analyzer_view = new avd_o_AnalyzerView(
    0, "<avd_o_AnalyzerCtrlData::m_analyzer_view>");
  m_analyzer_menubar_handler->setupMenuBar(m_analyzer_view->menuBar());

  //tListIterator<avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> >
  //  it(m_analyzer_model->getGenotypeDataList());
  //it.Reset();
  //while (it.Prev() != 0){
  //  avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command =
  //    new avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype>(it.Get());
  //  m_data_entry_cmd_list.PushRear(entry_command);
  //}

  /*
  FIXME
  specify initial data entry commands somewhere else, so that they can
  be loaded as 'preferences'.
  -- kgn
  */
  cStringList data_entry_cmd_strings;
  data_entry_cmd_strings.PushRear("id");

  //data_entry_cmd_strings.PushRear("tag");
  data_entry_cmd_strings.PushRear("parent_id");
  //data_entry_cmd_strings.PushRear("parent_dist");
  //data_entry_cmd_strings.PushRear("length");
  data_entry_cmd_strings.PushRear("merit");
  //data_entry_cmd_strings.PushRear("gest_time");
  data_entry_cmd_strings.PushRear("fitness");
  ////data_entry_cmd_strings.PushRear("sequence");
  //data_entry_cmd_strings.PushRear("task_list");
  //data_entry_cmd_strings.PushRear("total_cpus");

  m_analyzer_view->getGenotypesListView()->addColumn("id");
  m_analyzer_view->getGenotypesListView()->addColumn("parent_id");
  m_analyzer_view->getGenotypesListView()->addColumn("merit");
  m_analyzer_view->getGenotypesListView()->addColumn("fitness");

  m_analyzer_model->LoadDataEntryCommands(data_entry_cmd_strings, m_data_entry_cmd_list);
  AnGuiDebug << "after initializing m_data_entry_cmd_list: m_data_entry_cmd_list.GetSize(): "
    << m_data_entry_cmd_list.GetSize();

  for(int i = 0; i < MAX_BATCHES; i++){ m_free_batch_list[i] = i; }
  m_free_batch_list_idx = 0;
  m_batch_id_ctr = 0;

  connect( m_analyzer_menubar_handler, SIGNAL(loadCurrentGenotypesSig()),
    this, SLOT(loadCurrentGenotypesSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(loadHistoricGenotypesSig()),
    this, SLOT(loadHistoricGenotypesSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(openSig()),
    this, SLOT(openSlot()));

  connect( m_analyzer_menubar_handler, SIGNAL(deleteBatchesSig()),
    this, SLOT(deleteBatchesSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(mergeBatchesSig()),
    this, SLOT(mergeBatchesSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(duplicateBatchesSig()),
    this, SLOT(duplicateBatchesSlot()));

  connect( m_analyzer_menubar_handler, SIGNAL(findGenotypeSig()),
    this, SLOT(findGenotypeSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(findLineageSig()),
    this, SLOT(findLineageSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(findCladeSig()),
    this, SLOT(findCladeSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(sampleOrganismsSig()),
    this, SLOT(sampleOrganismsSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(sampleGenotypesSig()),
    this, SLOT(sampleGenotypesSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(invertSig()),
    this, SLOT(invertSlot()));

  connect( m_analyzer_menubar_handler, SIGNAL(genotypePhenotypeMapSig()),
    this, SLOT(genotypePhenotypeMapSlot()));
  connect( m_analyzer_menubar_handler, SIGNAL(chooseColumnsSig()),
    this, SLOT(chooseColumnsSlot()));

  connect(
    m_analyzer_view->getBatchesListView(),
    SIGNAL(itemRenamed(
      QListViewItem *, int, const QString &)),
    this,
    SLOT(batchListItemRenamedSlot(
      QListViewItem *, int, const QString &)));
  connect(
    m_analyzer_view->getBatchesListView(),
    SIGNAL(selectionChanged(void)),
    this,
    SLOT(batchListSelectionChangedSlot(void)));

  AnGuiDebug << "done.";
}
avd_o_AnalyzerCtrlData::~avd_o_AnalyzerCtrlData(){
  AnGuiDebug << "entered.";
  if(m_analyzer_model) delete m_analyzer_model;
  if(m_analyzer_view) delete m_analyzer_view;
  if(m_columns_selector) delete m_columns_selector;
  AnGuiDebug << "done.";
}

void avd_o_AnalyzerCtrlData::loadCurrentGenotypesSlot(){
  AnGuiDebug << "entered.";
  AnGuiDebug << "done.";
}
void avd_o_AnalyzerCtrlData::loadHistoricGenotypesSlot(){
  AnGuiDebug << "entered.";
  /*
  Ask user for Historic Genotypes file to open.
  */
  QString filename = QFileDialog::getOpenFileName(
    QString::null, QString::null, m_analyzer_view,
    "Load Historic Genotypes", "Choose historic genotypes file to open");

  /*
  Bail early if user cancels Open File dialog.
  */
  if(QString::null == filename){
    AnGuiDebug << "user has cancelled 'Load Historic Genotypes'; early exit.";
    return;
  }

  /*
  User didn't cancel; try to load the user's file.
  */
  m_analyzer_model->setCurrentBatchIdx(m_free_batch_list[m_free_batch_list_idx]);
  bool got_file = m_analyzer_model->loadHistoricGenotypes(filename.ascii());

  if(got_file){
    /*
    Successfully loaded Historic Genotypes file and created new batch.
    Locate and name the new batch.
    */
    QString batch_name(QString("Batch%1").arg(m_batch_id_ctr));
    m_analyzer_model->batchName(
      m_free_batch_list[m_free_batch_list_idx],
      batch_name.ascii());

    /*
    Create a new CheckListItem for the new batch, with addition data:
    the index into the batch array stored in m_analyzer_model.
    */
    avd_o_AnalyzerBatchListItem *bli = new avd_o_AnalyzerBatchListItem(
      m_free_batch_list[m_free_batch_list_idx],
      m_analyzer_view->getBatchesListView(),
      batch_name
    );
    bli->setRenameEnabled(0, true);

    /*
    Increment unique-id counter, take the new batch off of the free
    batch list, and point m_free_batch_list_idx to the next free batch.
    */
    m_batch_id_ctr++;
    m_free_batch_list[m_free_batch_list_idx] = -1;
    m_free_batch_list_idx++;
  } else {
    /*
    Oh no! failed to load Historic Genotypes file.  Give user warning
    feedback.
    */
    AnGuiError
    << "Failed to load Historic Genotypes file \"" << filename << "\".";
  }
  AnGuiDebug << "done.";
}
void avd_o_AnalyzerCtrlData::openSlot(){
  AnGuiDebug << "entered.";
  QString filename = QFileDialog::getOpenFileName(
    QString::null, QString::null, m_analyzer_view,
    "open file dialog", "Choose a file to open");
  if(QString::null == filename){
    AnGuiDebug << "User has cancelled 'Open'; early exit.";
    return;
  }
  bool got_file = m_analyzer_model->open(filename.ascii());
  if(!got_file) AnGuiError << "couldn't open file \"" << filename << "\".";

  AnGuiDebug << "done.";
}

void avd_o_AnalyzerCtrlData::chooseColumnsSlot(){
  AnGuiDebug << "entered.";
  if(!m_columns_selector){
    AnGuiDebug << "makinge new selector widget.";

    m_columns_selector = new avd_o_AnalyzerViewOutputFmtSelector(
      0, "<avd_o_AnalyzerCtrlData::m_columns_selector>");
    m_analyzer_menubar_handler->setupMenuBar(
      m_columns_selector->menuBar());

    // Populate format-menu listview with analyze formats.
    tListIterator<avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> >
      fm_it(m_analyzer_model->getGenotypeDataList());
    fm_it.Reset();
    while (fm_it.Prev() != 0){
      avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command =
        new avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype>(fm_it.Get());
      avd_o_AnalyzerDataEntryCommandListItem *lvi =
        new avd_o_AnalyzerDataEntryCommandListItem(
          entry_command, m_columns_selector->getMenuListView());
      lvi->setDragEnabled(true);
      lvi->setDropEnabled(false);
    }

    // Populate formats-chosen listview with analyze formats.
    tListIterator<avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> >
      fc_it(m_data_entry_cmd_list);
    fc_it.Reset();
    while (fc_it.Prev() != 0){
      avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command =
        new avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype>(*fc_it.Get());
      avd_o_AnalyzerDataEntryCommandListItem *lvi =
        new avd_o_AnalyzerDataEntryCommandListItem(
          entry_command, m_columns_selector->getChoiceListView());
      lvi->setDragEnabled(true);
      lvi->setDropEnabled(true);
    }
    connect(
      m_columns_selector->getCancelButton(), 
      SIGNAL(clicked()),
      this, SLOT(selectorCancelButtonClickedSlot()));
    connect(
      m_columns_selector->getApplyButton(), 
      SIGNAL(clicked()),
      this, SLOT(selectorApplyButtonClickedSlot()));
    connect(
      m_columns_selector->getOkayButton(), 
      SIGNAL(clicked()),
      this, SLOT(selectorOkayButtonClickedSlot()));
  }
  m_columns_selector->show();
  m_columns_selector->raise();
  AnGuiDebug << "done.";
}

void avd_o_AnalyzerCtrlData::updateColumnsSlot(){
  AnGuiDebug << "entered.";
  AnGuiDebug << "done.";
}

void avd_o_AnalyzerCtrlData::batchListItemRenamedSlot(
  QListViewItem *lvi,
  int,
  const QString &text
){
  AnGuiDebug << "entered.";
  avd_o_AnalyzerBatchListItem *bli = static_cast<avd_o_AnalyzerBatchListItem *>(lvi);
  if(bli){
    AnGuiDebug << "index " << bli->getBatchIndex() << '.';
    m_analyzer_model->batchName(bli->getBatchIndex(), text.ascii());
    bli->setText(0, QString(
      m_analyzer_model->getBatchAt(bli->getBatchIndex())->Name()));
    /*
    XXX:  current code, in analyze.cc, truncates name to one word if
    full name has more than one word.  should we change that behavior? 
    */
    {
      cString fullname(text.ascii());
      if(fullname.CountNumWords() > 1){
        AnGuiInfo << "Batch name \"" << fullname << "\" will be truncated.";
        QMessageBox::warning(
          m_analyzer_view,
          "Name was truncated",
          "Current code truncates the batch name\n"
          "to one word if it is longer than one word.",
          QMessageBox::Ok, 0, 0);
      }
    }
  } else {
    AnGuiError << "Trying to rename a null avd_o_AnalyzerBatchListItem!";
  }
  AnGuiDebug << "done.";
}

void avd_o_AnalyzerCtrlData::batchListSelectionChangedSlot(void){
  AnGuiDebug << "entered.";
  avd_o_AnalyzerBatchListItem *bli = static_cast<avd_o_AnalyzerBatchListItem *>(
    m_analyzer_view->getBatchesListView()->selectedItem()
  );

  // clear existing rows.
  m_analyzer_view->getGenotypesListView()->clear();

  // Rebuild columns.
  m_analyzer_view->getGenotypesListView()->setUpdatesEnabled(false);
  m_analyzer_view->getGenotypesListView()->blockSignals(true);
  tListIterator<avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> >
    fc_it(m_data_entry_cmd_list);
  avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command = 0;
  if(bli && m_analyzer_model->getBatchAt(bli->getBatchIndex())){
    tListIterator<cAnalyzeGenotype> it(
      m_analyzer_model->getBatchAt(bli->getBatchIndex())->List());
    it.Reset();
    cAnalyzeGenotype *genotype(0);
    while((genotype = it.Next()) != 0){
      QListViewItem *lvi = new avd_o_AnalyzerGenotypeListItem(
        genotype,
        m_analyzer_view->getGenotypesListView());
      fc_it.Reset();
      int col = 0;
      while (fc_it.Next() != 0){
        entry_command = fc_it.Get();
        entry_command->SetTarget(genotype);
        lvi->setText(
          //m_analyzer_view->getGenotypesListView()->header()->mapToIndex(col),
          col,
          entry_command->text());
        col++;
      }
    }
  }
  m_analyzer_view->getGenotypesListView()->blockSignals(false);
  m_analyzer_view->getGenotypesListView()->setUpdatesEnabled(true);
  m_analyzer_view->getGenotypesListView()->update();

  AnGuiDebug << "done.";
}

void avd_o_AnalyzerCtrlData::selectorCancelButtonClickedSlot(){
  AnGuiDebug << "entered.";
  AnGuiDebug << "done.";
}
void avd_o_AnalyzerCtrlData::selectorApplyButtonClickedSlot(){
  AnGuiDebug << "entered.";
  int col = 0;
  m_analyzer_view->getGenotypesListView()->setUpdatesEnabled(false);
  m_analyzer_view->getGenotypesListView()->blockSignals(true);
  // find and remove obsolete columns
  tListIterator<avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> >
    dec_it(m_data_entry_cmd_list);
  dec_it.Reset();
  while(dec_it.Next()){
    QListViewItem *lvi = m_columns_selector->getChoiceListView()->findItem(
      QString(dec_it.Get()->GetName()), 0);
    if(lvi){ col++; }
    else {
      m_data_entry_cmd_list.Remove(dec_it);
      m_analyzer_view->getGenotypesListView()->removeColumn(col);
  } }
  // add new columns
  tList<avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> > new_dec_list;
  QListViewItemIterator lvi_it(m_columns_selector->getChoiceListView());
  while(lvi_it.current()){
    avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *dec =
      static_cast<avd_o_AnalyzerDataEntryCommandListItem *>(
        lvi_it.current())->getDataEntryCommand();
    dec_it.Reset();
    bool found_the_dec = false;
    while(dec_it.Next() != 0){
      if(dec_it.Get()->GetName() == dec->GetName()) {
        found_the_dec = true;
        break;
    } }
    if(!found_the_dec){
      m_analyzer_view->getGenotypesListView()->addColumn(
        QString(dec->GetName()));
      m_data_entry_cmd_list.PushRear(dec);
      new_dec_list.PushRear(dec);
    }
    ++lvi_it;
  }
  // update new columns
  tListIterator<avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> >
    new_dec_it(new_dec_list);
  QListViewItemIterator genotype_it(m_analyzer_view->getGenotypesListView());
  while(genotype_it.current()){
    cAnalyzeGenotype *genotype = static_cast<avd_o_AnalyzerGenotypeListItem *>(
      genotype_it.current())->getGenotype();
    new_dec_it.Reset();
    int new_col = col;
    while(new_dec_it.Next() != 0){
      avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command = new_dec_it.Get();
      entry_command->SetTarget(genotype);
      genotype_it.current()->setText(
        new_col,
        entry_command->text());
      new_col++;
    }
    genotype_it++;
  }
  m_analyzer_view->getGenotypesListView()->blockSignals(false);
  m_analyzer_view->getGenotypesListView()->setUpdatesEnabled(true);
  m_analyzer_view->getGenotypesListView()->update();

  AnGuiDebug << "done.";
}
void avd_o_AnalyzerCtrlData::selectorOkayButtonClickedSlot(){
  AnGuiDebug << "entered.";
  AnGuiDebug << "done.";
}

// arch-tag: implementation file for old analyzer gui controller private data
