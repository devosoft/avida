#ifndef AVD_N_ANALYZER_GENOTYPE_LIST_ITEM_PROTO_HH
#define AVD_N_ANALYZER_GENOTYPE_LIST_ITEM_PROTO_HH

class cAnalyzeGenotype;
class avdAnalyzerGenotypeListItem : public QListViewItem, public cRetainable {
protected:
  cAnalyzeGenotype *m_genotype;
public:
  avdAnalyzerGenotypeListItem(
    cAnalyzeGenotype *genotype,
    QListView *parent
  ):QListViewItem(parent), m_genotype(genotype)
  {}
  cAnalyzeGenotype *getGenotype(){ return m_genotype; }
};

#endif

// arch-tag: proto file for analyzer genotype list item object
