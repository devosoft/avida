#ifndef AVD_N_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_PROTO_HH
#define AVD_N_ANALYZER_DATA_ENTRY_COMMAND_LIST_ITEM_PROTO_HH

class QMimeSource;
class avdAnalyzerDataEntryCommandListItem
: public QListViewItem, public cRetainable {
protected:
  avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *m_data_entry_command;
  QString m_name;
public:
  avdAnalyzerDataEntryCommandListItem(QListView *parent);
  ~avdAnalyzerDataEntryCommandListItem();
  void setDataEntryCommand(avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *data_entry_command);
  avdAnalyzerDataEntryCommand<cAnalyzeGenotype> *getDataEntryCommand(void)
  { return m_data_entry_command; }
  virtual bool acceptDrop(const QMimeSource *mime) const { return true; }
  const QString &getName(){ return m_name; }
};

#endif

// arch-tag: proto file for analyzer data entry command list item object
