#ifndef AVD_ANALYZER_BATCHLIST_ITEM_PROTO_HH
#define AVD_ANALYZER_BATCHLIST_ITEM_PROTO_HH


class avd_o_AnalyzerBatchListItem : public QCheckListItem {
protected:
  const unsigned int m_batch_index;
public:
  avd_o_AnalyzerBatchListItem(
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

// arch-tag: proto file for old analyzer batchlist item object
