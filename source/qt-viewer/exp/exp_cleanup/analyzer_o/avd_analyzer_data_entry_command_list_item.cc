#ifndef AVD_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_HH
#include "avd_analyzer_data_entry_command_list_item.hh"
#endif

avd_o_AnalyzerDataEntryCommandListItem::avd_o_AnalyzerDataEntryCommandListItem(
  avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *data_entry_command,
  QListView *parent
):QListViewItem(parent),
  m_data_entry_command(data_entry_command),
  m_name(data_entry_command->GetName())
{
  setText(0, QString(data_entry_command->GetName()));
  setText(1, QString(data_entry_command->GetDesc()));
}

// arch-tag: implementation file for old analyzer data entry command list item objects
