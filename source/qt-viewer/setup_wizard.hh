//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_HH
#define SETUP_WIZARD_HH

#include <qwizard.h>

class cGenesis;

class avd_SetupFile;

class GenesisPage;
class WorldDimensionsPage;
class TopologyChoicePage;
class PopulateChoicePage;
class EndConditionPage;

class AvidaSetupWizard : public QWizard
{
Q_OBJECT
protected:
  GenesisPage *m_genesis_page;
  WorldDimensionsPage *m_dimensions_page;
  TopologyChoicePage *m_topology_choice_page;
  PopulateChoicePage *m_populate_choice_page;
  EndConditionPage *m_end_condition_page;

  cGenesis *m_genesis;
  bool m_using_existing_genesis;
  avd_SetupFile *m_test_setup_file;
  int *m_argc;
  char ***m_argv;
public:
  AvidaSetupWizard(int *argc, char ***argv);
  bool preparseCommandLine(int argc, char * argv[]);
  void testme(void);
  //void showPage(QWidget *page);
protected slots:
  void done(int r);
  void wrapup(void);
public slots:
  void testSaveToFile(avd_SetupFile *setup_file);
  void usingExistingGenesisSlot(bool);
};


#endif /* !SETUP_WIZARD_HH */
