#ifndef AVD_ANALYZER_GENOTYPE_LIST_ITEM_PROTO_HH
#define AVD_ANALYZER_GENOTYPE_LIST_ITEM_PROTO_HH


class cAnalyzeGenotype;
class avd_o_AnalyzerGenotypeListItem : public QListViewItem {
protected:
  cAnalyzeGenotype *m_genotype;
public:
  avd_o_AnalyzerGenotypeListItem(
    cAnalyzeGenotype *genotype,
    QListView *parent
  ):QListViewItem(parent), m_genotype(genotype)
  {}
  cAnalyzeGenotype *getGenotype(){ return m_genotype; }
};

#endif /* AVD_ANALYZER_GENOTYPE_LIST_ITEM_PROTO_HH */

// arch-tag: proto file for old analyzer genotype list item objects
