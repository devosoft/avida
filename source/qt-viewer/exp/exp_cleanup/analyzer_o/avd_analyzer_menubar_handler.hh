#ifndef AVD_ANALYZER_MENUBAR_HANDLER_HH
#define AVD_ANALYZER_MENUBAR_HANDLER_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif


class QMenuBar;
class avd_o_AnalyzerMenuBarHandler : public QObject {
  Q_OBJECT
public:
  avd_o_AnalyzerMenuBarHandler(
    QObject *parent = 0,
    const char *name = 0
  ):QObject(parent, name){}
  void setupMenuBar(QMenuBar *menubar);
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

// arch-tag: header file for old analyzer menubar handler
