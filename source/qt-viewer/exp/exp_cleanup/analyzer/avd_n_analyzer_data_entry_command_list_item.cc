#ifndef AVD_N_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_HH
#include "analyzer/avd_n_analyzer_data_entry_command_list_item.hh"
#endif

avdAnalyzerDataEntryCommandListItem::avdAnalyzerDataEntryCommandListItem(QListView *parent)
:QListViewItem(parent), m_data_entry_command(0) {}
avdAnalyzerDataEntryCommandListItem::~avdAnalyzerDataEntryCommandListItem(){
  setDataEntryCommand(0);
}
void avdAnalyzerDataEntryCommandListItem::setDataEntryCommand(
  avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *data_entry_command
){
  SETretainable(m_data_entry_command, data_entry_command);
  if(getDataEntryCommand()){
    m_name = QString(getDataEntryCommand()->GetName());
    setText(0, m_name);
    setText(1, QString(getDataEntryCommand()->GetDesc()));
  }
}

// arch-tag: definition file for analyzer data entry command list item object
