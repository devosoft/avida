//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD2_HH
#define SETUP_WIZARD2_HH

#include <qwizard.h>
#include <qptrlist.h>

#include "setup_wizard_page2.hh"

class cGenesis;
class GenesisWrapper;

class SetupStylePage;

class ExistingGenesisPage;
class WorkingDirectoryPage;
class NewGenesisPage;

class FinalPage;
class ExistingFinalPage;
class SimpleFinalPage;
class DetailedFinalPage;


class AvidaSetupWizard2
: public QWizard
{
  Q_OBJECT
protected:
  int m_argc;
  char **m_argv;
  GenesisWrapper *m_existing_genwrap;
  GenesisWrapper *m_simple_genwrap;
  GenesisWrapper *m_detailed_genwrap;

  QPtrList<AvidaSetupPage2> m_existing_genesis_set;
  QPtrList<AvidaSetupPage2> m_simple_genesis_set;
  QPtrList<AvidaSetupPage2> m_detailed_genesis_set;

  SetupStylePage *m_setup_style_page;

  ExistingGenesisPage *m_existing_genesis_page;
  WorkingDirectoryPage *m_existing_workingdir_page;
  ExistingFinalPage *m_existing_final_page;

  NewGenesisPage *m_simple_genesis_page;
  SimpleFinalPage *m_simple_final_page;

  NewGenesisPage *m_detailed_genesis_page;
  DetailedFinalPage *m_detailed_final_page;

public:
  AvidaSetupWizard2(
    int argc,
    char *argv[],
    QWidget *parent = 0,
    const char *name = 0,
    bool modal = FALSE,
    WFlags f = 0
  );
  void setAppropriate(
    QPtrList<AvidaSetupPage2> &page_set,
    bool appropriate
  );
protected slots:
  void existingSetupSlot();
  void simpleSetupSlot();
  void detailedSetupSlot();

  void validExistingGenesisSlot(bool);

  void accept(void);
  void reject(void);
};

#endif /* !SETUP_WIZARD2_HH */
