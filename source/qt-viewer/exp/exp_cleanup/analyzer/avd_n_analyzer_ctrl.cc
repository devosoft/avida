#ifndef QCHECKBOX_H
#include "qcheckbox.h"
#endif
#ifndef QFILEDIALOG_H
#include <qfiledialog.h>
#endif
#ifndef QMESSAGEBOX_H
#include <qmessagebox.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif

#ifndef AVD_N_ANALYZER_BATCHLIST_ITEM_HH
#include "analyzer/avd_n_analyzer_batchlist_item.hh"
#endif
#ifndef AVD_N_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_HH
#include "analyzer/avd_n_analyzer_data_entry_command_list_item.hh"
#endif
#ifndef AVD_N_ANALYZER_DBG_MSGS_HH
#include "analyzer/avd_n_analyzer_dbg_msgs.hh"
#endif
#ifndef AVD_N_ANALYZER_GENOTYPE_LIST_ITEM_HH
#include "analyzer/avd_n_analyzer_genotype_list_item.hh"
#endif
#ifndef AVD_N_ANALYZER_GENOTYPE_SEL_VIEW_HH
#include "analyzer/avd_n_analyzer_genotype_sel_view.hh"
#endif
#ifndef AVD_N_ANALYZER_LISTVIEW_HH
#include "analyzer/avd_n_analyzer_listview.hh"
#endif
#ifndef AVD_N_ANALYZER_MDL_HH
#include "analyzer/avd_n_analyzer_mdl.hh"
#endif
#ifndef AVD_N_ANALYZER_MENUBAR_HANDLER_HH
#include "analyzer/avd_n_analyzer_menubar_handler.hh"
#endif  
#ifndef AVD_N_ANALYZER_OUT_FMT_SEL_HH
#include "analyzer/avd_n_analyzer_out_fmt_sel.hh"
#endif
#ifndef AVD_N_ANALYZER_VIEW_HH
#include "analyzer/avd_n_analyzer_view.hh"
#endif
#ifndef AVD_CONNECTIONS_HH
#include "mc/avd_connections.hh"
#endif

#ifndef AVD_N_ANALYZER_CTRL_HH
#include "analyzer/avd_n_analyzer_ctrl.hh"
#endif


void avdAnalyzerCtrl::setGenotypeSelView(avdAnalyzerGenotypeSelView *view)
{ SETretainable(m_gentype_sel, view); }
void avdAnalyzerCtrl::setView(avdAnalyzerView *view)
{ SETretainable(m_view, view); }
void avdAnalyzerCtrl::setMdl(avdAnalyzerMdl *mdl)
{ SETretainable(m_mdl, mdl); }
void avdAnalyzerCtrl::setLocalMenuBarHdlr(avdAnalyzerMenuBarHandler *local_menubar_hdlr)
{ SETretainable(m_local_menubar_hdlr, local_menubar_hdlr); }
void avdAnalyzerCtrl::setOutFmtSel(avdAnalyzerOutFmtSel *columns_selector)
{ SETretainable(m_columns_selector, columns_selector); }

