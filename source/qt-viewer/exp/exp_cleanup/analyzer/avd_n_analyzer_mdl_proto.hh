#ifndef AVD_N_ANALYZER_MDL_PROTO_HH
#define AVD_N_ANALYZER_MDL_PROTO_HH

class cGenotypeBatch;
class avdAnalyzerMdl : public cAnalyze, public cRetainable {
protected:
  //tList<avdAnalyzerDataEntryBase<cAnalyzeGenotype> > m_genotype_data_list;
  tRetainableList<avdAnalyzerDataEntryBase<cAnalyzeGenotype> > m_genotype_data_list;
protected:
  bool LoadDetailDump(cString cur_string);
  bool LoadFile(cString cur_string);
  void LoadGenotypeDataList(
    cStringList arg_list,
    tRetainableList<avdAnalyzerDataEntryCommand<cAnalyzeGenotype> >&output_list);
  void SetupGenotypeDataList();
  void FindGenotype(cString cur_string);
  void FindLineage(cString cur_string);
  void FindClade(cString cur_string);
public:
  bool loadCurrentGenotypes(cString filename);
  bool loadHistoricGenotypes(cString filename);
  bool open(cString filename);
  void findGenotype(QString cur_string);
  void findLineage(QString cur_string);
  void findClade(QString cur_string);
  void batchPurge(int idx);
  void batchMerge(int into, int src);
  bool batchName(int idx, cString name);
  const cString &getBatchName(int idx);

  void setCurrentBatchIdx(int idx);
  
  int getCurrentBatchIdx(void);
  cGenotypeBatch *getBatchAt(int idx);
  //void LoadDataEntryCommands(
  //  cStringList arg_list, tList< avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > &output_list);
  void LoadDataEntryCommands(
    cStringList arg_list,
    tRetainableList< avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > &output_list
  );
  //tList< avdAnalyzerDataEntryBase<cAnalyzeGenotype> > &getGenotypeDataList(void);
  tRetainableList< avdAnalyzerDataEntryBase<cAnalyzeGenotype> > &getGenotypeDataList(void);

  avdAnalyzerMdl();
};

#endif

// arch-tag: proto file for analyzer model object
