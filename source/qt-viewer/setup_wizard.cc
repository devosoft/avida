//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_HH
#include "setup_wizard.hh"
#endif

#ifndef GENESIS_HH
#include "genesis.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif
#ifndef SETUP_FILE_HH
#include "setup_file.hh"
#endif
#ifndef SETUP_WIZARD_PAGE_HH
#include "setup_wizard_page.hh"
#endif
#ifndef SETUP_WIZARD_CONFIG_HH
#include "setup_wizard_config.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif

#ifndef QFILE_H
#include <qfile.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QLINEEDIT_H
#include <qlineedit.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif

#include <stdlib.h>
#include <stdio.h>

using namespace std;


void AvidaSetupWizard::done(int r)
{
  Message
  ("<AvidaSetupWizard::done> entered; r: ")(r);

  if(0 == r){
    delete m_genesis;
    exit(0);
  }

  wrapup();
  
  QWizard::done(r);

  delete m_genesis;

  Message("<AvidaSetupWizard::done> done.");
}

bool
AvidaSetupWizard::preparseCommandLine(int argc, char * argv[])
{
  Message("<AvidaSetupWizard::ProcessConfiguration> entered.");

  cSetupAssistantConfig::ProcessConfiguration(*m_argc, *m_argv, *m_genesis);

  Message("<AvidaSetupWizard::ProcessConfiguration> done.");

  return false;
}

AvidaSetupWizard::AvidaSetupWizard(int *argc, char ***argv)
: m_argc(argc), m_argv(argv), m_using_existing_genesis(false)
{
  Message("<AvidaSetupWizard(int *argc, char ***argv)> constructor.");

  m_genesis = new cGenesis();
  m_genesis->SetVerbose();

  /*
  AvidaSetupPage *testpage = new AvidaSetupPage(
    this
  ,
    "description for use in testing."
  ,
    "help for use in testing."
  ,
    "file comment for use in testing."
  );
  addPage(testpage, "test page");
  */

  m_genesis_page = new GenesisPage(
    this
  ,
    "Avida must be configured using the contents of a\n"
    "\"genesis\" file.\n\n"

    "You can create a new genesis file from scratch\n"
    "using this setup assistant, or you can specify an\n"
    "existing genesis file.\n\n"

    "Avida also needs a place to store any files it\n"
    "must generate.  You must specify a \"work\"\n"
    "directory for these generated files."
  ,
    "The \"genesis\" file contains Avida's configuration information,\n"
    "including the dimensions of the Avida world, the birth method\n"
    "for new organisms (e.g., whether they should be placed adjacent\n"
    "to their parents, or anywhere in the Avida world, etc.), the\n"
    "initial organism(s), information about the environment, and when\n"
    "to end an Avida run.\n"
  );
  addPage(m_genesis_page, "Welcome to Avida!");
  connect(
    m_genesis_page, SIGNAL(usingExistingGenesisSig(bool)),
    this, SLOT(usingExistingGenesisSlot(bool))
  );

  m_dimensions_page = new WorldDimensionsPage(
    this
  ,
    "The Avida world is a grid with specific height\n"
    "and width that you can specify.  The current\n"
    "ranges of valid widths and heights are between\n"
    "one and 400, inclusive."
  ,
    "<fill me in...>"
  ,
    "# Width and height of the world in Avida mode.\n"
  ,
    "WORLD-X",
    1, 400,
    60,
    "WORLD-Y",
    1, 400,
    60
  );
  addPage(m_dimensions_page, "Avida World Dimension");

  m_topology_choice_page = new TopologyChoicePage(
    this
  ,
    "When new Avida organism is born, Avida must\n"
    "have a way to choose a place to put the new\n"
    "organism.\n\n"

    "You may specify one of two birth methods:\n"
    "the \"two-dimensional\" birth method, in\n"
    "which Avida places the new organism near its\n"
    "parent (in the cell occupied by the organism\n"
    "with greates Age/Merit ratio in the immediate\n"
    "neigborhood);\n"
    "and the \"well-stirred\" birth method, in\n"
    "which Avida places the new organism in a cell\n"
    "chosen at random from the Avida world."
  ,
    "<fill me in...>"
  ,
    "### Reproduction ###\n"
    "# 2 = Replace oldest organism in neighborhood\n"
    "# 4 = Replace random from entire population\n"
  ,
    "BIRTH_METHOD"
  );
  addPage(m_topology_choice_page, "Birth Method");

  m_populate_choice_page = new PopulateChoicePage(
    this
  ,
    "Avida must place initial organisms into\n"
    "the world; you may specify whether a single\n"
    "organism is placed (\"inject-one\"), or as\n"
    "many organisms as there are cells in the\n"
    "world are placed (\"inject-all\")."
  ,
    "<fill me in...>"
  ,
    "### Initial Population ###\n"
    "# inject-all : place a copy of START_CREATURE in each cell\n"
    "# inject-one : place a START_CREATURE in cell zero\n"
  );
  addPage(m_populate_choice_page, "Population Method");

  m_end_condition_page = new EndConditionPage(
    this
  ,
    "Avida must be told when to stop running.\n\n"

    "Avida keeps track of the average number of\n"
    "generations of organisms born.  Avida also\n"
    "manages \"time\" in terms of \"updates\", where\n"
    "in each update, certain organisms are chosen\n"
    "to receive more processing time.\n\n"

    "You may specify whether Avida should stop\n"
    "running after a specified number of updates\n"
    "or a specified average number of generations."
  ,
    "<fill me in...>"
  ,
    "### End Condition ###\n"
    "# end after N updates, or after average N generations.\n"
  );
  addPage(m_end_condition_page, "End Condition");

  setNextEnabled(m_genesis_page, false);
  setFinishEnabled(m_end_condition_page, true);
}

void
AvidaSetupWizard::testme(void)
{
  Message("<AvidaSetupWizard::testme> entered.");
}

/*
void
AvidaSetupWizard::showPage(QWidget *page)
{
  Message("<AvidaSetupWizard::showPage> entered.");
  QWizard::showPage(page);
  setNextEnabled(page, true);
  if(page == m_end_condition_page) setFinishEnabled(page, true);
}
*/