avdAnalyzerCtrl::avdAnalyzerCtrl(QObject *parent, const char *name)
:QObject(parent, name), m_mdl(0) {
  Debug << "<avdAnalyzerCtrl> ctor.";

  avdAnalyzerView *view = new avdAnalyzerView(0, "<avdAnalyzerCtrl(m_view)>");
  setView(view); view->release();
  getView()->nameWatched("<avdAnalyzerCtrl(m_view)>");
  connect(getView(), SIGNAL(destroyed()), this, SLOT(deleteLater()));

  avdAnalyzerMdl *mdl = new avdAnalyzerMdl();
  if(!mdl) Fatal << "couldn't instantiate analyzer.";
  setMdl(mdl); mdl->release();
  getMdl()->nameWatched("<<avdAnalyzerCtrl(m_mdl)>");

  avdAnalyzerMenuBarHandler *local_menubar_hdlr = new avdAnalyzerMenuBarHandler
  (0, "<avdAnalyzerCtrl(m_local_menubar_hdlr)>");
  setLocalMenuBarHdlr(local_menubar_hdlr); local_menubar_hdlr->release();
  getLocalMenuBarHdlr()->nameWatched("<avdAnalyzerCtrl(m_local_menubar_hdlr)>");

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

  getView()->getGenotypesListView()->addColumn("id");
  getView()->getGenotypesListView()->addColumn("parent_id");
  getView()->getGenotypesListView()->addColumn("merit");
  getView()->getGenotypesListView()->addColumn("fitness");
    
  getMdl()->LoadDataEntryCommands(data_entry_cmd_strings, m_data_entry_cmd_list);
  Debug << "after initializing m_data_entry_cmd_list: m_data_entry_cmd_list.GetSize(): "
    << m_data_entry_cmd_list.GetSize();

  for(int i = 0; i < MAX_BATCHES; i++){ m_free_batch_list[i] = i; }
  m_free_batch_list_idx = 0;
  m_batch_id_ctr = 0;

  Debug << "<avdAnalyzerCtrl> done.";
}
avdAnalyzerCtrl::~avdAnalyzerCtrl(){
  setLocalMenuBarHdlr(0);
  setMdl(0);
  setView(0);
  setOutFmtSel(0);
  setGenotypeSelView(0);
  Debug << "<avdAnalyzerCtrl> dtor.";
}
bool avdAnalyzerCtrl::setup(){
  Debug << "<avdAnalyzerCtrl::setup>";
  emit(doResetMenuBar(getView()->menuBar()));
  getLocalMenuBarHdlr()->setupMenuBar(getView()->menuBar());

  avdConnections cnct(getLocalMenuBarHdlr(), this);
  cnct.add(SIGNAL(loadCurrentGenotypesSig()), SLOT(loadCurrentGenotypesSlot()));
  cnct.add(SIGNAL(loadHistoricGenotypesSig()), SLOT(loadHistoricGenotypesSlot()));
  cnct.add(SIGNAL(openSig()), SLOT(openSlot()));

  cnct.add(SIGNAL(deleteBatchesSig()), SLOT(deleteBatchesSlot()));
  cnct.add(SIGNAL(mergeBatchesSig()), SLOT(mergeBatchesSlot()));
  cnct.add(SIGNAL(duplicateBatchesSig()), SLOT(duplicateBatchesSlot()));

  cnct.add(SIGNAL(findGenotypeSig()), SLOT(findGenotypeSlot()));
  cnct.add(SIGNAL(findLineageSig()), SLOT(findLineageSlot()));
  cnct.add(SIGNAL(findCladeSig()), SLOT(findCladeSlot()));
  cnct.add(SIGNAL(sampleOrganismsSig()), SLOT(sampleOrganismsSlot()));
  cnct.add(SIGNAL(sampleGenotypesSig()), SLOT(sampleGenotypesSlot()));
  cnct.add(SIGNAL(invertSig()), SLOT(invertSlot()));

  cnct.add(SIGNAL(genotypePhenotypeMapSig()), SLOT(genotypePhenotypeMapSlot()));
  cnct.add(SIGNAL(chooseColumnsSig()), SLOT(chooseColumnsSlot()));

  avdConnections cnct2(getView()->getBatchesListView(), this);
  cnct2.add(SIGNAL(itemRenamed(QListViewItem *, int, const QString &)),
    SLOT(batchListItemRenamedSlot(QListViewItem *, int, const QString &)));
  cnct2.add(SIGNAL(selectionChanged(void)),
    SLOT(batchListSelectionChangedSlot(void)));

  getView()->show();
  Debug << "<avdAnalyzerCtrl::setup> done.";
  return true;
}

// menu responders
  //
