//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_GENESIS_HH
#include "setup_genesis.hh"
#endif
#ifndef SETUP_WIZARD_PAGE2_HH
#include "setup_wizard_page2.hh"
#endif
#ifndef SETUP_WIZARD2_HH
#include "setup_wizard2.hh"
#endif
#ifndef SETUP_WIZARD_CONFIG_HH
#include "setup_wizard_config.hh"
#endif

#ifndef FILE_HH
#include "file.hh"
#endif
#ifndef GENESIS_HH
#include "genesis.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif

using namespace std;

AvidaSetupWizard2::AvidaSetupWizard2(
  int argc,
  char *argv[],
  QWidget *parent,
  const char *name,
  bool modal,
  WFlags f
)
: m_argc(argc),
  m_argv(argv),
  QWizard(parent, name, modal, f)
{
  Message("<AvidaSetupWizard2>");

  AvidaSetupPage2 *testpage = new AvidaSetupPage2(
    "description for use in testing."
  , 
    "help for use in testing."
  , 
    this
  , 
    "testpage"
  ,
    0
  );
  addPage(testpage, "test page");

  m_setup_style_page = new SetupStylePage(this, "m_setup_style_page"); 
  connect(
    m_setup_style_page, SIGNAL(existingSetupSig()),
    this, SLOT(existingSetupSlot())
  );
  connect(
    m_setup_style_page, SIGNAL(simpleSetupSig()),
    this, SLOT(simpleSetupSlot())
  );
  connect(
    m_setup_style_page, SIGNAL(detailedSetupSig()),
    this, SLOT(detailedSetupSlot())
  );
  addPage(m_setup_style_page, "m_setup_style_page");


  /*
  branch for existing genesis file
  */
  m_existing_genwrap = new GenesisWrapper();
  m_existing_genwrap->setGenesis(new cGenesis());
  m_existing_genwrap->getGenesis()->SetVerbose();

  m_existing_genesis_set.append(
    m_existing_genesis_page = new ExistingGenesisPage(
      m_existing_genwrap, this, "m_existing_genesis_page"
    )
  );
  connect(
    m_existing_genesis_page, SIGNAL(isValidSig(bool)),
    this, SLOT(validExistingGenesisSlot(bool))
  );
  addPage(m_existing_genesis_page, "m_existing_genesis_page");

  m_existing_genesis_set.append(
    m_existing_workingdir_page = new WorkingDirectoryPage(
      m_existing_genwrap, this, "m_existing_workingdir_page"
    )
  );
  addPage(m_existing_workingdir_page, "m_existing_workingdir_page");

  m_existing_genesis_set.append(
    m_existing_final_page = new ExistingFinalPage(
      m_existing_genwrap, this, "m_existing_final_page"
    )
  );
  addPage(m_existing_final_page, "m_existing_final_page");


  /*
  branch for simple new genesis file
  */
  m_simple_genwrap = new GenesisWrapper();
  m_simple_genwrap->setGenesis(new cGenesis());
  m_simple_genwrap->getGenesis()->SetVerbose();

  m_simple_genesis_set.append(
    m_simple_genesis_page = new NewGenesisPage(
      m_simple_genwrap, this, "m_simple_genesis_page"
    )
  );
  addPage(m_simple_genesis_page, "m_simple_genesis_page");

  m_simple_genesis_set.append(
    m_simple_final_page = new SimpleFinalPage(
      m_simple_genwrap, this, "m_simple_final_page"
    )
  );
  addPage(m_simple_final_page, "m_simple_final_page");


  /*
  branch for detailed new genesis file
  */
  m_detailed_genwrap = new GenesisWrapper();
  m_detailed_genwrap->setGenesis(new cGenesis());
  m_detailed_genwrap->getGenesis()->SetVerbose();

  m_detailed_genesis_set.append(
    m_detailed_genesis_page = new NewGenesisPage(
      m_detailed_genwrap, this, "m_detailed_genesis_page"
    )
  );
  addPage(m_detailed_genesis_page, "m_detailed_genesis_page");

  m_detailed_genesis_set.append(
    m_detailed_final_page = new DetailedFinalPage(
      m_detailed_genwrap, this, "m_detailed_final_page"
    )
  );
  addPage(m_detailed_final_page, "m_detailed_final_page");



  setAppropriate(m_existing_genesis_set, false);
  setAppropriate(m_simple_genesis_set, false);
  setAppropriate(m_detailed_genesis_set, false);

  /*
  what the hell?
  unless these next two lines are the very last in this method, they
  have no effect!  grrr.
  */
  setNextEnabled(m_setup_style_page, false);
  setNextEnabled(m_existing_final_page, false);
  setNextEnabled(m_simple_final_page, false);
  setNextEnabled(m_detailed_final_page, false);

  setFinishEnabled(m_existing_final_page, true);
  setFinishEnabled(m_simple_final_page, true);
  setFinishEnabled(m_detailed_final_page, true);
}


