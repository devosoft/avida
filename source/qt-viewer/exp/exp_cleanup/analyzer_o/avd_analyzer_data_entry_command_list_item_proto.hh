#ifndef AVD_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_PROTO_HH
#define AVD_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_PROTO_HH


class QDropEvent;
class QMimeSource;
class avd_o_AnalyzerDataEntryCommandListItem
: public QListViewItem
{
protected:
  avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *m_data_entry_command;
  const QString m_name;
public:
  avd_o_AnalyzerDataEntryCommandListItem(
    avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *data_entry_command,
    QListView *parent
  );
  avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> *getDataEntryCommand(void){
    return m_data_entry_command; }
  virtual bool acceptDrop(const QMimeSource *mime) const { return true; }
  const QString &getName(){ return m_name; }
};

#endif

// arch-tag: proto file for old analyzer data entry command list item objects