void avdAnalyzerCtrl::loadCurrentGenotypesSlot()
{ Debug << "loadCurrentGenotypesSlot."; }
void avdAnalyzerCtrl::loadHistoricGenotypesSlot()
{
  Debug << "loadHistoricGenotypesSlot.";
  /*
  Ask user for Historic Genotypes file to open.
  */  
  QString filename = QFileDialog::getOpenFileName(
    QString::null, QString::null, getView(),
    "Load Historic Genotypes", "Choose historic genotypes file to open");

  /*
  Bail early if user cancels Open File dialog.
  */
  if(QString::null == filename){
    Debug << "user has cancelled 'Load Historic Genotypes'; early exit.";
    return;
  }
  
  /*
  User didn't cancel; try to load the user's file.
  */
  m_mdl->setCurrentBatchIdx(m_free_batch_list[m_free_batch_list_idx]);
  bool got_file = m_mdl->loadHistoricGenotypes(filename.ascii());
  
  if(!got_file){ 
    /*
    Oh no! failed to load Historic Genotypes file.  Give user warning
    feedback.
    */
    Error << "Failed to load Historic Genotypes file \"" << filename << "\".";
  } else {
    Debug << "Successfully loaded Historic Genotypes file \"" << filename << "\".";
    /*
    Successfully loaded Historic Genotypes file and created new batch.
    Locate and name the new batch.
    */
    QString batch_name(QString("(Batch%1)").arg(m_batch_id_ctr));
    m_mdl->batchName(m_free_batch_list[m_free_batch_list_idx], batch_name.ascii());
    
    /*
    Create a new CheckListItem for the new batch, with addition data:
    the index into the batch array stored in m_mdl.
    */
    avdAnalyzerBatchListItem *bli = new avdAnalyzerBatchListItem(
      m_free_batch_list[m_free_batch_list_idx],
      getView()->getBatchesListView(),
      batch_name);
    bli->nameWatched("<avdAnalyzerCtrl::loadHistoricGenotypesSlot(bli)>");
    /*
    don't release bli, in violation of the rules, because the QListView
    getView()->getBatchesListView() has taken ownership of bli.
    */
    bli->setRenameEnabled(0, true);

    /*
    Increment unique-id counter, take the new batch off of the free
    batch list, and point m_free_batch_list_idx to the next free batch.
    */
    m_batch_id_ctr++;
    m_free_batch_list[m_free_batch_list_idx] = -1;
    m_free_batch_list_idx++; 
  }
  Debug << "done.";
}
void avdAnalyzerCtrl::openSlot()
{
  Debug << "openSlot.";
  QString filename = QFileDialog::getOpenFileName(
    QString::null, QString::null, getView(),
    "open file dialog", "Choose a file to open");
  if(QString::null == filename){
    Debug << "User has cancelled 'Open'; early exit.";
    return;
  }
  bool got_file = m_mdl->open(filename.ascii());
  if(!got_file) Error << "couldn't open file \"" << filename << "\".";
  Debug << "done.";
}
  //
