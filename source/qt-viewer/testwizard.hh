#ifndef TESTWIZARD_HH
#define TESTWIZARD_HH

#include <qmap.h>

#include "wizard.h"

class GenesisWrapper;

/*
FIXME:  If I'm going to use this, I should use some hoohah to give an
error on invalid mappings.  -0- K.
*/
class OneToOneMap
: public QObject{
protected:
  QMap<int, int> m_filechoice_to_cbindex;
  QMap<int, int> m_cbindex_to_filechoice;
public:
  OneToOneMap(
    QObject *parent = 0,
    const char *name = 0
  ):QObject(parent, name){}
  void insert(int filechoice, int cbindex);
  int cbindex(int filechoice);
  int filechoice(int cbindex);
};

class TestWizard
: public SetupAssistant
{
  Q_OBJECT
protected:
  int m_argc;
  char **m_argv;

  GenesisWrapper *m_existing_genwrap;
  GenesisWrapper *m_new_simple_genwrap;
  GenesisWrapper *m_modified_simple_genwrap;
  GenesisWrapper *m_new_detailed_genwrap;
  GenesisWrapper *m_modified_detailed_genwrap;

  GenesisWrapper *m_current_genwrap;

  OneToOneMap m_end_condition_map;
  OneToOneMap m_birth_method_map;
  OneToOneMap m_death_method_map;
  OneToOneMap m_alloc_method_map;
  OneToOneMap m_divide_method_map;
  OneToOneMap m_generation_inc_method_map;
  OneToOneMap m_slicing_method_map;
  OneToOneMap m_size_merit_method_map;
  OneToOneMap m_merit_time_map;
  OneToOneMap m_species_recording_map;
  OneToOneMap m_log_genotypes_map;
  OneToOneMap m_log_genealogy_map;
  OneToOneMap m_lineage_creation_method_map;
  OneToOneMap m_debug_level_map;

  OneToOneMap m_on_off_map;

  enum eGenesisType {
    ExistingGenesis = 0,
    NewGenesis = 1,
    ModifiedGenesis = 2
  } e_genesis_type;
  enum eSetupStyle {
    SimpleStyle = 0,
    AdvancedStyle = 1
  } e_setup_style;
  enum eSimpleTopology {
    TwoDimensional = 0,
    WellStirredFluid = 1
  } e_simple_topology;

  typedef QString (TestWizard::*PageFunction)(QWidget *);
  class PageFunctWrap {
  protected:
    static PageFunction s_default_validator_function;
    PageFunction m_validator_function;
  public:
    PageFunctWrap()
    : m_validator_function(s_default_validator_function)
    {}
    PageFunctWrap(PageFunction validator_function)
    : m_validator_function(validator_function)
    {}
    PageFunction operator()(){ return m_validator_function; }
  };
  typedef QMap<QWidget *, PageFunctWrap> PageValidationMap;
  typedef QMap<QWidget *, PageFunctWrap> PageSetupMap;
  PageValidationMap m_vmap;
  PageSetupMap m_smap;
public:
  TestWizard(
    int argc,
    char *argv[],
    QWidget *parent = 0,
    const char *name = 0,
    bool modal = FALSE,
    WFlags f = 0
  );
  QString invalidPageValidator(QWidget *page);
protected:
  QString genesisTypePageSetup(QWidget *page);
  QString genesisTypePageValidator(QWidget *page);
  QString existingGenesisSelectPageSetup(QWidget *page);
  QString existingGenesisSelectPageValidator(QWidget *page);
  QString modifyGenesisSelectPageSetup(QWidget *page);
  QString modifyGenesisSelectPageValidator(QWidget *page);
  QString setupStylePageSetup(QWidget *page);
  QString setupStylePageValidator(QWidget *page);
  QString simpleTopologyPageSetup(QWidget *page);
  QString simpleTopologyPageValidator(QWidget *page);
  QString simple2dSizePageSetup(QWidget *page);
  QString simple2dSizePageValidator(QWidget *page);
  QString simpleWellmixedSizePageSetup(QWidget *page);
  QString simpleWellmixedSizePageValidator(QWidget *page);
  QString simplePopulatePageSetup(QWidget *page);
  QString simplePopulatePageValidator(QWidget *page);
  QString simpleEndconditionPageSetup(QWidget *page);
  QString simpleEndconditionPageValidator(QWidget *page);
  QString simpleEnvironmentPageSetup(QWidget *page);
  QString simpleEnvironmentPageValidator(QWidget *page);
  QString detailedEndconditionPageSetup(QWidget *page);
  QString detailedEndconditionPageValidator(QWidget *page);
  QString detailedWorldsizePageSetup(QWidget *page);
  QString detailedWorldsizePageValidator(QWidget *page);
  QString detailedCPUThreadsPageSetup(QWidget *page);
  QString detailedCPUThreadsPageValidator(QWidget *page);
  QString detailedRandomseedPageSetup(QWidget *page);
  QString detailedRandomseedPageValidator(QWidget *page);
  QString detailedFilepathsPageSetup(QWidget *page);
  QString detailedFilepathsPageValidator(QWidget *page);
  QString detailedBirthsPageSetup(QWidget *page);
  QString detailedBirthsPageValidator(QWidget *page);
  QString detailedDeathsPageSetup(QWidget *page);
  QString detailedDeathsPageValidator(QWidget *page);
  QString detailedDivrestrictionsPageSetup(QWidget *page);
  QString detailedDivrestrictionsPageValidator(QWidget *page);
  QString detailedExemutationsPageSetup(QWidget *page);
  QString detailedExemutationsPageValidator(QWidget *page);
  QString detailedDivmutationsPageSetup(QWidget *page);
  QString detailedDivmutationsPageValidator(QWidget *page);
  QString detailedReversionsPageSetup(QWidget *page);
  QString detailedReversionsPageValidator(QWidget *page);
  QString detailedTimeslicingPageSetup(QWidget *page);
  QString detailedTimeslicingPageValidator(QWidget *page);
  QString detailedMeritsizePageSetup(QWidget *page);
  QString detailedMeritsizePageValidator(QWidget *page);
  QString detailedMerittasksPageSetup(QWidget *page);
  QString detailedMerittasksPageValidator(QWidget *page);
  QString detailedGenotypeinfoPageSetup(QWidget *page);
  QString detailedGenotypeinfoPageValidator(QWidget *page);
  QString detailedSpeciesinfoPageSetup(QWidget *page);
  QString detailedSpeciesinfoPageValidator(QWidget *page);
  QString detailedLogfilesPageSetup(QWidget *page);
  QString detailedLogfilesPageValidator(QWidget *page);
  QString newSavegenesisPageSetup(QWidget *page);
  QString newSavegenesisPageValidator(QWidget *page);
  QString modifiedSavegenesisPageSetup(QWidget *page);
  QString modifiedSavegenesisPageValidator(QWidget *page);

  void appropriatePages(
    eGenesisType genesis_type,
    eSetupStyle setup_style,
    eSimpleTopology simple_topology
  );
  void appropriateButtonStates(
    eGenesisType genesis_type,
    eSetupStyle setup_style,
    eSimpleTopology simple_topology
  );
  void appropriateGenwrap(
    eGenesisType genesis_type,
    eSetupStyle setup_style
  );
  void insertSetupAndValidatorMappings(
    QWidget *page,
    PageFunction setup_function,
    PageFunction validator_function
  );

  void loadGenwrapData(GenesisWrapper *genwrap);
public slots:
    void load();
    void next();
    void accept();
    void reject();

    /*
    FIXME:  replace with real help documentation.  -- kgn
    */
    void helpButtonClicked(void);

    void createNewWorldSlot();
    void useExistingWorldSlot();
    void modifyExistingWorldSlot();

    void useSimpleSetupStyleSlot();
    void useAdvancedSetupStyleSlot();

    void simpleTwoDimensionalSlot();
    void simpleWellMixedFluidSlot();

    void simpleStartcreatureSelectSlot();
    void simpleEnvfileSelectSlot();

    void existingWorldSelectSlot();
    void existingWorkdirSelectSlot();

    void modifyWorldSelectSlot();

    void detailedEnvfileSelectSlot();
    void detailedEventsfileSelectSlot();
    void detailedInstfileSelectSlot();
    void detailedStartcreatureSelectSlot();

    void modifiedWorkdirSelectSlot();
    void modifiedWorldSelectSlot();
    void newWorkdirSelectSlot();
    void newWorldSelectSlot();
};

#endif /* !TESTWIZARD_HH */
