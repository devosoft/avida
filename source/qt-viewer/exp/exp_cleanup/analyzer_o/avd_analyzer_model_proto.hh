#ifndef AVD_ANALYZER_MODEL_PROTO_HH
#define AVD_ANALYZER_MODEL_PROTO_HH


class cGenotypeBatch;
class avd_o_AnalyzerModel : public cAnalyze {
protected:
  tList<avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> > m_genotype_data_list;
protected:
  bool LoadDetailDump(cString cur_string);
  bool LoadFile(cString cur_string);
  void SetupGenotypeDataList();
public:
  bool loadCurrentGenotypes(cString filename);
  bool loadHistoricGenotypes(cString filename);
  bool open(cString filename);
  bool batchName(int idx, cString name);

  void setCurrentBatchIdx(int idx);

  int getCurrentBatchIdx(void);
  cGenotypeBatch *getBatchAt(int idx);
  tList< avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> > &getGenotypeDataList(void);
  void LoadDataEntryCommands(
    cStringList arg_list,
    tList< avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> > &output_list
  );

  avd_o_AnalyzerModel();
};

#endif

// arch-tag: proto file for old analyzer model