void avdAnalyzerCtrl::deleteBatchesSlot()
{
  Debug << "deleteBatchesSlot.";
  QListViewItemIterator it(getView()->getBatchesListView());
  while(it.current()){
    avdAnalyzerBatchListItem *bli = dynamic_cast<avdAnalyzerBatchListItem *>(it.current());
    if(bli && bli->isOn()){
      Debug << "delete bli[" << bli->getBatchIndex() << "] named \""
      << getMdl()->getBatchName(bli->getBatchIndex()) << "\".";
      m_free_batch_list_idx--;
      m_free_batch_list[m_free_batch_list_idx] = bli->getBatchIndex();
      delete bli;
      getMdl()->batchPurge(bli->getBatchIndex());
    } else it++;
  }
  Debug << "done.";
}
void avdAnalyzerCtrl::mergeBatchesSlot()
{
  Debug << "mergeBatchesSlot.";
  QString batch_name("(Merge");
  QListViewItemIterator it(getView()->getBatchesListView());
  while(it.current()){
    avdAnalyzerBatchListItem *bli = dynamic_cast<avdAnalyzerBatchListItem *>(it.current());
    if(bli && bli->isOn()){
      Debug << "merging bli[" << bli->getBatchIndex() << "] named \""
      << getMdl()->getBatchName(bli->getBatchIndex()) << "\".";
      getMdl()->batchMerge(m_free_batch_list[m_free_batch_list_idx], bli->getBatchIndex());
      batch_name += "," + QString(getMdl()->getBatchName(bli->getBatchIndex()));
    }
    it++;
  }
  batch_name += ")";
  m_mdl->batchName(m_free_batch_list[m_free_batch_list_idx], batch_name.ascii());
  avdAnalyzerBatchListItem *bli = new avdAnalyzerBatchListItem(
    m_free_batch_list[m_free_batch_list_idx],
    getView()->getBatchesListView(),
    batch_name);
  bli->nameWatched("<avdAnalyzerCtrl::mergeBatchesSlot(bli)>");
  /*
  don't release bli, in violation of the rules, because the QListView
  getView()->getBatchesListView() has taken ownership of bli.
  */
  bli->setRenameEnabled(0, true);
  m_batch_id_ctr++;
  m_free_batch_list[m_free_batch_list_idx] = -1;
  m_free_batch_list_idx++; 
}
void avdAnalyzerCtrl::duplicateBatchesSlot()
{
  Debug << "duplicateBatchesSlot.";
  QListViewItemIterator it(getView()->getBatchesListView());
  while(it.current()){
    avdAnalyzerBatchListItem *bli = dynamic_cast<avdAnalyzerBatchListItem *>(it.current());
    if(bli && bli->isOn()){
      Debug << "duplicating bli[" << bli->getBatchIndex() << "] named \""
      << getMdl()->getBatchName(bli->getBatchIndex()) << "\".";
      getMdl()->batchMerge(m_free_batch_list[m_free_batch_list_idx], bli->getBatchIndex());
      QString batch_name("(Copy," + QString(getMdl()->getBatchName(bli->getBatchIndex())) + ")");
      getMdl()->batchName(m_free_batch_list[m_free_batch_list_idx], batch_name.ascii());
      avdAnalyzerBatchListItem *bli = new avdAnalyzerBatchListItem(
        m_free_batch_list[m_free_batch_list_idx],
        getView()->getBatchesListView(),
        batch_name);
      bli->nameWatched("<avdAnalyzerCtrl::duplicateBatchesSlot(bli)>");
      /*
      don't release bli, in violation of the rules, because the QListView
      getView()->getBatchesListView() has taken ownership of bli.
      */
      bli->setRenameEnabled(0, true);
      m_batch_id_ctr++;
      m_free_batch_list[m_free_batch_list_idx] = -1;
      m_free_batch_list_idx++; 
    }
    it++;
  }
}
  //
void avdAnalyzerCtrl::setupGenotypeSelView(){
  if(!getGenotypeSelView()){
    avdAnalyzerGenotypeSelView *gsv = new avdAnalyzerGenotypeSelView(
      0, "<avdAnalyzerCtrl::findGenotypeSlot(gsv)>");
    setGenotypeSelView(gsv); gsv->release();
    getGenotypeSelView()->nameWatched("<avdAnalyzerCtrl::findGenotypeSlot(gsv)>");
    
  }
  disconnect(getGenotypeSelView()->getOkayBtn(), 0, this, 0);
  getGenotypeSelView()->getListView()->clear();
  getGenotypeSelView()->chooseNumCPUs()->setChecked(false);
  getGenotypeSelView()->chooseTotalCPUs()->setChecked(false);
  getGenotypeSelView()->chooseFitness()->setChecked(false);
  getGenotypeSelView()->chooseMerit()->setChecked(false);
  getGenotypeSelView()->show();
  getGenotypeSelView()->raise();
}
void avdAnalyzerCtrl::findGenotypeSlot()
{
  Debug << "findGenotypeSlot.";
  setupGenotypeSelView();
  getGenotypeSelView()->setCaption("Selection Criteria: Find Genotype");
  connect(getGenotypeSelView()->getOkayBtn(), SIGNAL(clicked()),
    this, SLOT(doFindGenotypeSlot()));
  Debug << "done.";
}
void avdAnalyzerCtrl::findLineageSlot()
{
  Debug << "findLineageSlot.";
  setupGenotypeSelView();
  getGenotypeSelView()->setCaption("Selection Criteria: Find Lineage");
  connect(getGenotypeSelView()->getOkayBtn(), SIGNAL(clicked()),
    this, SLOT(doFindLineageSlot()));
  Debug << "done.";
}
void avdAnalyzerCtrl::findCladeSlot()
{ 
  Debug << "findCladeSlot.";
  setupGenotypeSelView();
  getGenotypeSelView()->setCaption("Selection Criteria: Find Clade");
  connect(getGenotypeSelView()->getOkayBtn(), SIGNAL(clicked()),
    this, SLOT(doFindCladeSlot()));
  Debug << "done.";
}

