#ifndef AVD_ANALYZER_VIEW_HH
#define AVD_ANALYZER_VIEW_HH

#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif


class avd_o_AnalyzerViewData;
class QListView;
class avd_o_AnalyzerView : public QMainWindow
{
  Q_OBJECT
private:
  avd_o_AnalyzerViewData *d;
public:
  avd_o_AnalyzerView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  ~avd_o_AnalyzerView();
  QListView *getBatchesListView(void);
  QListView *getGenotypesListView(void);
signals:
  void loadCurrentGenotypesSig();
  void loadHistoricGenotypesSig();
  void openSig();

  void deleteBatchesSig();
  void mergeBatchesSig();
  void duplicateBatchesSig();

  void findGenotypeSig();
  void findLineageSig();
  void findCladeSig();
  void sampleOrganismsSig();
  void sampleGenotypesSig();
  void invertSig();

  void genotypePhenotypeMapSig();
  void chooseColumnsSig();
};

#endif

// arch-tag: header file for old analyzer gui view