/*
FIXME:
this really awful hack gives proper access to the environment file,
instruction set file, start creature file, and event file.  asap I want
to change the way configuration works...
-- kaben.
*/
void
AvidaSetupWizard::wrapup(void)
{
  Message("<AvidaSetupWizard::wrapup> entered.");

  if(m_using_existing_genesis){
  #if 0
    int i;
    int number_of_new_args = 14;
    char **new_argv;

    // note:  I never free this malloc'd memory.
    new_argv
      = (char **)malloc((number_of_new_args + *m_argc) * sizeof(char *));

    // copy in application pathname (first argument)...
    new_argv[0] = (*m_argv)[0];

    // insert new arguments...
    new_argv[1] = (char *)strdup("-genesis");
    new_argv[2] = (char *)strdup(
      m_genesis_page->existingGenesisFilename().latin1()
    );

    new_argv[3] = (char *)strdup("-set");
    new_argv[4] = (char *)strdup("ENVIRONMENT_FILE");
    new_argv[5] = (char *)strdup(
      m_genesis_page->environmentFilename().latin1()
    );

    new_argv[6] = (char *)strdup("-set");
    new_argv[7] = (char *)strdup("INST_SET");
    new_argv[8] = (char *)strdup(
      m_genesis_page->instructionSetFilename().latin1()
    );

    new_argv[9] = (char *)strdup("-set");
    new_argv[10] = (char *)strdup("START_CREATURE");
    new_argv[11] = (char *)strdup(
      m_genesis_page->startCreatureFilename().latin1()
    );

    new_argv[12] = (char *)strdup("-set");
    new_argv[13] = (char *)strdup("EVENT_FILE");
    new_argv[14] = (char *)strdup(
      m_genesis_page->eventsFilename().latin1()
    );

    // insert the rest of the original arguments...
    for(
      i = number_of_new_args + 1;
      i < *m_argc + number_of_new_args;
      i++
    ){
      new_argv[i] = (*m_argv)[i - number_of_new_args];
      if(0 == new_argv[i]) break;
    }
    *m_argc += number_of_new_args;
    *m_argv = new_argv;
  #endif
    Message("<AvidaSetupWizard::wrapup> existing genesis.");

    m_genesis->Open(m_genesis_page->existingGenesisFilename().latin1());
    m_genesis->AddInput(
      "DEFAULT_DIR",
      "./"
    );
    m_genesis->AddInput(
      "ENVIRONMENT_FILE",
      m_genesis_page->environmentFilename().latin1()
    );
    m_genesis->AddInput(
      "INST_SET",
      m_genesis_page->instructionSetFilename().latin1()
    );
    m_genesis->AddInput(
      "START_CREATURE",
      m_genesis_page->startCreatureFilename().latin1()
    );
    m_genesis->AddInput(
      "EVENT_FILE",
      m_genesis_page->eventsFilename().latin1()
    );
  } else {
    Message("<AvidaSetupWizard::wrapup> new genesis.");
    QFile genesis_file(m_genesis_page->newGenesisFilename());
    QString working_dirname(m_genesis_page->workingDirname() + '/');
    QString environment_filename(working_dirname + "environment.cfg");
    QString instr_set_filename(working_dirname + "inst_set.default");
    QString start_creature_filename(working_dirname + "organism.default");
    QString events_filename(working_dirname + "events.cfg");
    QString analysis_filename(working_dirname + "analyze.cfg");

    if (genesis_file.open(IO_WriteOnly))
    {
      QTextStream t(&genesis_file);
      t.setEncoding(QTextStream::Latin1);

      t << "# This file includes all the run-time defines..." << endl;
      t << "VERSION_ID 1.6    # Do not change this value!" << endl;
      t << endl;
      t << "### Architecture Variables ###" << endl;
      t << "MAX_UPDATES  -1         # Maximum updates to run simulation (-1 = no limit)" << endl;
      t << "MAX_GENERATIONS -1      # Maximum generations to run simulation (-1 = no limit)" << endl;
      t << "END_CONDITION_MODE 0  # End run when ..." << endl;
      t << "      # 0 = MAX_UPDATES _OR_ MAX_GENERATIONS is reached" << endl;
      t << "      # 1 = MAX_UPDATES _AND_ MAX_GENERATIONS is reached" << endl;

      t << "# Width of the world in Avida mode." << endl;
      t << "WORLD-X " << m_dimensions_page->m_width_field->text() << endl;
      t << "# Height of the world in Avida mode." << endl;
      t << "WORLD-Y " << m_dimensions_page->m_height_field->text() << endl;

      t << "MAX_CPU_THREADS 1 # Number of Threads CPU's can spawn" << endl;
      t << "RANDOM_SEED 0   # Random number seed. (0 for based on time)" << endl;
      t << endl;
      t << "### Configuration Files ###" << endl;
      t << "# Directory in which config files are found" << endl;
      t << "DEFAULT_DIR " << working_dirname << endl;
      t << "# File containing instruction set" << endl;
      t << "INST_SET " << instr_set_filename << endl;
      t << "# File containing list of events during run" << endl;
      t << "EVENT_FILE " << events_filename << endl;
      t << "# File used for analysis mode" << endl;
      t << "ANALYZE_FILE " << analysis_filename << endl;
      t << "# File that describes the environment" << endl;
      t << "ENVIRONMENT_FILE " << environment_filename << endl;
      t << "# Organism to seed the soup" << endl;
      t << "START_CREATURE " << start_creature_filename << endl;
      t << endl;
      t << "### Reproduction ###" << endl;
      //t << "BIRTH_METHOD 4    # 0 = Replace random organism in neighborhood" << endl;
      t << "# 0 = Replace random organism in neighborhood" << endl;
      t << "# 1 = Replace oldest organism in neighborhood" << endl;
      t << "# 2 = Replace largest Age/Merit in neighborhood" << endl;
      t << "# 3 = Place only in empty cells in neighborhood" << endl;
      t << "# 4 = Replace random from entire population (Mass Action)" << endl;
      t << "# 5 = Replace oldest in entire population (like Tierra)" << endl;

      t << "BIRTH_METHOD " << m_topology_choice_page->parameters() << endl;

      t << "DEATH_METHOD 0    # 0 = Never die of old age." << endl;
      t << "      # 1 = Die when inst executed = AGE_LIMIT" << endl;
      t << "      # 2 = Die when inst executed = length * AGE_LIMIT" << endl;
      t << "AGE_LIMIT 5000    # Modifies DEATH_METHOD" << endl;
      t << "ALLOC_METHOD 0    # 0 = Allocated space is set to default instruction." << endl;
      t << "                  # 1 = Set to section of dead genome (Necrophilia)" << endl;
      t << "                  # 2 = Allocated space is set to random instruction." << endl;
      t << "DIVIDE_METHOD 1   # 0 = Divide leaves state of mother untouched." << endl;
      t << "                  # 1 = Divide resets state of mother" << endl;
      t << "                  #     (after the divide, we have 2 children)" << endl;
      t << endl;
      t << "GENERATION_INC_METHOD 1 # 0 = Only the generation of the child is" << endl;
      t << "                        #     increased on divide." << endl;
      t << "      # 1 = Both the generation of the mother and child are" << endl;
      t << "      #     increased on divide (good with DIVIDE_METHOD 1)." << endl;
      t << endl;
      t << "### Divide Restrictions ####" << endl;
      t << "CHILD_SIZE_RANGE 2.0  # Maximal differential between child and parent sizes." << endl;
      t << "MIN_COPIED_LINES 0.5    # Code fraction which must be copied before divide." << endl;
      t << "MIN_EXE_LINES    0.5    # Code fraction which must be executed before divide." << endl;
      t << "REQUIRE_ALLOCATE   1    # Is a an allocate required before a divide? (0/1)" << endl;
      t << "REQUIRED_TASK -1  # Number of task required for successful divide." << endl;
      t << endl;
      t << "### Mutations ###" << endl;
      t << "" << endl;
      t << "# mutations that occur during execution.." << endl;
      t << "POINT_MUT_PROB  0.0     # Mutation rate (per-location per update)" << endl;
      t << "COPY_MUT_PROB   0.0075  # Mutation rate (per copy)." << endl;
      t << endl;
      t << "# mutations that occur on divide..." << endl;
      t << "INS_MUT_PROB    0.0     # Insertion rate (per site, applied on divide)." << endl;
      t << "DEL_MUT_PROB    0.0     # Deletion rate (per site, applied on divide)." << endl;
      t << "DIVIDE_MUT_PROB 0.0     # Mutation rate (per divide)." << endl;
      t << "DIVIDE_INS_PROB 0.05    # Insertion rate (per divide)." << endl;
      t << "DIVIDE_DEL_PROB 0.05    # Deletion rate (per divide)." << endl;
      t << "PARENT_MUT_PROB 0.0     # Per-site, in parent, on divide" << endl;
      t << endl;
      t << "# heads based mutations" << endl;
      t << "# READ_SHIFT_PROB   0.0" << endl;
      t << "# READ INS_PROB     0.0" << endl;
      t << "# READ_DEL_PROB     0.0" << endl;
      t << "# WRITE_SHIFT_PROB  0.0" << endl;
      t << "# WRITE_INS_PROB    0.0" << endl;
      t << "# WRITE_DEL_PROB    0.0" << endl;
      t << endl;
      t << "### Mutation reversions ###" << endl;
      t << "# these slow down avida a lot, and should be set to 0 normally." << endl;
      t << "REVERT_FATAL       0.0  # Should any mutations be reverted on birth?" << endl;
      t << "REVERT_DETRIMENTAL 0.0  #   0.0 to 1.0; Probability of reversion." << endl;
      t << "REVERT_NEUTRAL     0.0" << endl;
      t << "REVERT_BENEFICIAL  0.0" << endl;
      t << endl;
      t << "STERILIZE_FATAL       0.0  # Should any mutations clear (kill) the organism?" << endl;
      t << "STERILIZE_DETRIMENTAL 0.0  #   0.0 to 1.0; Probability of reset." << endl;
      t << "STERILIZE_NEUTRAL     0.0" << endl;
      t << "STERILIZE_BENEFICIAL  0.0" << endl;
      t << "" << endl;
      t << "FAIL_IMPLICIT     0 # Should copies that failed *not* due to mutations" << endl;
      t << "      # be eliminated?" << endl;
      t << endl;
      t << "### Time Slicing ###" << endl;
      t << "AVE_TIME_SLICE 30" << endl;
      t << "SLICING_METHOD 2  # 0 = CONSTANT: all organisms get default..." << endl;
      t << "      # 1 = PROBABILISTIC: Run _prob_ proportional to merit." << endl;
      t << "      # 2 = INTEGRATED: Perfectly integrated deterministic." << endl;
      t << "SIZE_MERIT_METHOD 4 # 0 = off (merit is independent of size)" << endl;
      t << "      # 1 = Merit proportional to copied size" << endl;
      t << "      # 2 = Merit prop. to executed size" << endl;
      t << "      # 3 = Merit prop. to full size" << endl;
      t << "      # 4 = Merit prop. to min of executed or copied size" << endl;
      t << "      # 5 = Merit prop. to sqrt of the minimum size" << endl;
      t << "TASK_MERIT_METHOD 1 # 0 = No task bonuses" << endl;
      t << "      # 1 = Bonus just equals the task bonus" << endl;
      t << endl;
      t << "MAX_LABEL_EXE_SIZE 1  # Max nops marked as executed when labels are used" << endl;
      t << "MERIT_TIME 1            # 0 = Merit Calculated when task completed" << endl;
      t << "            # 1 = Merit Calculated on Divide" << endl;
      t << "MAX_NUM_TASKS_REWARDED -1  # -1 = Unlimited" << endl;
      t << endl;
      t << "### Genotype Info ###" << endl;
      t << "THRESHOLD 3   # Number of organisms in a genotype needed for it" << endl;
      t << "      #   to be considered viable." << endl;
      t << "GENOTYPE_PRINT 0  # 0/1 (off/on) Print out all threshold genotypes?" << endl;
      t << "GENOTYPE_PRINT_DOM 0  # Print out a genotype if it stays dominant for" << endl;
      t << "                        #   this many updates. (0 = off)" << endl;
      t << "SPECIES_THRESHOLD 2     # max failure count for organisms to be same species" << endl;
      t << "SPECIES_RECORDING 0 # 1 = full, 2 = limited search (parent only)" << endl;
      t << "SPECIES_PRINT 0   # 0/1 (off/on) Print out all species?" << endl;
      t << "TEST_CPU_TIME_MOD 20    # Time allocated in test CPUs (multiple of length)" << endl;
      t << "TRACK_MAIN_LINEAGE 0    # Track primary lineage leading to final population?" << endl;
      t << endl;
      t << "### Log Files ###" << endl;
      t << "LOG_CREATURES 0   # 0/1 (off/on) toggle to print file." << endl;
      t << "LOG_GENOTYPES 0   # 0 = off, 1 = print ALL, 2 = print threshold ONLY." << endl;
      t << "LOG_THRESHOLD 0   # 0/1 (off/on) toggle to print file." << endl;
      t << "LOG_SPECIES 0   # 0/1 (off/on) toggle to print file." << endl;
      t << "LOG_GENEOLOGY 0   # 0 = off, 1 = all, 2 = parents only." << endl;
      t << "LOG_LANDSCAPE 0   # 0/1 (off/on) toggle to print file." << endl;
      t << endl;
      t << "LOG_LINEAGES 0          # 0/1 (off/on) to log advantageous mutations" << endl;
      t << "# This one can slow down avida a lot. It is used to get an idea of how" << endl;
      t << "# often an advantageous mutation arises, and where it goes afterwards." << endl;
      t << "# See also LINEAGE_CREATION_METHOD." << endl;
      t << "" << endl;
      t << "LINEAGE_CREATION_METHOD 0" << endl;
      t << "# Lineage creation options are.  Works only when LOG_LINEAGES is set to 1." << endl;
      t << "#   0 = manual creation (on inject, use successive integers as lineage labels)." << endl;
      t << "#   1 = when a child's (potential) fitness is higher than its parent." << endl;
      t << "#   2 = when a child's (potential) fitness is higher than max in population." << endl;
      t << "#   3 = when a child's (potential) fitness is higher than max in dom. lineage" << endl;
      t << "# *and* the child is in the dominant lineage, or (2)" << endl;
      t << "#   4 = when a child's (potential) fitness is higher than max in dom. lineage" << endl;
      t << "# (and that of its own lineage)" << endl;
      t << "#   5 = same as (4)???" << endl;
      t << "#   6 = when a child's (potential) fitness is higher than own lineage." << endl;
      t << endl;
      t << "### Debug ###" << endl;
      t << "DEBUG_LEVEL 2   # 0 = ERRORS ONLY, 1 = WARNINGS, 2 = COMMENTS" << endl;
      t << endl;
      t << "### END ###" << endl;

      genesis_file.close();
    }

    QFile environment_file(environment_filename);
    if (environment_file.open(IO_WriteOnly))
    {
      QTextStream t(&environment_file);
      t.setEncoding(QTextStream::Latin1);


      t << "##############################################################################" << endl;
      t << "#" << endl;
      t << "# Filename: environment.cfg" << endl;
      t << "#" << endl;
      t << "# This is the setup file for the new task/resource system, which takes the" << endl;
      t << "# place of the old 'task_set' and 'resources' files.  Additionally, there" << endl;
      t << "# are several events that can affect the inflow and outflow of resources in" << endl;
      t << "# the environment." << endl;
      t << "# " << endl;
      t << "# Two main keywords are used in this file, RESOURCE and REACTION.  Their" << endl;
      t << "# formats are:" << endl;
      t << "# " << endl;
      t << "#   RESOURCE  name[:...]  {name:...}" << endl;
      t << "#   REACTION  name  task  [process:...]  [requisite:...]" << endl;
      t << "# " << endl;
      t << "# Where 'name' is a unique identifier.  Resources can be detailed by their" << endl;
      t << "# quatities and flows; Reactions are further described by the task that" << endl;
      t << "# triggers them, the processes they perform (including resources used and the" << endl;
      t << "# results of using them), and requisites on when they can occur." << endl;
      t << "# " << endl;
      t << "# All entries on a resource line are names of individual resources.  A" << endl;
      t << "# resource name can be follow by a ':' and then specific aspects can be" << endl;
      t << "# specified." << endl;
      t << "#" << endl;
      t << "#   'inflow' indicates the quantity entering the population" << endl;
      t << "#      each update (spread evenly over the course of the update).   [0.0]" << endl;
      t << "#   'outflow' is a value from 0 to 1, and indicates the fraction of" << endl;
      t << "#      the resource drained out of the population over an update.   [0.0]" << endl;
      t << "#   'initial' the quantity of resource at the start of run          [0.0]" << endl;
      t << "#" << endl;
      t << "# Resources have a quantity at each location in the population, and a" << endl;
      t << "# global quantity depleatable by all organisms.  Typically only global or the" << endl;
      t << "# local will be used alone in single experiment, but if both local and global" << endl;
      t << "# resources are present, local is depleated first.  The resouce name 'infinite'" << endl;
      t << "# is used to refer to an undepleatable resource.  Currently the inflow," << endl;
      t << "# outflow, and initial in the description of the resource only refers to" << endl;
      t << "# the global resources." << endl;
      t << "#" << endl;
      t << "# As an example, to simulate a chemostat, we might have:" << endl;
      t << "#   RESOURCE glucose:outflow=10000:outflow=0.2" << endl;
      t << "# " << endl;
      t << "# This would create a resource called 'glucose' that has a fixed inflow rate of" << endl;
      t << "# 10000 units where 20% flows out every update.  (Leaving a steady state of" << endl;
      t << "# 50,000 units if no organism-consumption occurs)." << endl;
      t << "#" << endl;
      t << "# When describing a reaction, the 'process' portion determines consumption" << endl;
      t << "# of resources, their byproducts, and the resulting bonuses.  There are" << endl;
      t << "# several arguments (sepated by colons; example below) to detail the use of a" << endl;
      t << "# resource.  Default values are in brackets:" << endl;
      t << "# " << endl;
      t << "#   'resource' is the name of the resource consumed.                 [infinite]" << endl;
      t << "#   'value' multiplied by the amount of the resource consumed gives bonus." << endl;
      t << "#      (0.5 may be inefficient, while 5.0 is very efficient.)        [1.0]" << endl;
      t << "#   'type' determines how to apply the bonus.                        [add]" << endl;
      t << "#      add: increase current merit by bonus" << endl;
      t << "#      mult: multiply merit by current bonus (watch for bonus < 1)" << endl;
      t << "#      pow: multiply merit by 2^bonus (positive bonus -> benefit)" << endl;
      t << "#   'max' is the max amount of the resource consumed per occurance.  [1.0]" << endl;
      t << "#   'min' is the min amount of resource required.                    [0.0]" << endl;
      t << "#   'frac' is the max fraction of the resource that can be consumed. [1.0]" << endl;
      t << "#   'product' is the name of the by-product resource.                [none]" << endl;
      t << "#   'conversion' is the conversion rate to product resource.         [1.0]" << endl;
      t << "# " << endl;
      t << "# If no process is given, a single associated process with all default" << endl;
      t << "# settings is assumed.  If multiple process statements are given, *all* are" << endl;
      t << "# acted upon when the reaction is triggered." << endl;
      t << "# " << endl;
      t << "# The 'requisite' entry limits when this reaction can be triggered.  The" << endl;
      t << "# following requisites (in any combination) are possible:" << endl;
      t << "# " << endl;
      t << "#   'reaction' is a reaction that must be triggered first.           [none]" << endl;
      t << "#   'noreaction' is a reaction that must NOT have been triggered.    [none]" << endl;
      t << "#   'min_count' is min times task must be performed for bonus.       [0]" << endl;
      t << "#   'max_count' is max times task can be performed for bonus.        [INT_MAX]" << endl;
      t << "# " << endl;
      t << "# No restructions are present by default.  If there are multiple requisite " << endl;
      t << "# entries, only *one* of them need be satisfied in order to trigger the" << endl;
      t << "# reaction.  Note though that a single requisite entry can have as many" << endl;
      t << "# portions as needed." << endl;
      t << "# " << endl;
      t << "# EXAMPLES:" << endl;
      t << "# " << endl;
      t << "# We could simulate the old system with a file including only lines like:" << endl;
      t << "# " << endl;
      t << "#   REACTION AND logic:2a process:type=mult:value=4.0   requisite:max_count=1" << endl;
      t << "#   REACTION EQU logic:2h process:type=mult:value=32.0  requisite:max_count=1" << endl;
      t << "# " << endl;
      t << "# No RESOURCE statements need be included since only the infinite resource is" << endl;
      t << "# used (by default, since we don't specify another resources' name)" << endl;
      t << "#" << endl;
      t << "# To create an environment with two resources that are converted back and" << endl;
      t << "# forth as tasks are performed, we might have:" << endl;
      t << "# " << endl;
      t << "#   RESOURCE  yummyA" << endl;
      t << "#   RESOURCE  yummyB" << endl;
      t << "#   REACTION  AtoB  gobbleA  process:resource=yummyA:frac=0.001:product=yummyB" << endl;
      t << "#   REACTION  BtoA  gobbleB  process:resource=yummyB:frac=0.001:product=yummyA" << endl;
      t << "# " << endl;
      t << "# A value of 1.0 per reaction is default.  Obviously 'gobbleA' and 'gobbleB'" << endl;
      t << "# would have to be tasks described within avida." << endl;
      t << "# " << endl;
      t << "# A requisite against the other reaction being performed would prevent a" << endl;
      t << "# single organism from garnering both rewards in equal measure." << endl;
      t << "# " << endl;
      t << "# " << endl;
      t << "# There are three main events that directly relate to resources.  These" << endl;
      t << "# can be used to make changes in the quantities of resources locally, and" << endl;
      t << "# cause more dramatic changes during the course of a run.  Global resources" << endl;
      t << "# fluctuations should be described in the resource description, since they" << endl;
      t << "# will update constantly, not just at update changes." << endl;
      t << "# " << endl;
      t << "#   resource_in  res_name  amount  cell_id" << endl;
      t << "# " << endl;
      t << "# 	Introduces resources into the environment.  The 'amount' input" << endl;
      t << "# 	indicates the numerical quatity to input, and 'cell_id' indicates" << endl;
      t << "#	which cell to put the resource in.  If cell_id is -1 (the default)," << endl;
      t << "#	this amount is globally accessable by all organisms.  A -2 indicates" << endl;
      t << "#	that this amount is placed locally into _each_ cell." << endl;
      t << "# " << endl;
      t << "#   resource_out  res_name  fraction  cell_id" << endl;
      t << "# " << endl;
      t << "# 	Removes resources from the environment.  The 'fraction' input is the" << endl;
      t << "# 	portion of the current resource that flows out, and 'cell_id' works" << endl;
      t << "# 	as in the previous event." << endl;
      t << "# " << endl;
      t << "#   resource_decay  from_resource  to_resource  rate  conversion  cell_id" << endl;
      t << "# " << endl;
      t << "# 	Resource decay converts a portion of one resource into another." << endl;
      t << "# 	The 'from_resource' and 'to_resource' inputs determine the resources" << endl;
      t << "# 	involved in this decay process, 'rate' is the portion of the" << endl;
      t << "# 	from resource that decays, and 'conversion' is the the amount of the" << endl;
      t << "# 	to resource that a single unit of the from resource will turn into." << endl;
      t << "# 	The 'cell_id' input works as above." << endl;
      t << "# " << endl;
      t << "#  [ Note: events triggered on an update begin with a 'u' followed by the" << endl;
      t << "#    update number.  Periodic events then have a ':' and their frequency. ]" << endl;
      t << "# " << endl;
      t << "# An environment with a fixed supply of resources that we periodically reset" << endl;
      t << "# the quantities of might simply look like:" << endl;
      t << "#   u 0:5000 resource_out  glucose    1.0" << endl;
      t << "#   u 0:5000 resource_out  maltose    1.0" << endl;
      t << "#   u 0:5000 resource_in   glucose  10000" << endl;
      t << "#   u 0:5000 resource_in   maltose  10000" << endl;
      t << "#" << endl;
      t << "# And left to develop further on its own." << endl;
      t << "# " << endl;
      t << "# Limitations to this system:" << endl;
      t << "# * Only a single resource can be required at a time, and only a single" << endl;
      t << "#   by-product can be produced." << endl;
      t << "#" << endl;
      t << "##############################################################################" << endl;
      t << "" << endl;
      t << "REACTION  NOT  not   process:value=1.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  NAND nand  process:value=1.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  AND  and   process:value=2.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  ORN  orn   process:value=2.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  OR   or    process:value=3.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  ANDN andn  process:value=3.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  NOR  nor   process:value=4.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  XOR  xor   process:value=4.0:type=pow  requisite:max_count=1" << endl;
      t << "REACTION  EQU  equ   process:value=5.0:type=pow  requisite:max_count=1" << endl;

      environment_file.close();
    }

    QFile instr_set_file(instr_set_filename);
    if (instr_set_file.open(IO_WriteOnly))
    {
      QTextStream t(&instr_set_file);
      t.setEncoding(QTextStream::Latin1);


      t << "nop-A      1   # a" << endl;
      t << "nop-B      1   # b" << endl;
      t << "nop-C      1   # c" << endl;
      t << "if-n-equ   1   # d" << endl;
      t << "if-less    1   # e" << endl;
      t << "pop        1   # f" << endl;
      t << "push       1   # g" << endl;
      t << "swap-stk   1   # h" << endl;
      t << "swap       1   # i " << endl;
      t << "shift-r    1   # j" << endl;
      t << "shift-l    1   # k" << endl;
      t << "inc        1   # l" << endl;
      t << "dec        1   # m" << endl;
      t << "add        1   # n" << endl;
      t << "sub        1   # o" << endl;
      t << "nand       1   # p" << endl;
      t << "IO         1   # q   Puts current contents of register and gets new." << endl;
      t << "h-alloc    1   # r   Allocate as much memory as organism can use." << endl;
      t << "h-divide   1   # s   Cuts off everything between the read and write heads" << endl;
      t << "h-copy     1   # t   Combine h-read and h-write" << endl;
      t << "h-search   1   # u   Search for matching template, set flow head & return info" << endl;
      t << "               #   #   if no template, move flow-head here, set size&offset=0." << endl;
      t << "mov-head   1   # v   Move ?IP? head to flow control." << endl;
      t << "jmp-head   1   # w   Move ?IP? head by fixed amount in CX.  Set old pos in CX." << endl;
      t << "get-head   1   # x   Get position of specified head in CX." << endl;
      t << "if-label   1   # y" << endl;
      t << "set-flow   1   # z   Move flow-head to address in ?CX? " << endl;
      t << "" << endl;
      t << "#adv-head   1" << endl;
      t << "#jump-f     1" << endl;
      t << "#jump-b     1" << endl;
      t << "#call       1" << endl;
      t << "#return     1" << endl;
      t << "#if-bit-1   1" << endl;
      t << "#get        1" << endl;
      t << "#put        1" << endl;
      t << "#h-read     1" << endl;
      t << "#h-write    1" << endl;
      t << "#set-head   1" << endl;
      t << "#search-f   1" << endl;
      t << "#search-b   1" << endl;
      t << "" << endl;
      t << "" << endl;
      t << "# Works on multiple nops:  pop  push  inc  dec  IO  adv-head " << endl;
      t << "" << endl;
      t << "# What if we add a new head.  Search will return the location of something," << endl;
      t << "# and put the new head there.  Then set-head will move another head to that" << endl;
      t << "# point.  In the case of the copy loop, it only needs to be set once and" << endl;
      t << "# this will speed up the code quite a bit!" << endl;
      t << "" << endl;
      t << "# Search with no template returns current position (abs line number) in" << endl;
      t << "# genome." << endl;

      instr_set_file.close();
    }

    QFile start_creature_file(start_creature_filename);
    if (start_creature_file.open(IO_WriteOnly))
    {
      QTextStream t(&start_creature_file);
      t.setEncoding(QTextStream::Latin1);


      t << "# ---  Setup  ---" << endl;
      t << "h-alloc    # Allocate space for child" << endl;
      t << "h-search   # Locate the end of the organism" << endl;
      t << "nop-C      #" << endl;
      t << "nop-A      #" << endl;
      t << "mov-head   # Place write-head at beginning of offspring." << endl;
      t << "nop-C      #" << endl;
      t << "" << endl;
      t << "# ---  Copy Loop  ---" << endl;
      t << "h-search   # Mark the beginning of the copy loop" << endl;
      t << "h-copy     # Do the copy" << endl;
      t << "if-label   # If we're done copying...." << endl;
      t << "nop-C      #" << endl;
      t << "nop-A      #" << endl;
      t << "h-divide   #    ...divide!" << endl;
      t << "mov-head   # Otherwise, loop back to the beginning of the copy loop." << endl;
      t << "nop-A      # End label." << endl;
      t << "nop-B      # End label." << endl;

      start_creature_file.close();
    }

    QFile event_file(events_filename);
    if (event_file.open(IO_WriteOnly))
    {
      QTextStream t(&event_file);
      t.setEncoding(QTextStream::Latin1);


      t << "# basic syntax: 'trigger  start:interval:stop  event...' " << endl;
      t << "# (begin|#:all|once|#:end|#) " << endl;
      t << "# " << endl;
      t << "# trigger can be omitted and 'update' is assumed " << endl;
      t << "# " << endl;
      t << "# If trigger is 'i' or 'immediate' timing must be skipped " << endl;
      t << "# Example: i pause " << endl;
      t << "# " << endl;
      t << "# If trigger and timing are skipped, immediate trigger is assumed " << endl;
      t << "# Example: pause " << endl;
      t << "# " << endl;
      t << "# start can be a value or 'begin' " << endl;
      t << "# interval can be a value, 'once', or 'all' (implies every update) " << endl;
      t << "# stop can be a value or 'end' " << endl;
      t << "# " << endl;
      t << "# Examples: " << endl;
      t << "# trigger start event " << endl;
      t << "#	once when trigger passes start " << endl;
      t << "#	same as [start:once:end] " << endl;
      t << "# trigger start:interval event " << endl;
      t << "#	every time after start that interval has been passed (like cycle) " << endl;
      t << "#	same as [start:interval:end] " << endl;
      t << "# trigger start:interval:stop event " << endl;
      t << "#	every time after start that interval has been passed until stop " << endl;
      t << "# " << endl;
      t << "# Examples: " << endl;
      t << "# update  	begin:100:end	calc_consensus 200 " << endl;
      t << "#	from the beginning to the end of the run, every 100 updates ... " << endl;
      t << "# generation  	 1000:all:20000	inject creature.parasite 0 " << endl;
      t << "#	from when generation crosses 1000 to when generation crosses 20000, " << endl;
      t << "#	every update (when events are processed) ... " << endl;
      t << " " << endl;
      t << "# TRIGGERS: " << endl;
      t << "# update (or u) " << endl;
      t << "# generation (or g) " << endl;
      t << " " << endl;
      t << "# EVENTS: " << endl;
      t << "# echo " << endl;
      t << "# pause " << endl;
      t << "# save_clone " << endl;
      t << "# load_clone " << endl;
      t << "# inject " << endl;
      t << "# inject_all " << endl;
      t << "# inject_random " << endl;
      t << "# mod_copy_mut " << endl;
      t << "# set_copy_mut " << endl;
      t << "# set_point_mut " << endl;
      t << "# calc_landscape " << endl;
      t << "# sample_landscape " << endl;
      t << "# random_landscape " << endl;
      t << "# analyze_landscape " << endl;
      t << "# analyze_population " << endl;
      t << "# pairtest_landscape " << endl;
      t << "# pairtest_full_landscape " << endl;
      t << "# calc_consensus " << endl;
      t << "# test_threads " << endl;
      t << "# print_threads " << endl;
      t << "# dump_basic_grid " << endl;
      t << "# dump_fitness_grid " << endl;
      t << "# print_dom " << endl;
      t << "# print_genealogy_tree  (untested but should work) " << endl;
      t << "# print_genealogy_depth (untested but should work) " << endl;
      t << "# exit " << endl;
      t << "# " << endl;
      t << "# print_average_data " << endl;
      t << "# print_error_data " << endl;
      t << "# print_variance_data " << endl;
      t << "# print_dominant_data " << endl;
      t << "# print_stats_data " << endl;
      t << "# print_count_data " << endl;
      t << "# print_totals_data " << endl;
      t << "# print_tasks_data " << endl;
      t << "# print_tasks_exe_data  // count zeros on output! " << endl;
      t << "# print_time_data " << endl;
      t << "# print_resource_data " << endl;
      t << "# apocalypse " << endl;
      t << "# rate_kill " << endl;
      t << " " << endl;

      //t << "u 0	inject_all START_CREATURE	# Inject all the begining " << endl;
      t << m_populate_choice_page->parameters() << endl;

      t << " " << endl;
      t << " " << endl;
      t << "# Print all of the standard data files... " << endl;
      t << "u 10:10 print_dom			# Regularly save the dominant organism " << endl;
      t << "u 10:10:end print_average_data " << endl;
      t << "u 10:10:end print_dominant_data " << endl;
      t << "u 10:10:end print_stats_data " << endl;
      t << "u 10:10:end print_count_data " << endl;
      t << "u 10:10:end print_tasks_data " << endl;
      t << "u 10:10:end print_time_data " << endl;
      t << "u 10:10:end print_resource_data " << endl;
      t << " " << endl;
      t << "# A few data files not printed by default " << endl;
      t << "# u 10:10:end print_error_data " << endl;
      t << "# u 10:10:end print_variance_data " << endl;
      t << "# u 10:10:end print_totals_data " << endl;
      t << "# u 10:10:end print_tasks_exe_data " << endl;
      t << " " << endl;
      t << "# And lets calculate the landscape info by default. " << endl;
      t << "# u 100:100 calc_landscape " << endl;
      t << " " << endl;
      t << "# Setup the exit time and full population data collection. " << endl;
      t << "u 50000:50000 detail_pop " << endl;
      t << "u 50000:50000 dump_historic_pop " << endl;

      //t << "u 100000 exit				# exit " << endl;
      t << m_end_condition_page->parameters() << endl;


      event_file.close();
    }

    QFile analyze_file(analysis_filename);
    if (analyze_file.open(IO_WriteOnly))
    {
      QTextStream t(&analyze_file);
      t.setEncoding(QTextStream::Latin1);


      t << "#############################################################################" << endl;
      t << "# This file is used to setup avida when its in analysis-only mode (by" << endl;
      t << "# running 'avida -a'.  It gives a variety of methods to load organisms" << endl;
      t << "# (specifying the type of each) and gives a list of tests that should be" << endl;
      t << "# performed on all of them.  Multiple batches of genotypes can be loaded" << endl;
      t << "# when needed for specific tests." << endl;
      t << "#" << endl;
      t << "# A minus ('-') is placed in front of those commands not fully implemented." << endl;
      t << "#" << endl;
      t << "# Input formats:" << endl;
      t << "#  LOAD_ORGANISM <file>     -  A normal organism file, as output from avida" << endl;
      t << "#  LOAD_BASE_DUMP <file>    -  A basic dump file..." << endl;
      t << "#  LOAD_DETAIL_DUMP <file>  -  Input from dump_detail or dump_historic." << endl;
      t << "#  LOAD_SEQUENCE <sequence> -  Load a specific sequence into memory." << endl;
      t << "#" << endl;
      t << "#  -LOAD_DOMINANT [file='dominant.dat'] [dir='genebank/'] [freq=1]" << endl;
      t << "#	Load dominant organisms from 'file' that can be found in 'dir'," << endl;
      t << "#	taking only one ever 'freq' lines." << endl;
      t << "#" << endl;
      t << "# Reduction:" << endl;
      t << "#  FIND_GENOTYPE [type='num_cpus']" << endl;
      t << "#	Remove all genotypes but the one selected.  Type indicates which" << endl;
      t << "#	organism to choose.  Options are to find the organism with the" << endl;
      t << "#	maximum in 'num_cpus' (number of organisms at time of printing)," << endl;
      t << "#       'total_cpus' (number of organisms ever), 'fitness', 'merit'.  If" << endl;
      t << "#	a number is given for type, it is used as an id number to indicate" << endl;
      t << "#	the desired genotype." << endl;
      t << "#" << endl;
      t << "#  FIND_LINEAGE [type='num_cpus']" << endl;
      t << "#	Delete everything except the lineage from the chosen organism back" << endl;
      t << "#	to the ancestor.  Type is the same as the FIND command." << endl;
      t << "#" << endl;
      t << "#  SAMPLE_ORGANISMS <fraction>" << endl;
      t << "#	Keep only 'fraction' of organisms in the current batch.  This" << endl;
      t << "#       is done per organism, not per genotype.  Thus, genotypes of high" << endl;
      t << "#       abundance will only have their abundance lowered, while genotypes" << endl;
      t << "#	of abundance 1 will either stay or be removed entirely." << endl;
      t << "#" << endl;
      t << "#  SAMPLE_GENOTYPES <fraction>" << endl;
      t << "#	Keept only fraction of genotypes in the current batch." << endl;
      t << "#" << endl;
      t << "# Output:" << endl;
      t << "#  PRINT [dir='genebank/']" << endl;
      t << "#	Just print the organisms from the current batch as basic files in" << endl;
      t << "#       the directory given." << endl;
      t << "#" << endl;
      t << "#  TRACE [dir='genebank/']" << endl;
      t << "#       Trace all of the organisms and print a listing of their execution." << endl;
      t << "#" << endl;
      t << "#  PRINT_TASKS [file='tasks.dat']" << endl;
      t << "#	Print out the tasks doable by each genotype." << endl;
      t << "#" << endl;
      t << "#  RENAME" << endl;
      t << "#	Change the names of all the genotypes to start at one." << endl;
      t << "#" << endl;
      t << "#  LANDSCAPE [file='landscape.dat'] [dist=1]" << endl;
      t << "#	Do full landscapping on each organism" << endl;
      t << "#" << endl;
      t << "#  FITNESS_MATRIX [depth-limit=4] [fitness-threshold=.9]" << endl;
      t << "#		  [hamming-thrshold=1] [error-rate-min=.005]" << endl;
      t << "#		  [error-rate-max=.05] [error-rate-step=.005]" << endl;
      t << "#		  [vector-output-fmax=1.1] [vector-output-fstep=.1]" << endl;
      t << "#		  [diag_iters=200] [write_ham_vector=0]" << endl;
      t << "#		  [write_full_vector=0]" << endl;
      t << "#       Do the fitness matrix calculation. Detailed information on the" << endl;
      t << "#	different parameters can be found in fitness_matrix.hh." << endl;
      t << "#" << endl;
      t << "#  DETAIL [file='detail.dat'] [format ...]" << endl;
      t << "#       Print out all of the stats for each organism, one per line.  The" << endl;
      t << "#	format indicates the layout of columns in the file.  If the filename" << endl;
      t << "#       given ends in '.html', html formatting will be used." << endl;
      t << "#" << endl;
      t << "#  MAP [dir='phenotype/'] [flags ...] [format ...]" << endl;
      t << "#	Construct a genotype-phenotype array for each organism in the" << endl;
      t << "#	current batch.  The format is the list of stats that you want" << endl;
      t << "#       to include as columns in the array.  Additionally you can have" << endl;
      t << "#       special format flags; the possible flags are 'html' to print output" << endl;
      t << "#       in HTML format, and 'link_maps' to create html links between" << endl;
      t << "#       consecutive genotypes." << endl;
      t << "#" << endl;
      t << "#  HAMMING [file='hamming.dat'] [b1=current] [b2=b1]" << endl;
      t << "#	Calc hamming dist between batches b1 and b2; 'sample' is a" << endl;
      t << "#	fraction between 0.0 and 1.0.  If only one batch is given (or" << endl;
      t << "#	available) calculations are on all pairs within that batch." << endl;
      t << "#" << endl;
      t << "#  LEVENSTEIN [file='lev.dat'] [b1=current] [b2=b1]" << endl;
      t << "#	Calc levenstein dist between batches b1 and b2; 'sample' is a" << endl;
      t << "#	fraction between 0.0 and 1.0.  If only one batch is given (or" << endl;
      t << "#	available) calculations are on all pairs within that batch." << endl;
      t << "#" << endl;
      t << "#  SPECIES [file='species.dat'] [b1=current] [b2=b1]" << endl;
      t << "#	Calc species dist between batches b1 and b2; 'sample' is a" << endl;
      t << "#	fraction between 0.0 and 1.0.  If only one batch is given (or" << endl;
      t << "#	available) calculations are on all pairs within that batch." << endl;
      t << "#	Output:  Batch1Name  Batch2Name  AveDistance  Count  FailCount" << endl;
      t << "#" << endl;
      t << "#  ALIGN" << endl;
      t << "#	Align all sequences; place '_'s in the sequences to align them." << endl;
      t << "#" << endl;
      t << "# Control:" << endl;
      t << "#  SET_BATCH <id>            -  Set the batch ID currently active; default is 0" << endl;
      t << "#  NAME_BATCH <name>         -  Set a name to this batch..." << endl;
      t << "#  TAG_BATCH <tag>           -  Tag current genotypes for later ID" << endl;
      t << "#  PURGE_BATCH [id=current]  -  Remove all genotypes in a batch" << endl;
      t << "#  STATUS                    -  Print status to analyze.log (for debug)" << endl;
      t << "#  DEBUG                     -  Special debug function..." << endl;
      t << "#  VERBOSE                   -  Toggle verbose/minimal log messages." << endl;
      t << "#  SYSTEM <command>          -  Run the following command on command line" << endl;
      t << "#  INCLUDE <file> [file...]  -  Include other files into this one" << endl;
      t << "#" << endl;
      t << "#  DUPLICATE <id1> [id2=current]" << endl;
      t << "#	Copy batch id2 of genotypes into id1.  By default, copy into the" << endl;
      t << "#	current batch." << endl;
      t << "#" << endl;
      t << "#  RECALCULATE" << endl;
      t << "#	Run all of the organisms in the current batch through a test CPU" << endl;
      t << "#	and use those values instead of those read in." << endl;
      t << "#" << endl;
      t << "#" << endl;
      t << "# Output Formats:" << endl;
      t << "#  Several commands (such as DETAIL and MAP) require format parameters" << endl;
      t << "#  to specify what genotypic features should be output.  Before the" << endl;
      t << "#  such commands are used, other collection functions may need to be" << endl;
      t << "#  run." << endl;
      t << "#" << endl;
      t << "#  Allowable formats after a normal load (assuming they are given" << endl;
      t << "#  in the input file) are: 'id' (Genome ID), 'parent_id' (Parent ID)," << endl;
      t << "#  'num_cpus' (Number of CPUs), 'total_cpus' (Total CPUs Ever), 'length'" << endl;
      t << "#  (Genome Length), 'update_born' (Update Born), 'update_dead' (Update" << endl;
      t << "#  Dead), 'depth' (Tree Depth), 'sequence' (Genome Sequence)," << endl;
      t << "#" << endl;
      t << "#  After a RECALCULATE, the additional formats become available:" << endl;
      t << "#  'viable' (Is Viable [0/1]), 'copy_length' (Copied Length), 'exe_length'" << endl;
      t << "#  (Executed Length), 'merit' (Merit), 'comp_merit' (Computational Merit)," << endl;
      t << "#  'gest_time' (Gestation Time), 'efficiency' (Replication Efficiency)," << endl;
      t << "#  'fitness' (Fitness)" << endl;
      t << "#" << endl;
      t << "#  If a FIND_LINEAGE was done before the RECALCULATE, the parent" << endl;
      t << "#  genotype for each regular genotype will be available, enabling the" << endl;
      t << "#  additional formats: 'parent_dist' (Parent Distance), 'comp_merit_ratio'," << endl;
      t << "#  (Computational Merit Ratio [with parent]), 'efficiency_ratio'" << endl;
      t << "#  (Replication Efficiency Ratio [with parent]), 'fitness_ratio' (Fitness" << endl;
      t << "#  Ratio [with parent]), 'parent_muts' (Mutations from Parent)" << endl;
      t << "#  'html.sequence' (Genome Sequence in Color; html format)" << endl;
      t << "#" << endl;
      t << "#  Finally, if an ALIGN is run, one additional format is available:" << endl;
      t << "# 'alignment' (Aligned Sequence)" << endl;
      t << "#" << endl;
      t << "#" << endl;
      t << "# Variables:" << endl;
      t << "#  For the moment, all variables can only be a single character (letter or" << endl;
      t << "#  number) and begin with a $ whenever they need to be translated to their" << endl;
      t << "#  value.  Lowercase letters are global variables, capital letters are" << endl;
      t << "#  local to a function, and number are arguments to a function.  A $$ will" << endl;
      t << "#  act as a single dollersign, if needed." << endl;
      t << "#" << endl;
      t << "#  SET <variable> <value>    - Sets the variable to the value..." << endl;
      t << "#" << endl;
      t << "#  FOREACH <variable> <value> [value ...]" << endl;
      t << "#       Set the variable to each of the values listed, and run the following" << endl;
      t << "#       code until an END is reached." << endl;
      t << "#" << endl;
      t << "#  FORRANGE <variable> <min_value> <max_value> [step_value=1]" << endl;
      t << "#       Set the variable to each of the values between min and max (at steps" << endl;
      t << "#       given), and run the following code until an END is reached." << endl;
      t << "#" << endl;
      t << "# Functions:" << endl;
      t << "#  These functions are currently very primitive with fixed inputs of" << endl;
      t << "#  $0 through $9.  $0 is always the function name, and then there can be up" << endl;
      t << "#  to 9 other arguments passed through.  Once a function is created, it can" << endl;
      t << "#  be run just like any other command." << endl;
      t << "#" << endl;
      t << "#  FUNCTION <name>" << endl;
      t << "#       Create a function of the given name, including all commands up until" << endl;
      t << "#       and END is reached.  These commands are not executed until a RUN" << endl;
      t << "#       command is used." << endl;
      t << "#" << endl;
      t << "#" << endl;
      t << "#  Currently there are no conditionals or mathematical commands in this" << endl;
      t << "#  scripting language." << endl;
      t << "#" << endl;
      t << "#############################################################################" << endl;
      t << "#" << endl;
      t << "# Todo: LOAD_DOMINANT" << endl;
      t << "#	EVAL command for math (perhaps anything inside of [ and ] ?)" << endl;
      t << "#	Write more and better example script (and some libraries?)" << endl;
      t << "#       Write documentation for DETAIL data types" << endl;
      t << "#       Setup task command to take args" << endl;
      t << "#       Place tests in genotype 'Get' methods to do lazy data collection" << endl;
      t << "#" << endl;
      t << "#############################################################################" << endl;
      t << "" << endl;
      t << "# This example script loops through 100 through 109, clears out the current" << endl;
      t << "# batch of genotypes (if there are any) and then loads the new population" << endl;
      t << "# and all direct ancestors in.  From this population, it finds the dominant" << endl;
      t << "# lineage, which it will to perform a number of measurments on.  These" << endl;
      t << "# include landscaping, running them through test_cpus, printing all tasks" << endl;
      t << "# that can be done at each stage of the lineage." << endl;
      t << "" << endl;
      t << "# VERBOSE" << endl;
      t << "# FORRANGE i 100 149" << endl;
      t << "#   PURGE_BATCH" << endl;
      t << "#   SYSTEM gunzip 2input_$i/detail_pop.100000.gz" << endl;
      t << "#   SYSTEM gunzip 2input_$i/historic_dump.100000.gz" << endl;
      t << "#   LOAD_DETAIL_DUMP 2input_$i/detail_pop.100000" << endl;
      t << "#   LOAD_DETAIL_DUMP 2input_$i/historic_dump.100000" << endl;
      t << "#   FIND_LINEAGE" << endl;
      t << "#   LANDSCAPE landscape.$i.dat" << endl;
      t << "#   RECALCULATE" << endl;
      t << "#   DETAIL detail.$i.html depth fitness merit length task.1:binary task.2:binary task.3:binary" << endl;
      t << "#   PRINT_TASKS tasks.$i.dat" << endl;
      t << "#   SYSTEM gzip 2input_$i/detail_pop.100000" << endl;
      t << "#   SYSTEM gzip 2input_$i/historic_dump.100000" << endl;
      t << "# END" << endl;

      analyze_file.close();
    }

    m_genesis->Open(m_genesis_page->newGenesisFilename().latin1());

  }

  // Initialize the configuration data...
  //cSetupAssistantConfig::ProcessConfiguration(*m_argc, *m_argv, *m_genesis);

  Message
  ("<AvidaSetupWizard::wrapup> I am not using \"ProcessConfiguration\"!");

  cSetupAssistantConfig::SetupFiles(*m_genesis);
  cSetupAssistantConfig::Setup(*m_genesis);
  cConfig::SetupMS();

  Message("<AvidaSetupWizard::wrapup> done.");
}

void
AvidaSetupWizard::testSaveToFile(avd_SetupFile *setup_file)
{
  Message("<AvidaSetupWizard::testSaveToFile> entered.");
}

void
AvidaSetupWizard::usingExistingGenesisSlot(bool yep)
{
  Message("<AvidaSetupWizard::usingExistingGenesisSlot> entered.");

  m_using_existing_genesis = yep;
  if(yep){
    Message(" --- yep.");
    // XXX:  disable lotsa pages...  -- kaben.
  } else {
    Message(" --- nope.");
    // XXX:  enable lotsa pages...  -- kaben.
  }
}