QString avdAnalyzerCtrl::verifyGenotypeSelView(bool &verifies_ok)
{
  Debug << "verifyGenotypeSelView.";
  QString retval;
  verifies_ok = true;
  if(getGenotypeSelView()->chooseNumCPUs()->isChecked()) retval += "num_cpus ";
  if(getGenotypeSelView()->chooseTotalCPUs()->isChecked()) retval += "total_cpus ";
  if(getGenotypeSelView()->chooseFitness()->isChecked()) retval += "fitness ";
  if(getGenotypeSelView()->chooseMerit()->isChecked()) retval += "merit ";
  QListViewItemIterator it(getGenotypeSelView()->getListView());
  while(it.current()){
    bool ok;
    it.current()->text(0).toUInt(&ok);
    if(!ok) verifies_ok = false;
    else retval += it.current()->text(0) + " ";
  }
  Debug << "done.";
  return retval;
}
void avdAnalyzerCtrl::doFindGenotypeSlot()
{
  Debug << "doFindGenotypeSlot.";
  bool ok;
  QString spec_string = verifyGenotypeSelView(ok);
  if(!ok){}
  else{
    avdAnalyzerBatchListItem *bli =
      dynamic_cast<avdAnalyzerBatchListItem *>(getView()->getBatchesListView()->selectedItem());
    getMdl()->setCurrentBatchIdx(bli->getBatchIndex());
    getMdl()->findGenotype(spec_string);
    batchListSelectionChangedSlot();
  }
  Debug << "done.";
}
void avdAnalyzerCtrl::doFindLineageSlot()
{
  Debug << "doFindLineageSlot.";
  bool ok;
  QString spec_string = verifyGenotypeSelView(ok);
  if(!ok){}
  else{
    avdAnalyzerBatchListItem *bli =
      dynamic_cast<avdAnalyzerBatchListItem *>(getView()->getBatchesListView()->selectedItem());
    getMdl()->setCurrentBatchIdx(bli->getBatchIndex());
    getMdl()->findLineage(spec_string);
    batchListSelectionChangedSlot();
  }
  Debug << "done.";
}
void avdAnalyzerCtrl::doFindCladeSlot()
{
  Debug << "doFindCladeSlot.";
  bool ok;
  QString spec_string = verifyGenotypeSelView(ok);
  if(!ok){}
  else{
    avdAnalyzerBatchListItem *bli =
      dynamic_cast<avdAnalyzerBatchListItem *>(getView()->getBatchesListView()->selectedItem());
    getMdl()->setCurrentBatchIdx(bli->getBatchIndex());
    getMdl()->findClade(spec_string);
    batchListSelectionChangedSlot();
  }
  Debug << "done.";
}

void avdAnalyzerCtrl::sampleOrganismsSlot()
{ Debug << "sampleOrganismsSlot."; }
void avdAnalyzerCtrl::sampleGenotypesSlot()
{ Debug << "sampleGenotypesSlot."; }
void avdAnalyzerCtrl::invertSlot()
{ Debug << "invertSlot."; }
  //
