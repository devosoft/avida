
#ifndef TESTWIZARD_HH
#include "testwizard.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#ifndef SETUP_GENESIS_HH
#include "setup_genesis.hh"
#endif

#include "file.hh"

#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>


using namespace std;


void
OneToOneMap::insert(int filechoice, int cbindex){
  m_filechoice_to_cbindex[filechoice] = cbindex;
  m_cbindex_to_filechoice[cbindex] = filechoice;
}

int
OneToOneMap::cbindex(int filechoice){
  return m_filechoice_to_cbindex[filechoice];
}

int
OneToOneMap::filechoice(int cbindex){
  return m_cbindex_to_filechoice[cbindex];
}

void
TestWizard::loadGenwrapData(GenesisWrapper *genwrap){
  GenDebug("entered.");

  bool ok;
  int intval;
  double dblval;
  QString strval;


  // m_genesis_type_page
  // m_existing_genesis_select_page
  // m_modify_genesis_select_page
  // m_setup_style_page
  // m_simple_topology_page
  // m_simple_2d_size_page
  // m_simple_wellmixed_size_page
  // m_simple_populate_page
  // m_simple_endcondition_page
  // m_simple_environment_page
  // m_detailed_endcondition_page
  // m_detailed_worldsize_page
  // m_detailed_cputhreads_page
  // m_detailed_randomseed_page
  // m_detailed_filepaths_page
  // m_detailed_births_page
  // m_detailed_deaths_page
  // m_detailed_divrestrictions_page
  // m_detailed_exemutations_page
  // m_detailed_divmutations_page
  // m_detailed_reversions_page
  // m_detailed_timeslicing_page
  // m_detailed_meritsize_page
  // m_detailed_merittasks_page
  // m_detailed_genotypeinfo_page
  // m_detailed_speciesinfo_page
  // m_detailed_logfiles_page
  // m_new_savegenesis_page
  // m_modified_savegenesis_page
}
