#ifndef AVD_N_ANALYZER_MENUBAR_HANDLER_HH
#define AVD_N_ANALYZER_MENUBAR_HANDLER_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QMenuBar;
class avdAnalyzerMenuBarHandler : public QObject, public cRetainable {
  Q_OBJECT
public:
  avdAnalyzerMenuBarHandler(
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

// arch-tag: header file for analyzer menubar handler