void avdAnalyzerCtrl::genotypePhenotypeMapSlot()
{ Debug << "genotypePhenotypeMapSlot."; }
// batch listview responders
void avdAnalyzerCtrl::batchListItemRenamedSlot(
  QListViewItem *lvi, int, const QString &text)
{
  Debug << "entered.";
  avdAnalyzerBatchListItem *bli = static_cast<avdAnalyzerBatchListItem *>(lvi);
  if(bli){
    Debug << "index " << bli->getBatchIndex() << '.';
    m_mdl->batchName(bli->getBatchIndex(), text.ascii());
    bli->setText(0, QString(
      m_mdl->getBatchAt(bli->getBatchIndex())->Name()));
    /*
    XXX:  current code, in analyze.cc, truncates name to one word if
    full name has more than one word.  should we change that behavior? 
    */
    {
      cString fullname(text.ascii());
      if(fullname.CountNumWords() > 1){
        Info << "Batch name \"" << fullname << "\" will be truncated.";
        QMessageBox::warning(
          getView(),
          "Name was truncated",
          "Current code truncates the batch name\n"
          "to one word if it is longer than one word.",
          QMessageBox::Ok, 0, 0);
      }
    }
  } else {
    Error << "Trying to rename a null avdAnalyzerBatchListItem!";
  }
  Debug << "done.";
}
void avdAnalyzerCtrl::batchListSelectionChangedSlot(void)
{
  Debug << "batchListSelectionChangedSlot.";
  avdAnalyzerBatchListItem *bli = static_cast<avdAnalyzerBatchListItem *>(
    getView()->getBatchesListView()->selectedItem()
  );
  //
  // clear existing rows.
  getView()->getGenotypesListView()->clear();
  
  // Rebuild columns.
  getView()->getGenotypesListView()->setUpdatesEnabled(false);
  getView()->getGenotypesListView()->blockSignals(true);
  tRetainableListIter<avdAnalyzerDataEntryCommand<cAnalyzeGenotype> >
    fc_it(m_data_entry_cmd_list);
  avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command = 0;
  if(bli && getMdl()->getBatchAt(bli->getBatchIndex())){
    tListIterator<cAnalyzeGenotype> it(
      getMdl()->getBatchAt(bli->getBatchIndex())->List());
    it.Reset();
    cAnalyzeGenotype *genotype(0);
    while((genotype = it.Next()) != 0){
      QListViewItem *lvi = new avdAnalyzerGenotypeListItem(
        genotype, getView()->getGenotypesListView());
      fc_it.Reset();
      int col = 0;
      while (fc_it.Next() != 0){
        entry_command = fc_it.Get();
        entry_command->SetTarget(genotype);
        lvi->setText(
          //getView()->getGenotypesListView()->header()->mapToIndex(col),
          col,
          entry_command->text());
        col++;
      }
      Debug << "nother genotype.";
    }
  }
  getView()->getGenotypesListView()->blockSignals(false);
  getView()->getGenotypesListView()->setUpdatesEnabled(true);
  getView()->getGenotypesListView()->update();

  Debug << "done.";
}
void avdAnalyzerCtrl::chooseColumnsSlot()
{
  Debug << "chooseColumnsSlot.";
  if(!getOutFmtSel()){
    avdAnalyzerOutFmtSel *columns_selector = new avdAnalyzerOutFmtSel(
      0, "<avd_o_AnalyzerCtrlData::m_columns_selector>");
    setOutFmtSel(columns_selector);
    columns_selector->release();
    getOutFmtSel()->nameWatched("<avd_o_AnalyzerCtrlData::m_columns_selector>");
    getLocalMenuBarHdlr()->setupMenuBar(getOutFmtSel()->menuBar());
  
    // Populate format-menu listview with analyze formats.
    tRetainableListIter<avdAnalyzerDataEntryBase<cAnalyzeGenotype> >
      fm_it(getMdl()->getGenotypeDataList());
    fm_it.Reset(); 
    while (fm_it.Prev() != 0){
      avdAnalyzerDataEntryCommandListItem *lvi =
        new avdAnalyzerDataEntryCommandListItem(getOutFmtSel()->getMenuListView());
      avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command =
        new avdAnalyzerDataEntryCommand<cAnalyzeGenotype>(fm_it.Get());
      lvi->setDataEntryCommand(entry_command);
      entry_command->nameWatched("<avdAnalyzerCtrl::chooseColumnsSlot(menu entry_command)>");
      entry_command->release();
        //new avdAnalyzerDataEntryCommandListItem(
        //  entry_command, getOutFmtSel()->getMenuListView());
      //lvi->nameWatched("<avdAnalyzerCtrl::chooseColumnsSlot(lvi)>");
      //Debug << "new lvi named.";
      lvi->setDragEnabled(true);
      lvi->setDropEnabled(false);
    }
    
    // Populate formats-chosen listview with analyze formats.
    tRetainableListIter<avdAnalyzerDataEntryCommand<cAnalyzeGenotype> >
      fc_it(m_data_entry_cmd_list);
    fc_it.Reset();
    while (fc_it.Prev() != 0){
      avdAnalyzerDataEntryCommandListItem *lvi =
        new avdAnalyzerDataEntryCommandListItem(getOutFmtSel()->getChoiceListView());
      avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command =
        new avdAnalyzerDataEntryCommand<cAnalyzeGenotype>(*fc_it.Get());
      lvi->setDataEntryCommand(entry_command);
      entry_command->nameWatched("<avdAnalyzerCtrl::chooseColumnsSlot(choice entry_command)>");
      entry_command->release();
        //new avdAnalyzerDataEntryCommandListItem(
        //  entry_command, getOutFmtSel()->getChoiceListView());
      //lvi->nameWatched("<avdAnalyzerCtrl::chooseColumnsSlot(lvi)>");
      //Debug << "new lvi named.";
      lvi->setDragEnabled(true);
      lvi->setDropEnabled(true);
    }
    connect(getOutFmtSel()->getCancelButton(), SIGNAL(clicked()),
      this, SLOT(selectorCancelBtnClickedSlot()));
    connect(getOutFmtSel()->getApplyButton(), SIGNAL(clicked()),
      this, SLOT(selectorApplyBtnClickedSlot()));
    connect(getOutFmtSel()->getOkayButton(), SIGNAL(clicked()),
      this, SLOT(selectorOkayBtnClickedSlot()));
  }
  getOutFmtSel()->show();
  getOutFmtSel()->raise();
  Debug << "done.";
}
void avdAnalyzerCtrl::updateColumnsSlot()
{
  Debug << "entered.";
  int col = 0;
  getView()->getGenotypesListView()->setUpdatesEnabled(false);
  getView()->getGenotypesListView()->blockSignals(true);
  // find and remove obsolete columns
  tRetainableListIter<avdAnalyzerDataEntryCommand<cAnalyzeGenotype> >
    dec_it(m_data_entry_cmd_list);
  dec_it.Reset();
  while(dec_it.Next()){
    QListViewItem *lvi = m_columns_selector->getChoiceListView()->findItem(
      QString(dec_it.Get()->GetName()), 0);
    if(lvi){ col++; }
    else {
      m_data_entry_cmd_list.Remove(dec_it);
      getView()->getGenotypesListView()->removeColumn(col);
  } }
  // add new columns
  tRetainableList<avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > new_dec_list;
  QListViewItemIterator lvi_it(m_columns_selector->getChoiceListView());
  while(lvi_it.current()){
    avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *dec =
      static_cast<avdAnalyzerDataEntryCommandListItem *>(
        lvi_it.current())->getDataEntryCommand();
    dec_it.Reset();
    bool found_the_dec = false;
    while(dec_it.Next() != 0){
      if(dec_it.Get()->GetName() == dec->GetName()) {
        found_the_dec = true;
        break;
    } }
    if(!found_the_dec){
      getView()->getGenotypesListView()->addColumn(
        QString(dec->GetName()));
      m_data_entry_cmd_list.PushRear(dec);
      new_dec_list.PushRear(dec);
    }
    ++lvi_it;
  }
  // update new columns
  tRetainableListIter<avdAnalyzerDataEntryCommand<cAnalyzeGenotype> >
    new_dec_it(new_dec_list);
  QListViewItemIterator genotype_it(getView()->getGenotypesListView());
  while(genotype_it.current()){
    cAnalyzeGenotype *genotype = static_cast<avdAnalyzerGenotypeListItem *>(
      genotype_it.current())->getGenotype();
    new_dec_it.Reset();
    int new_col = col;
    while(new_dec_it.Next() != 0){
      avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *entry_command = new_dec_it.Get();
      entry_command->SetTarget(genotype);
      genotype_it.current()->setText(
        new_col,
        entry_command->text());
      new_col++;
    }
    genotype_it++;
  } 
  getView()->getGenotypesListView()->blockSignals(false);
  getView()->getGenotypesListView()->setUpdatesEnabled(true);
  getView()->getGenotypesListView()->update();

  Debug << "done.";
}
void avdAnalyzerCtrl::selectorCancelBtnClickedSlot()
{ Debug << "selectorCancelBtnClickedSlot."; }
void avdAnalyzerCtrl::selectorApplyBtnClickedSlot()
{
  Debug << "selectorApplyBtnClickedSlot.";
  updateColumnsSlot();
  Debug << "done.";
}
void avdAnalyzerCtrl::selectorOkayBtnClickedSlot()
{
  Debug << "selectorOkayBtnClickedSlot.";
  updateColumnsSlot();
  Debug << "done.";
}

// arch-tag: definition file for analyzer gui controller
