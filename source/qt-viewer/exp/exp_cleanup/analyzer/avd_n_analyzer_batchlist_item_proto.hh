#ifndef AVD_N_ANALYZER_BATCHLIST_ITEM_PROTO_HH
#define AVD_N_ANALYZER_BATCHLIST_ITEM_PROTO_HH


class avdAnalyzerBatchListItem
: public QCheckListItem, public cRetainable {
protected:
  const unsigned int m_batch_index;
public:
  avdAnalyzerBatchListItem(
    int idx,
    QListView *parent,
    const QString &text
  ):QCheckListItem(
      parent,
      text,
      QCheckListItem::CheckBox
    ),
    m_batch_index(idx)
  {}
  const unsigned int getBatchIndex(void){ return m_batch_index; }
};

#endif

// arch-tag: proto file for analyzer batchlist item object