void
AvidaSetupWizard2::setAppropriate(
  QPtrList<AvidaSetupPage2> &page_set,
  bool appropriate
){
  Message("<AvidaSetupWizard2::setAppropriate>");
  AvidaSetupPage2 *page;
  for(page = page_set.first(); page; page = page_set.next())
  {
    Message((appropriate)?("appropriate"):("inappropriate"));
    QWizard::setAppropriate(page, appropriate);
  }
}

void
AvidaSetupWizard2::simpleSetupSlot(){
  Message("<AvidaSetupWizard2::simpleSetupSlot>");

  setAppropriate(m_existing_genesis_set, false);
  setAppropriate(m_simple_genesis_set, true);
  setAppropriate(m_detailed_genesis_set, false);

  setNextEnabled(m_setup_style_page, true);
}

void
AvidaSetupWizard2::detailedSetupSlot(){
  Message("<AvidaSetupWizard2::detailedSetupSlot>");

  setAppropriate(m_existing_genesis_set, false);
  setAppropriate(m_simple_genesis_set, false);
  setAppropriate(m_detailed_genesis_set, true);

  setNextEnabled(m_setup_style_page, true);
}

void
AvidaSetupWizard2::existingSetupSlot(){
  Message("<AvidaSetupWizard2::existingSetupSlot>");

  setAppropriate(m_existing_genesis_set, true);
  setAppropriate(m_simple_genesis_set, false);
  setAppropriate(m_detailed_genesis_set, false);

  setNextEnabled(m_setup_style_page, true);
}

void
AvidaSetupWizard2::validExistingGenesisSlot(bool is_valid){
  Message("<AvidaSetupWizard2::validExistingGenesisSlot>");

  setNextEnabled(m_existing_genesis_page, is_valid);
}

void
AvidaSetupWizard2::accept(void){
  Message("<AvidaSetupWizard2::accept>");

  FinalPage *final_page = (FinalPage *)currentPage();

  if(final_page == m_existing_final_page){
    Message(" --- m_existing_final_page");
    Message(" --- testing <GenesisWrapper::saveGenesis>");
    final_page->genWrap()->saveGenesis("testgenesis");
    Message("<AvidaSetupWizard2::accept> done testing <GenesisWrapper::saveGenesis>");
  } else if (final_page == m_simple_final_page){
    Message(" --- m_simple_final_page");
  } else if (final_page == m_detailed_final_page){
    Message(" --- m_detailed_final_page");
  } else {
    //Message(" --- unexpected final page");
    qFatal("<AvidaSetupWizard2::accept> unexpected final page");
    return;
  }

  cSetupAssistantConfig::SetupFiles(*(final_page->genWrap()->getGenesis()));
  cSetupAssistantConfig::Setup(*(final_page->genWrap()->getGenesis()));
  cConfig::SetupMS();

  QWizard::accept();
}

void
AvidaSetupWizard2::reject(void){
  Message("<AvidaSetupWizard2::reject>");

  exit(0);
}
