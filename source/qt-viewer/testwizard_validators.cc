
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

#ifndef SETUP_WIZARD_CONFIG_HH
#include "setup_wizard_config.hh"
#endif

#include "file.hh"

#include <qdir.h>
#include <qfileinfo.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>


using namespace std;


QString
TestWizard::genesisTypePageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

  result = title(page);
  return result;
}
QString
TestWizard::genesisTypePageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  // nothing to do here; handled by slots
  //  useExistingWorldSlot()
  //  createNewWorldSlot()
  //  modifyExistingWorldSlot()
  return result;
}


QString
TestWizard::existingGenesisSelectPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  error-checks
    strval = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
    QFileInfo genesis_fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(genesis_fileinfo.exists()){
          if(genesis_fileinfo.isReadable()){
            m_existing_world_lb->setText(genesis_fileinfo.absFilePath());
          } else GenDebug(" --- !genesis_fileinfo.isReadable()");
        } else GenDebug("! --- genesis_fileinfo.exists()");
      } else GenDebug(" --- strval == QString::null");
    } else GenDebug(" --- bad key:  GENESIS_PATH");

    strval = m_current_genwrap->qStringValue("DEFAULT_DIR", ok);
    GenDebug(" --- m_current_genwrap->qStringValue(\"DEFAULT_DIR\", ok):")
      (m_current_genwrap->qStringValue("DEFAULT_DIR", ok));
    QFileInfo working_dirinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(working_dirinfo.exists()){
          if(working_dirinfo.isDir()){
            if(working_dirinfo.isReadable()){
              if(working_dirinfo.isWritable()){
                m_existing_workdir_lb->setText(working_dirinfo.absFilePath());
                GenDebug(" --- working_dirinfo.absFilePath(): ")
                  (working_dirinfo.absFilePath());
              } else GenDebug(" --- !working_dirinfo.isWritable()");
            } else GenDebug(" --- !working_dirinfo.isReadable()");
          } else GenDebug(" --- !working_dirinfo.isDir()");
        } else GenDebug(" --- !working_dirinfo.exists()");
      } else GenDebug(" --- strval == QString::null");
    } else GenDebug(" --- bad key:  DEFAULT_DIR");

  result = title(page);
  return result;
}
// This page validator is reached via the "Finish" button.
QString
TestWizard::existingGenesisSelectPageValidator(QWidget *page)
{
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  QString genesis_filename = m_existing_world_lb->text();
  QFileInfo genesis_fileinfo(genesis_filename);
  if(genesis_filename.isEmpty()){
    result += "no world file chosen\n";
  } else if(!genesis_fileinfo.exists()){
    result += "selected world file does not exist\n";
  } else if(!genesis_fileinfo.isReadable()){
    result += "selected world is not readable\n";
  }

  QString working_dirname = m_existing_workdir_lb->text();
  QFileInfo working_dirinfo(working_dirname);
  if(working_dirname.isEmpty()){
    result += "no work directory chosen\n";
  } else if(!working_dirinfo.exists()){
    result += "work directory does not exist\n";
  } else if(!working_dirinfo.isDir()){
    result += "file chosen instead of directory for work directory\n";
  } else if(!working_dirinfo.isReadable()){
    result += "work directory is not readable\n";
  } else if(!working_dirinfo.isWritable()){
    result += "work directory is not writable\n";
  }

  if (!result.isEmpty()) return result;

  m_current_genwrap->loadGenesis(genesis_fileinfo.absFilePath());
  m_current_genwrap->set(
    "GENESIS_PATH",
    genesis_filename
  );

  QString inst_set_filename = m_current_genwrap->qStringValue("INST_SET", ok);
  QFileInfo inst_set_fileinfo(inst_set_filename);
  if(inst_set_fileinfo.isRelative()){
    GenDebug(" --- inst_set_fileinfo.isRelative()");
    inst_set_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      inst_set_filename
    );
  }
  if(!ok){
    result += "programmer error:  INST_SET is an invalid key\n";
  } else if(inst_set_filename.isEmpty()){
    result
      += "instruction set file specified in world file is blank\n";
  } else if(!inst_set_fileinfo.exists()){
    result
      += "instruction set file specified in world file doesn't exist\n";
  } else if(!inst_set_fileinfo.isReadable()){
    result
      += "instruction set file specified in world file can't be read\n";
  }

  QString start_creature_filename
    = m_current_genwrap->qStringValue("START_CREATURE", ok);
  QFileInfo start_creature_fileinfo(start_creature_filename);
  if(start_creature_fileinfo.isRelative()){
    GenDebug(" --- start_creature_fileinfo.isRelative()");
    start_creature_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      start_creature_filename
    );
  }
  if(!ok){
    result += "programmer error:  START_CREATURE is an invalid key\n";
  } else if(start_creature_filename.isEmpty()){
    result
      += "start creature file specified in world file is blank\n";
  } else if(!start_creature_fileinfo.exists()){
    result
      += "start creature file specified in world file doesn't exist\n";
  } else if(!start_creature_fileinfo.isReadable()){
    result
      += "start creature file specified in world file can't be read\n";
  }

  QString event_file_filename
    = m_current_genwrap->qStringValue("EVENT_FILE", ok);
  QFileInfo event_file_fileinfo(event_file_filename);
  if(event_file_fileinfo.isRelative()){
    GenDebug(" --- event_file_fileinfo.isRelative()");
    event_file_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      event_file_filename
    );
  }
  if(!ok){
    result += "programmer error:  EVENT_FILE is an invalid key\n";
  } else if(event_file_filename.isEmpty()){
    result
      += "event file specified in world file is blank\n";
  } else if(!event_file_fileinfo.exists()){
    result
      += "event file specified in world file doesn't exist\n";
  } else if(!event_file_fileinfo.isReadable()){
    result
      += "event file specified in world file can't be read\n";
  }

  QString environment_filename
    = m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok);
  QFileInfo environment_fileinfo(environment_filename);
  if(environment_fileinfo.isRelative()){
    GenDebug(" --- environment_fileinfo.isRelative()");
    environment_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      environment_filename
    );
  }
  if(!ok){
    result += "programmer error:  ENVIRONMENT_FILE is an invalid key\n";
  } else if(environment_filename.isEmpty()){
    result
      += "environment file specified in world file is blank\n";
  } else if(!environment_fileinfo.exists()){
    result
      += "environment file specified in world file doesn't exist\n";
  } else if(!environment_fileinfo.isReadable()){
    result
      += "environment file specified in world file can't be read\n";
  }

  QDir::setCurrent(working_dirname);

  if (!result.isEmpty()) return result;

  cSetupAssistantConfig::setDefaultDir(
    m_current_genwrap->qStringValue("DEFAULT_DIR", ok).latin1()
  );

  cSetupAssistantConfig::setInstFilename(
    m_current_genwrap->qStringValue("INST_SET", ok).latin1()
  );
  cSetupAssistantConfig::setEventFilename(
    m_current_genwrap->qStringValue("EVENT_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setAnalyzeFilename(
    m_current_genwrap->qStringValue("ANALYZE_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setEnvFilename(
    m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setStartCreatureFilename(
    m_current_genwrap->qStringValue("START_CREATURE", ok).latin1()
  );

  cSetupAssistantConfig::setMaxUpdates(
    m_current_genwrap->intValue("MAX_UPDATES", ok)
  );
  cSetupAssistantConfig::setMaxGenerations(
    m_current_genwrap->intValue("MAX_GENERATIONS", ok)
  );
  cSetupAssistantConfig::setEndConditionMode(
    m_current_genwrap->intValue("END_CONDITION_MODE", ok)
  );
  cSetupAssistantConfig::setWorldX(
    m_current_genwrap->intValue("WORLD-X", ok)
  );
  cSetupAssistantConfig::setWorldY(
    m_current_genwrap->intValue("WORLD-Y", ok)
  );

  cSetupAssistantConfig::setBirthMethod(
    m_current_genwrap->intValue("BIRTH_METHOD", ok)
  );
  cSetupAssistantConfig::setDeathMethod(
    m_current_genwrap->intValue("DEATH_METHOD", ok)
  );
  cSetupAssistantConfig::setAllocMethod(
    m_current_genwrap->intValue("ALLOC_METHOD", ok)
  );
  cSetupAssistantConfig::setDivideMethod(
    m_current_genwrap->intValue("DIVIDE_METHOD", ok)
  );
  cSetupAssistantConfig::setRequiredTask(
    m_current_genwrap->intValue("REQUIRED_TASK", ok)
  );
  cSetupAssistantConfig::setLineageCreationMethod(
    m_current_genwrap->intValue("LINEAGE_CREATION_METHOD", ok)
  );
  cSetupAssistantConfig::setGenerationIncMethod(
    m_current_genwrap->intValue("GENERATION_INC_METHOD", ok)
  );
  cSetupAssistantConfig::setAgeLimit(
    m_current_genwrap->intValue("AGE_LIMIT", ok)
  );
  cSetupAssistantConfig::setChildSizeRange(
  m_current_genwrap->dblValue("CHILD_SIZE_RANGE", ok)
  ); 
  cSetupAssistantConfig::setMinCopiedLines(
  m_current_genwrap->dblValue("MIN_COPIED_LINES", ok)
  );
  cSetupAssistantConfig::setMinExeLines(
  m_current_genwrap->dblValue("MIN_EXE_LINES", ok)
  );
  cSetupAssistantConfig::setRequireAllocate(
    m_current_genwrap->intValue("REQUIRE_ALLOCATE", ok)
  );

  cSetupAssistantConfig::setRevertFatal(
  m_current_genwrap->dblValue("REVERT_FATAL", ok)
  ); 
  cSetupAssistantConfig::setRevertNeg(
  m_current_genwrap->dblValue("REVERT_DETRIMENTAL", ok)
  );
  cSetupAssistantConfig::setRevertNeut(
  m_current_genwrap->dblValue("REVERT_NEUTRAL", ok)
  );
  cSetupAssistantConfig::setRevertPos(
  m_current_genwrap->dblValue("REVERT_BENEFICIAL", ok)
  );
  cSetupAssistantConfig::setSterilizeFatal(
  m_current_genwrap->dblValue("STERILIZE_FATAL", ok)
  );
  cSetupAssistantConfig::setSterilizeNeg(
  m_current_genwrap->dblValue("STERILIZE_DETRIMENTAL", ok)
  );
  cSetupAssistantConfig::setSterilizeNeut(
  m_current_genwrap->dblValue("STERILIZE_NEUTRAL", ok)
  );
  cSetupAssistantConfig::setSterilizePos(
  m_current_genwrap->dblValue("STERILIZE_BENEFICIAL", ok)
  );
  // watch-out.  call next function only after calling previous eight.
  cSetupAssistantConfig::setTestOnDivide();
  cSetupAssistantConfig::setFailImplicit(
    m_current_genwrap->intValue("FAIL_IMPLICIT", ok)
  );

  cSetupAssistantConfig::setSpeciesThreshold(
    m_current_genwrap->intValue("SPECIES_THRESHOLD", ok)
  );
  cSetupAssistantConfig::setThreshold(
    m_current_genwrap->intValue("THRESHOLD", ok)
  );
  cSetupAssistantConfig::setGenotypePrint(
    m_current_genwrap->intValue("GENOTYPE_PRINT", ok)
  );
  cSetupAssistantConfig::setSpeciesPrint(
    m_current_genwrap->intValue("SPECIES_PRINT", ok)
  );
  cSetupAssistantConfig::setSpeciesRecording(
    m_current_genwrap->intValue("SPECIES_RECORDING", ok)
  );
  cSetupAssistantConfig::setGenotypePrintDom(
    m_current_genwrap->intValue("GENOTYPE_PRINT_DOM", ok)
  );
  cSetupAssistantConfig::setTestCPUTimeMod(
    m_current_genwrap->intValue("TEST_CPU_TIME_MOD", ok)
  );
  cSetupAssistantConfig::setTrackMainLineage(
    m_current_genwrap->intValue("TRACK_MAIN_LINEAGE", ok)
  );

  cSetupAssistantConfig::setMaxCPUThreads(
    m_current_genwrap->intValue("MAX_CPU_THREADS", ok)
  );

  cSetupAssistantConfig::setSlicingMethod(
    m_current_genwrap->intValue("SLICING_METHOD", ok)
  );
  cSetupAssistantConfig::setSizeMeritMethod(
    m_current_genwrap->intValue("SIZE_MERIT_METHOD", ok)
  );
  cSetupAssistantConfig::setBaseSizeMerit(
    m_current_genwrap->intValue("BASE_SIZE_MERIT", ok)
  );
  cSetupAssistantConfig::setAveTimeSlice(
    m_current_genwrap->intValue("AVE_TIME_SLICE", ok)
  );
  cSetupAssistantConfig::setMeritTime(
    m_current_genwrap->intValue("MERIT_TIME", ok)
  );

  cSetupAssistantConfig::setTaskMeritMethod(
    m_current_genwrap->intValue("TASK_MERIT_METHOD", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setMaxNumTasksRewarded(
  //  m_current_genwrap->intValue("MAX_NUM_TASKS_REWARDED", ok)
  //);
  cSetupAssistantConfig::setMaxLabelExeSize(
    m_current_genwrap->intValue("MAX_LABEL_EXE_SIZE", ok)
  );

  cSetupAssistantConfig::setPointMutProb(
  m_current_genwrap->dblValue("POINT_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setCopyMutProb(
  m_current_genwrap->dblValue("COPY_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setInsMutProb(
  m_current_genwrap->dblValue("INS_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDelMutProb(
  m_current_genwrap->dblValue("DEL_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDivideMutProb(
  m_current_genwrap->dblValue("DIVIDE_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDivideInsProb(
  m_current_genwrap->dblValue("DIVIDE_INS_PROB", ok)
  );
  cSetupAssistantConfig::setDivideDelProb(
  m_current_genwrap->dblValue("DIVIDE_DEL_PROB", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setParentMutProb(
  //m_current_genwrap->dblValue("PARENT_MUT_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setCrossoverProb(
  //m_current_genwrap->dblValue("CROSSOVER_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setAlignedCrossoverProb(
  //m_current_genwrap->dblValue("ALIGNED_CROSS_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setExeErrProb(
  //m_current_genwrap->dblValue("EXE_ERROR_PROB", ok)
  //);

  cSetupAssistantConfig::setViewMode(
    m_current_genwrap->intValue("VIEW_MODE", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setViewerType(
  //  m_current_genwrap->intValue("VIEWER_TYPE", ok)
  //);

/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogBreedCount(
  //  m_current_genwrap->intValue("LOG_BREED_COUNT", ok)
  //);
  cSetupAssistantConfig::setLogCreatures(
    m_current_genwrap->intValue("LOG_CREATURES", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogPhylogeny(
  //  m_current_genwrap->intValue("LOG_PHYLOGENY", ok)
  //);

  cSetupAssistantConfig::setLogGenotypes(
    m_current_genwrap->intValue("LOG_GENOTYPES", ok)
  );
  // watch out.  only call after calling setLogGenotypes().
  cSetupAssistantConfig::setLogThresholdOnly();
  cSetupAssistantConfig::setLogThreshold(
    m_current_genwrap->intValue("LOG_THRESHOLD", ok)
  );
  cSetupAssistantConfig::setLogSpecies(
    m_current_genwrap->intValue("LOG_SPECIES", ok)
  );
  cSetupAssistantConfig::setLogLandscape(
    m_current_genwrap->intValue("LOG_LANDSCAPE", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogMutations(
  //  m_current_genwrap->intValue("LOG_MUTATIONS", ok)
  //);
  cSetupAssistantConfig::setLogLineages(
    m_current_genwrap->intValue("LOG_LINEAGES", ok)
  );

  cSetupAssistantConfig::setRandomSeed(
    m_current_genwrap->intValue("RANDOM_SEED", ok)
  );


  GenDebug(" --- done, happy.  current directory:  ")(QDir::currentDirPath());

  return result;
}


QString
TestWizard::setupStylePageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // nothing

  result = title(page);
  return result;
}
QString
TestWizard::setupStylePageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  return result;
}


QString
TestWizard::modifyGenesisSelectPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  error-checks
    strval = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
    QFileInfo genesis_fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(genesis_fileinfo.exists()){
          if(genesis_fileinfo.isReadable()){
            m_world_to_modify_lb->setText(genesis_fileinfo.absFilePath());
          } else GenDebug(" --- !genesis_fileinfo.isReadable()");
        } else GenDebug("! --- genesis_fileinfo.exists()");
      } else GenDebug(" --- strval == QString::null");
    } else GenDebug(" --- bad key:  GENESIS_PATH");

  result = title(page);
  return result;
}
QString
TestWizard::modifyGenesisSelectPageValidator(QWidget *page)
{
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  QString genesis_filename = m_world_to_modify_lb->text();
  QFileInfo genesis_fileinfo(genesis_filename);
  if(genesis_filename.isEmpty()){
    result += "no world file chosen\n";
  } else if(!genesis_fileinfo.exists()){
    result += "selected world file does not exist\n";
  } else if(!genesis_fileinfo.isReadable()){
    result += "selected world is not readable\n";
  }

  if (!result.isEmpty()) return result;

  m_current_genwrap->loadGenesis(genesis_fileinfo.absFilePath());
  m_current_genwrap->set(
    "GENESIS_PATH",
    genesis_filename
  );

  return result;
}


QString
TestWizard::simpleTopologyPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  okay, so I do need to have checks to see what kind of
    // setup is being performed.  simple topology can only take two of
    // the six possible values.

  result = title(page);
  return result;
}
QString
TestWizard::simpleTopologyPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;

  // nothing.  handled in slots
  //  simpleTwoDimensionalSlot(), simpleWellMixedFluidSlot()

  return result;
}


QString
TestWizard::simple2dSizePageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("WORLD-X", ok);
    if(ok) m_simple_worldwidth_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("WORLD-Y", ok);
    if(ok) m_simple_worldheight_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

  result = title(page);
  return result;
}


QString
TestWizard::simple2dSizePageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int width = m_simple_worldwidth_le->text().toInt(&ok);
  if(!ok) result += "world width not an integer\n";
  int height = m_simple_worldheight_le->text().toInt(&ok);
  if(!ok) result += "world height not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "WORLD-X",
    width
  );
  m_current_genwrap->set(
    "WORLD-Y",
    height
  );
  GenDebug(" --- width, height okay");

  return result;
}


QString
TestWizard::simpleWellmixedSizePageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  as in m_simple_topology_page.

  result = title(page);
  return result;
}
QString
TestWizard::simpleWellmixedSizePageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  int width, height;

  switch(m_simple_worldsize_cb->currentItem()){
  case 0:  width = height = 20; break;
  case 1:  width = height = 30; break;
  case 2:  width = height = 40; break;
  case 3:  width = height = 50; break;
  case 4:  width = height = 60; break;
  case 5:  width = height = 80; break;
  case 6:  width = height = 100; break;
  default:  result += "m_simple_worldsize_cb has unexpected setting\n";
  }

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "WORLD-X",
    width
  );
  m_current_genwrap->set(
    "WORLD-Y",
    height
  );
  GenDebug(" --- width, height okay");

  return result;
}


QString
TestWizard::simplePopulatePageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  error-checks
    strval = m_current_genwrap->qStringValue("START_CREATURE", ok);
    QFileInfo start_creature_fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(start_creature_fileinfo.exists()){
          if(start_creature_fileinfo.isReadable()){
            m_simple_startcreature_lb->setText(
              start_creature_fileinfo.absFilePath()
            );
          } else GenDebug(" --- !start_creature_fileinfo.isReadable()");
        } else GenDebug("! --- start_creature_fileinfo.exists()");
      } else GenDebug(" --- strval == QString::null");
    } else GenDebug(" --- bad key:  START_CREATURE");

    // FIXME:  population method should come from events file...

  result = title(page);
  return result;
}
QString
TestWizard::simplePopulatePageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;

  QString filename = m_simple_startcreature_lb->text();
  QFileInfo fileinfo(filename);
  if(filename.isEmpty()){
    result += "no organsim file chosen\n";
  } else if(!fileinfo.exists()){
    result += "organsim file does not exist\n";
  } else if(!fileinfo.isReadable()){
    result += "organsim file is not readable\n";
  }

  if (!result.isEmpty()) return result;

  m_current_genwrap->set(
    "START_CREATURE",
    filename
  );

  GenDebug(" --- FIXME:  injectmethod stuff not yet implemented.");

  return result;
}


QString
TestWizard::simpleEndconditionPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("MAX_GENERATIONS", ok);
    if(ok) m_simple_genlimit_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("MAX_UPDATES", ok);
    if(ok) m_simple_updatelimit_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("END_CONDITION_MODE", ok);
    if(ok){
      if(GenesisWrapper::MaxUpdatesORMaxGenerations == intval){
        m_simple_endcond_either_rb->setChecked(true);
      } else {
        m_simple_endcond_both_rb->setChecked(true);
      }
    } else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::simpleEndconditionPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int max_gen = m_simple_genlimit_le->text().toInt(&ok);
  if(!ok) result += "generation limit is not an integer\n";
  int max_update = m_simple_updatelimit_le->text().toInt(&ok);
  if(!ok) result += "update limit is not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "MAX_GENERATIONS",
    max_gen
  );
  m_current_genwrap->set(
    "MAX_UPDATES",
    max_update
  );
  m_current_genwrap->set(
    "END_CONDITION_MODE",
    (m_simple_endcond_either_rb->isChecked())
    ?
    (GenesisWrapper::MaxUpdatesORMaxGenerations)
    :
    (GenesisWrapper::MaxUpdatesANDMaxGenerations)
  );

  GenDebug(" --- max_gen, max_update okay");

  return result;
}


QString
TestWizard::simpleEnvironmentPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  error-checks
    strval = m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok);
    QFileInfo fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(fileinfo.exists()){
          if(fileinfo.isReadable()){
            m_simple_envfile_lb->setText(
              fileinfo.absFilePath()
            );
          } else GenDebug(" --- !fileinfo.isReadable()");
        } else GenDebug("! --- fileinfo.exists()");
      } else GenDebug(" --- strval == QString::null");
    } else GenDebug(" --- bad key:  ENVIRONMENT_FILE");

  result = title(page);
  return result;
}
QString
TestWizard::simpleEnvironmentPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;

  QString filename = m_simple_envfile_lb->text();
  QFileInfo fileinfo(filename);
  if(filename.isEmpty()){
    result += "no environment file chosen\n";
  } else if(!fileinfo.exists()){
    result += "environment file does not exist\n";
  } else if(!fileinfo.isReadable()){
    result += "environment file is not readable\n";
  }

  if (!result.isEmpty()) return result;

  m_current_genwrap->set(
    "ENVIRONMENT_FILE",
    filename
  );

  return result;
}


QString
TestWizard::detailedEndconditionPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("MAX_GENERATIONS", ok);
    if(ok) m_detailed_genlimit_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("MAX_UPDATES", ok);
    if(ok) m_detailed_updatelimit_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("END_CONDITION_MODE", ok);
    if(ok){
      if(GenesisWrapper::MaxUpdatesORMaxGenerations == intval){
        m_detailed_endcond_either_rb->setChecked(true);
      } else {
        m_detailed_endcond_both_rb->setChecked(true);
      }
    } else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedEndconditionPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int max_gen = m_detailed_genlimit_le->text().toInt(&ok);
  if(!ok) result += "generation limit is not an integer\n";
  int max_update = m_detailed_updatelimit_le->text().toInt(&ok);
  if(!ok) result += "update limit is not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "MAX_GENERATIONS",
    max_gen
  );
  m_current_genwrap->set(
    "MAX_UPDATES",
    max_update
  );
  m_current_genwrap->set(
    "END_CONDITION_MODE",
    (m_detailed_endcond_either_rb->isChecked())
    ?
    (GenesisWrapper::MaxUpdatesORMaxGenerations)
    :
    (GenesisWrapper::MaxUpdatesANDMaxGenerations)
  );

  GenDebug(" --- max_gen, max_update okay");

  return result;
}


QString
TestWizard::detailedWorldsizePageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("WORLD-X", ok);
    if(ok) m_detailed_width_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("WORLD-Y", ok);
    if(ok) m_detailed_height_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedWorldsizePageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int width = m_detailed_width_le->text().toInt(&ok);
  if(!ok) result += "world width not an integer\n";
  int height = m_detailed_height_le->text().toInt(&ok);
  if(!ok) result += "world height not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "WORLD-X",
    width
  );
  m_current_genwrap->set(
    "WORLD-Y",
    height
  );
  GenDebug(" --- width, height okay");

  return result;
}


QString
TestWizard::detailedCPUThreadsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("MAX_CPU_THREADS", ok);
    if(ok) m_detailed_cputhreads_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedCPUThreadsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int max_cpu_threads = m_detailed_cputhreads_le->text().toInt(&ok);
  if(!ok) result += "max_cpu_threads not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "MAX_CPU_THREADS",
    max_cpu_threads
  );
  GenDebug(" --- max_cpu_threads okay");

  return result;
}


QString
TestWizard::detailedRandomseedPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("RANDOM_SEED", ok);
    if(ok) m_detailed_randomseed_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedRandomseedPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int random_seed = m_detailed_randomseed_le->text().toInt(&ok);
  if(!ok) result += "random_seed not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "RANDOM_SEED",
    random_seed
  );
  GenDebug(" --- random_seed okay");

  return result;
}


QString
TestWizard::detailedFilepathsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  error-checks
  {
    strval = m_current_genwrap->qStringValue("INST_SET", ok);
    QFileInfo fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(fileinfo.exists()){
          if(fileinfo.isReadable()){
            m_detailed_instfile_lb->setText(
              fileinfo.absFilePath()
            );
          } else GenDebug(" --- !inst fileinfo.isReadable()");
        } else GenDebug("! --- inst fileinfo.exists()");
      } else GenDebug(" --- inst strval == QString::null");
    } else GenDebug(" --- bad key:  INST_SET");
  }
  {
    strval = m_current_genwrap->qStringValue("EVENT_FILE", ok);
    QFileInfo fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(fileinfo.exists()){
          if(fileinfo.isReadable()){
            m_detailed_eventfile_lb->setText(
              fileinfo.absFilePath()
            );
          } else GenDebug(" --- !event fileinfo.isReadable()");
        } else GenDebug("! --- event fileinfo.exists()");
      } else GenDebug(" --- event strval == QString::null");
    } else GenDebug(" --- bad key:  EVENT_FILE");
  }
  {
    strval = m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok);
    QFileInfo fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(fileinfo.exists()){
          if(fileinfo.isReadable()){
            m_detailed_envfile_lb->setText(
              fileinfo.absFilePath()
            );
          } else GenDebug(" --- !env fileinfo.isReadable()");
        } else GenDebug("! --- env fileinfo.exists()");
      } else GenDebug(" --- env strval == QString::null");
    } else GenDebug(" --- bad key:  ENVIRONMENT_FILE");
  }
  {
    strval = m_current_genwrap->qStringValue("START_CREATURE", ok);
    QFileInfo fileinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(fileinfo.exists()){
          if(fileinfo.isReadable()){
            m_detailed_startcreaturefile_lb->setText(
              fileinfo.absFilePath()
            );
          } else GenDebug(" --- !organism fileinfo.isReadable()");
        } else GenDebug("! --- organism fileinfo.exists()");
      } else GenDebug(" --- organism strval == QString::null");
    } else GenDebug(" --- bad key:  START_CREATURE");
  }

  result = title(page);
  return result;
}
QString
TestWizard::detailedFilepathsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;

  QString inst_filename = m_detailed_instfile_lb->text();
  QFileInfo inst_fileinfo(inst_filename);
  if(inst_filename.isEmpty()){
    result += "no instructions file chosen\n";
  } else if(!inst_fileinfo.exists()){
    result += "instructions file does not exist\n";
  } else if(!inst_fileinfo.isReadable()){
    result += "instructions file is not readable\n";
  }

  QString event_filename = m_detailed_eventfile_lb->text();
  QFileInfo event_fileinfo(event_filename);
  if(event_filename.isEmpty()){
    result += "no event file chosen\n";
  } else if(!event_fileinfo.exists()){
    result += "event file does not exist\n";
  } else if(!event_fileinfo.isReadable()){
    result += "event file is not readable\n";
  }

  QString env_filename = m_detailed_envfile_lb->text();
  QFileInfo env_fileinfo(env_filename);
  if(env_filename.isEmpty()){
    result += "no environment file chosen\n";
  } else if(!env_fileinfo.exists()){
    result += "environment file does not exist\n";
  } else if(!env_fileinfo.isReadable()){
    result += "environment file is not readable\n";
  }

  QString start_filename = m_detailed_startcreaturefile_lb->text();
  QFileInfo start_fileinfo(start_filename);
  if(start_filename.isEmpty()){
    result += "no organism file chosen\n";
  } else if(!start_fileinfo.exists()){
    result += "organism file does not exist\n";
  } else if(!start_fileinfo.isReadable()){
    result += "organism file is not readable\n";
  }

  if (!result.isEmpty()) return result;

  m_current_genwrap->set(
    "INST_SET",
    inst_filename
  );
  m_current_genwrap->set(
    "EVENT_FILE",
    event_filename
  );
  m_current_genwrap->set(
    "ENVIRONMENT_FILE",
    env_filename
  );
  m_current_genwrap->set(
    "START_CREATURE",
    start_filename
  );

  return result;
}


QString
TestWizard::detailedBirthsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("BIRTH_METHOD", ok);
    if(ok) m_detailed_birthmethod_cb->setCurrentItem(
        m_birth_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("ALLOC_METHOD", ok);
    if(ok) m_detailed_allocmethod_cb->setCurrentItem(
        m_alloc_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("DIVIDE_METHOD", ok);
    if(ok) m_detailed_dividemethod_cb->setCurrentItem(
        m_divide_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("GENERATION_INC_METHOD", ok);
    if(ok) m_detailed_genincmethod_cb->setCurrentItem(
        m_generation_inc_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedBirthsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;

  m_current_genwrap->set(
    "BIRTH_METHOD",
    m_birth_method_map.filechoice(
      m_detailed_birthmethod_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "ALLOC_METHOD",
    m_alloc_method_map.filechoice(
      m_detailed_allocmethod_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "DIVIDE_METHOD",
    m_divide_method_map.filechoice(
      m_detailed_dividemethod_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "GENERATION_INC_METHOD",
    m_generation_inc_method_map.filechoice(
      m_detailed_genincmethod_cb->currentItem()
    )
  );

  return result;
}


QString
TestWizard::detailedDeathsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("DEATH_METHOD", ok);
    if(ok) m_detailed_deathmethod_cb->setCurrentItem(
        m_death_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("AGE_LIMIT", ok);
    if(ok) m_detailed_agelimit_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("AGE_DEVIATION", ok);
    if(ok) m_detailed_agedeviation_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedDeathsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int age_limit = m_detailed_agelimit_le->text().toInt(&ok);
  if(!ok) result += "age_limit not an integer\n";

  double age_deviation = m_detailed_agedeviation_le->text().toDouble(&ok);
  if(!ok) result += "age_deviation not a floating point number\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "AGE_LIMIT",
    age_limit
  );
  m_current_genwrap->set(
    "AGE_DEVIATION",
    age_deviation
  );
  m_current_genwrap->set(
    "DEATH_METHOD",
    m_death_method_map.filechoice(
      m_detailed_deathmethod_cb->currentItem()
    )
  );
  GenDebug(" --- age_limit, age_deviation okay");

  return result;
}


QString
TestWizard::detailedDivrestrictionsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    dblval = m_current_genwrap->dblValue("CHILD_SIZE_RANGE", ok);
    if(ok) m_detailed_childsizerange_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("MIN_COPIED_LINES", ok);
    if(ok) m_detailed_mincopysize_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("MIN_EXE_LINES", ok);
    if(ok) m_detailed_minexelines_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("REQUIRE_ALLOCATE", ok);
    if(ok) m_detailed_allocrequired_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    // FIXME:  wtf is this thing for?
    //intval = m_current_genwrap->intValue("REQUIRED_TASK", ok);
    //m_detailed_taskrequired_cb

  result = title(page);
  return result;
}
QString
TestWizard::detailedDivrestrictionsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  double child_size_range = m_detailed_childsizerange_le->text().toDouble(&ok);
  if(!ok) result += "child_size_range not a floating point number\n";

  double min_copied_lines = m_detailed_mincopysize_le->text().toDouble(&ok);
  if(!ok) result += "min_copied_lines not a floating point number\n";

  double min_exe_lines = m_detailed_minexelines_le->text().toDouble(&ok);
  if(!ok) result += "min_exe_lines not a floating point number\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "CHILD_SIZE_RANGE",
    child_size_range
  );
  m_current_genwrap->set(
    "MIN_COPIED_LINES",
    min_copied_lines
  );
  m_current_genwrap->set(
    "MIN_EXE_LINES",
    min_exe_lines
  );
  m_current_genwrap->set(
    "REQUIRE_ALLOCATE",
    m_on_off_map.filechoice(
      m_detailed_allocrequired_cb->currentItem()
    )
  );
  // FIXME:  REQUIRED_TASK
  GenDebug(" --- child_size_range, min_copied_lines, min_exe_lines okay");

  return result;
}


QString
TestWizard::detailedExemutationsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    dblval = m_current_genwrap->dblValue("DEL_MUT_PROB", ok);
    if(ok) m_detailed_delmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("POINT_MUT_PROB", ok);
    if(ok) m_detailed_ptmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("COPY_MUT_PROB", ok);
    if(ok) m_detailed_cpmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

/* FIXME -- removed from base class. -- k */
    //dblval = m_current_genwrap->dblValue("EXE_ERROR_PROB", ok);
    //if(ok) m_detailed_exeerror_le->setText(QString("%1").arg(dblval));
    //else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("INS_MUT_PROB", ok);
    if(ok) m_detailed_insmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedExemutationsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  double del_mut_prob = m_detailed_delmut_le->text().toDouble(&ok);
  if(!ok) result += "del_mut_prob not a floating point number\n";

  double point_mut_prob = m_detailed_ptmut_le->text().toDouble(&ok);
  if(!ok) result += "point_mut_prob not a floating point number\n";

  double copy_mut_prob = m_detailed_cpmut_le->text().toDouble(&ok);
  if(!ok) result += "copy_mut_prob not a floating point number\n";

/* FIXME -- removed from base class. -- k */
  //double exe_error_prob = m_detailed_exeerror_le->text().toDouble(&ok);
  //if(!ok) result += "exe_error_prob not a floating point number\n";

  double ins_mut_prob = m_detailed_insmut_le->text().toDouble(&ok);
  if(!ok) result += "ins_mut_prob not a floating point number\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "DEL_MUT_PROB",
    del_mut_prob
  );
  m_current_genwrap->set(
    "POINT_MUT_PROB",
    point_mut_prob
  );
  m_current_genwrap->set(
    "COPY_MUT_PROB",
    copy_mut_prob
  );
/* FIXME -- removed from base class. -- k */
  //m_current_genwrap->set(
  //  "EXE_ERROR_PROB",
  //  exe_error_prob
  //);
  m_current_genwrap->set(
    "INS_MUT_PROB",
    ins_mut_prob
  );
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedDivmutationsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

/* FIXME -- removed from base class. -- k */
    //dblval = m_current_genwrap->dblValue("ALIGNED_CROSS_PROB", ok);
    //if(ok) m_detailed_alignedcrossover_le->setText(QString("%1").arg(dblval));
    //else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("PARENT_MUT_PROB", ok);
    if(ok) m_detailed_divparentmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("DIVIDE_MUT_PROB", ok);
    if(ok) m_detailed_divptmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("DIVIDE_DEL_PROB", ok);
    if(ok) m_detailed_divdelmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("DIVIDE_INS_PROB", ok);
    if(ok) m_detailed_divinsmut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

/* FIXME -- removed from base class. -- k */
    //dblval = m_current_genwrap->dblValue("CROSSOVER_PROB", ok);
    //if(ok) m_detailed_crossover_le->setText(QString("%1").arg(dblval));
    //else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedDivmutationsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

/* FIXME -- removed from base class. -- k */
  //double aligned_cross_prob = m_detailed_alignedcrossover_le->text().toDouble(&ok);
  //if(!ok) result += "aligned_cross_prob not a floating point number\n";

  double parent_mut_prob = m_detailed_divparentmut_le->text().toDouble(&ok);
  if(!ok) result += "parent_mut_prob not a floating point number\n";

  double divide_mut_prob = m_detailed_divptmut_le->text().toDouble(&ok);
  if(!ok) result += "divide_mut_prob not a floating point number\n";

  double divide_del_prob = m_detailed_divdelmut_le->text().toDouble(&ok);
  if(!ok) result += "divide_del_prob not a floating point number\n";

  double divide_ins_prob = m_detailed_divinsmut_le->text().toDouble(&ok);
  if(!ok) result += "divide_ins_prob not a floating point number\n";

/* FIXME -- removed from base class. -- k */
  //double crossover_prob = m_detailed_crossover_le->text().toDouble(&ok);
  //if(!ok) result += "crossover_prob not a floating point number\n";

  if(QString::null != result) return result;

/* FIXME -- removed from base class. -- k */
  //m_current_genwrap->set(
  //  "ALIGNED_CROSS_PROB",
  //  aligned_cross_prob
  //);
  m_current_genwrap->set(
    "PARENT_MUT_PROB",
    parent_mut_prob
  );
  m_current_genwrap->set(
    "DIVIDE_MUT_PROB",
    divide_mut_prob
  );
  m_current_genwrap->set(
    "DIVIDE_DEL_PROB",
    divide_del_prob
  );
  m_current_genwrap->set(
    "DIVIDE_INS_PROB",
    divide_ins_prob
  );
/* FIXME -- removed from base class. -- k */
  //m_current_genwrap->set(
  //  "CROSSOVER_PROB",
  //  crossover_prob
  //);
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedReversionsPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    dblval = m_current_genwrap->dblValue("REVERT_FATAL", ok);
    if(ok) m_detailed_revfatal_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("REVERT_DETRIMENTAL", ok);
    if(ok) m_detailed_revdet_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("REVERT_NEUTRAL", ok);
    if(ok) m_detailed_revneut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("REVERT_BENEFICIAL", ok);
    if(ok) m_detailed_revbenef_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("STERILIZE_FATAL", ok);
    if(ok) m_detailed_sterilfatal_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("STERILIZE_DETRIMENTAL", ok);
    if(ok) m_detailed_sterildet_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("STERILIZE_NEUTRAL", ok);
    if(ok) m_detailed_sterilneut_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    dblval = m_current_genwrap->dblValue("STERILIZE_BENEFICIAL", ok);
    if(ok) m_detailed_sterilbenef_le->setText(QString("%1").arg(dblval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("FAIL_IMPLICIT", ok);
    if(ok) m_detailed_failimpl_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedReversionsPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  double REVERT_FATAL = m_detailed_revfatal_le->text().toDouble(&ok);
  if(!ok) result += "REVERT_FATAL not a floating point number\n";

  double REVERT_DETRIMENTAL = m_detailed_revdet_le->text().toDouble(&ok);
  if(!ok) result += "REVERT_DETRIMENTAL not a floating point number\n";

  double REVERT_NEUTRAL = m_detailed_revneut_le->text().toDouble(&ok);
  if(!ok) result += "REVERT_NEUTRAL not a floating point number\n";

  double REVERT_BENEFICIAL = m_detailed_revbenef_le->text().toDouble(&ok);
  if(!ok) result += "REVERT_BENEFICIAL not a floating point number\n";

  double STERILIZE_FATAL = m_detailed_sterilfatal_le->text().toDouble(&ok);
  if(!ok) result += "STERILIZE_FATAL not a floating point number\n";

  double STERILIZE_DETRIMENTAL = m_detailed_sterildet_le->text().toDouble(&ok);
  if(!ok) result += "STERILIZE_DETRIMENTAL not a floating point number\n";

  double STERILIZE_NEUTRAL = m_detailed_sterilneut_le->text().toDouble(&ok);
  if(!ok) result += "STERILIZE_NEUTRAL not a floating point number\n";

  double STERILIZE_BENEFICIAL = m_detailed_sterilbenef_le->text().toDouble(&ok);
  if(!ok) result += "STERILIZE_BENEFICIAL not a floating point number\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "REVERT_FATAL",
    REVERT_FATAL
  );
  m_current_genwrap->set(
    "REVERT_DETRIMENTAL",
    REVERT_DETRIMENTAL
  );
  m_current_genwrap->set(
    "REVERT_NEUTRAL",
    REVERT_NEUTRAL
  );
  m_current_genwrap->set(
    "REVERT_BENEFICIAL",
    REVERT_BENEFICIAL
  );
  m_current_genwrap->set(
    "STERILIZE_FATAL",
    STERILIZE_FATAL
  );
  m_current_genwrap->set(
    "STERILIZE_DETRIMENTAL",
    STERILIZE_DETRIMENTAL
  );
  m_current_genwrap->set(
    "STERILIZE_NEUTRAL",
    STERILIZE_NEUTRAL
  );
  m_current_genwrap->set(
    "STERILIZE_BENEFICIAL",
    STERILIZE_BENEFICIAL
  );
  m_current_genwrap->set(
    "FAIL_IMPLICIT",
    m_on_off_map.filechoice(
      m_detailed_failimpl_cb->currentItem()
    )
  );
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedTimeslicingPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("AVE_TIME_SLICE", ok);
    if(ok) m_detailed_ave_timeslice_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("SLICING_METHOD", ok);
    if(ok) m_detailed_slicemethod_cb->setCurrentItem(
        m_slicing_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedTimeslicingPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int AVE_TIME_SLICE = m_detailed_ave_timeslice_le->text().toInt(&ok);
  if(!ok) result += "AVE_TIME_SLICE not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "AVE_TIME_SLICE",
    AVE_TIME_SLICE
  );
  m_current_genwrap->set(
    "SLICING_METHOD",
    m_slicing_method_map.filechoice(
      m_detailed_slicemethod_cb->currentItem()
    )
  );
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedMeritsizePageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("SIZE_MERIT_METHOD", ok);
    if(ok) m_detailed_sizemerit_method_cb->setCurrentItem(
        m_size_merit_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("MAX_LABEL_EXE_SIZE", ok);
    if(ok) m_detailed_maxlabelexe_size_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("BASE_SIZE_MERIT", ok);
    if(ok) m_detailed_basesizemerit_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedMeritsizePageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int MAX_LABEL_EXE_SIZE = m_detailed_maxlabelexe_size_le->text().toInt(&ok);
  if(!ok) result += "MAX_LABEL_EXE_SIZE not an integer\n";

  int BASE_SIZE_MERIT = m_detailed_basesizemerit_le->text().toInt(&ok);
  if(!ok) result += "BASE_SIZE_MERIT not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "MAX_LABEL_EXE_SIZE",
    MAX_LABEL_EXE_SIZE
  );
  m_current_genwrap->set(
    "BASE_SIZE_MERIT",
    BASE_SIZE_MERIT
  );
  m_current_genwrap->set(
    "SIZE_MERIT_METHOD",
    m_size_merit_method_map.filechoice(
      m_detailed_sizemerit_method_cb->currentItem()
    )
  );
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedMerittasksPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("MERIT_TIME", ok);
    if(ok) m_detailed_meritcalc_time_cb->setCurrentItem(
        m_merit_time_map.cbindex(intval)
      );
    else GenDebug(" --- ");

/* FIXME -- removed from base class. -- k */
    //intval = m_current_genwrap->intValue("MAX_NUM_TASKS_REWARDED", ok);
    //if(ok) m_detailed_maxtasks_rewarded_le->setText(QString("%1").arg(intval));
    //else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("TASK_MERIT_METHOD", ok);
    if(ok) m_detailed_taskmerit_method_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedMerittasksPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

/* FIXME -- removed from base class. -- k */
  //int MAX_NUM_TASKS_REWARDED = m_detailed_maxtasks_rewarded_le->text().toInt(&ok);
  //if(!ok) result += "MAX_NUM_TASKS_REWARDED not an integer\n";

  if(QString::null != result) return result;

/* FIXME -- removed from base class. -- k */
  //m_current_genwrap->set(
  //  "MAX_NUM_TASKS_REWARDED",
  //  MAX_NUM_TASKS_REWARDED
  //);
  m_current_genwrap->set(
    "MERIT_TIME",
    m_merit_time_map.filechoice(
      m_detailed_meritcalc_time_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "TASK_MERIT_METHOD",
    m_on_off_map.filechoice(
      m_detailed_taskmerit_method_cb->currentItem()
    )
  );
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedGenotypeinfoPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("GENOTYPE_PRINT_DOM", ok);
    if(ok) m_detailed_domgenotype_printing_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("THRESHOLD", ok);
    if(ok) m_detailed_genotype_threshold_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("GENOTYPE_PRINT", ok);
    if(ok) m_detailed_genotype_printing_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("TRACK_MAIN_LINEAGE", ok);
    if(ok) m_detailed_mainlineage_tracking_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedGenotypeinfoPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int GENOTYPE_PRINT_DOM = m_detailed_domgenotype_printing_le->text().toInt(&ok);
  if(!ok) result += "GENOTYPE_PRINT_DOM not an integer\n";

  int THRESHOLD = m_detailed_genotype_threshold_le->text().toInt(&ok);
  if(!ok) result += "THRESHOLD not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "GENOTYPE_PRINT_DOM",
    GENOTYPE_PRINT_DOM
  );
  m_current_genwrap->set(
    "THRESHOLD",
    THRESHOLD
  );
  m_current_genwrap->set(
    "GENOTYPE_PRINT",
    m_on_off_map.filechoice(
      m_detailed_genotype_printing_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "TRACK_MAIN_LINEAGE",
    m_on_off_map.filechoice(
      m_detailed_mainlineage_tracking_cb->currentItem()
    )
  );
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedSpeciesinfoPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("SPECIES_THRESHOLD", ok);
    if(ok) m_detailed_species_threshold_le->setText(QString("%1").arg(intval));
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("SPECIES_RECORDING", ok);
    if(ok) m_detailed_species_recording_cb->setCurrentItem(
        m_species_recording_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("SPECIES_PRINT", ok);
    if(ok) m_detailed_species_printing_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedSpeciesinfoPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  int SPECIES_THRESHOLD = m_detailed_species_threshold_le->text().toInt(&ok);
  if(!ok) result += "SPECIES_THRESHOLD not an integer\n";

  if(QString::null != result) return result;

  m_current_genwrap->set(
    "SPECIES_THRESHOLD",
    SPECIES_THRESHOLD
  );
  m_current_genwrap->set(
    "SPECIES_RECORDING",
    m_species_recording_map.filechoice(
      m_detailed_species_recording_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "SPECIES_PRINT",
    m_on_off_map.filechoice(
      m_detailed_species_printing_cb->currentItem()
    )
  );
  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::detailedLogfilesPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    intval = m_current_genwrap->intValue("LOG_GENOTYPES", ok);
    if(ok) m_detailed_log_genealogy_cb->setCurrentItem(
        m_log_genealogy_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("LOG_MUTATIONS", ok);
    if(ok) m_detailed_log_mutations_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

/* FIXME -- removed from base class. -- k */
    //intval = m_current_genwrap->intValue("LOG_BREED_COUNT", ok);
    //if(ok) m_detailed_log_breedcount_cb->setCurrentItem(
    //    m_on_off_map.cbindex(intval)
    //  );
    //else GenDebug(" --- ");

/* FIXME -- removed from base class. -- k */
    //intval = m_current_genwrap->intValue("LOG_PHYLOGENY", ok);
    //if(ok) m_detailed_log_phylogeny_cb->setCurrentItem(
    //    m_on_off_map.cbindex(intval)
    //  );
    //else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("LOG_CREATURES", ok);
    if(ok) m_detailed_log_creatures_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("LOG_THRESHOLD", ok);
    if(ok) m_detailed_log_threshold_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("LOG_SPECIES", ok);
    if(ok) m_detailed_log_species_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("LOG_GENOTYPES", ok);
    if(ok) m_detailed_log_genotypes_cb->setCurrentItem(
        m_log_genotypes_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("LINEAGE_CREATION_METHOD", ok);
    if(ok) m_detailed_lineagecreation_method_cb->setCurrentItem(
        m_lineage_creation_method_map.cbindex(intval)
      );
    else GenDebug(" --- ");

    intval = m_current_genwrap->intValue("LOG_LANDSCAPE", ok);
    if(ok) m_detailed_log_landscape_cb->setCurrentItem(
        m_on_off_map.cbindex(intval)
      );
    else GenDebug(" --- ");

  result = title(page);
  return result;
}
QString
TestWizard::detailedLogfilesPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;

  m_current_genwrap->set(
    "LOG_GENOTYPES",
    m_log_genealogy_map.filechoice(
      m_detailed_log_genealogy_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "LOG_MUTATIONS",
    m_on_off_map.filechoice(
      m_detailed_log_mutations_cb->currentItem()
    )
  );
/* FIXME -- removed from base class. -- k */
  //m_current_genwrap->set(
  //  "LOG_BREED_COUNT",
  //  m_on_off_map.filechoice(
  //    m_detailed_log_breedcount_cb->currentItem()
  //  )
  //);
/* FIXME -- removed from base class. -- k */
  //m_current_genwrap->set(
  //  "LOG_PHYLOGENY",
  //  m_on_off_map.filechoice(
  //    m_detailed_log_phylogeny_cb->currentItem()
  //  )
  //);
  m_current_genwrap->set(
    "LOG_CREATURES",
    m_on_off_map.filechoice(
      m_detailed_log_creatures_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "LOG_THRESHOLD",
    m_on_off_map.filechoice(
      m_detailed_log_threshold_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "LOG_SPECIES",
    m_on_off_map.filechoice(
      m_detailed_log_species_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "LOG_GENOTYPES",
    m_log_genotypes_map.filechoice(
      m_detailed_log_genotypes_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "LINEAGE_CREATION_METHOD",
    m_lineage_creation_method_map.filechoice(
      m_detailed_lineagecreation_method_cb->currentItem()
    )
  );
  m_current_genwrap->set(
    "LOG_LANDSCAPE",
    m_on_off_map.filechoice(
      m_detailed_log_landscape_cb->currentItem()
    )
  );

  GenDebug(" --- okay");

  return result;
}


QString
TestWizard::newSavegenesisPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  error-checks
    //strval = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
    //if(ok) m_new_world_lb->setText(strval);
    //else GenDebug(" --- ");

    // FIXME:  error-checks
    //strval = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
    //QFileInfo genesis_fileinfo(strval);
    //if(ok){
    //  if(strval != QString::null){
    //    if(genesis_fileinfo.exists()){
    //      if(genesis_fileinfo.isReadable()){
    //        m_new_world_lb->setText(genesis_fileinfo.absFilePath());
    //      } else GenDebug(" --- !genesis_fileinfo.isReadable()");
    //    } else GenDebug("! --- genesis_fileinfo.exists()");
    //  } else GenDebug(" --- strval == QString::null");
    //} else GenDebug(" --- bad key:  GENESIS_PATH");

    strval = m_current_genwrap->qStringValue("DEFAULT_DIR", ok);
    QFileInfo working_dirinfo(strval);
    m_new_workdir_lb->setText("");
    if(ok){
      if(!strval.isEmpty()){
        if(working_dirinfo.exists()){
          if(working_dirinfo.isDir()){
            if(working_dirinfo.isReadable()){
              if(working_dirinfo.isWritable()){
                m_new_workdir_lb->setText(working_dirinfo.absFilePath());
              } else GenDebug(" --- !working_dirinfo.isWritable()");
            } else GenDebug(" --- !working_dirinfo.isReadable()");
          } else GenDebug(" --- !working_dirinfo.isDir()");
        } else GenDebug(" --- !working_dirinfo.exists()");
      } else GenDebug(" --- strval == QString::null");
    } else GenDebug(" --- bad key:  DEFAULT_DIR");

  result = title(page);
  return result;
}
QString
TestWizard::newSavegenesisPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  QString genesis_filename = m_new_world_lb->text();
  QFileInfo genesis_fileinfo(genesis_filename);
  if(genesis_filename.isEmpty()){
    result += "no world file chosen\n";
  //} else if(!genesis_fileinfo.exists()){
  //  result += "selected world file does not exist\n";
  //} else if(!genesis_fileinfo.isWritable()){
  //  result += "selected world is not writable\n";
  }

  QString working_dirname = m_new_workdir_lb->text();
  QFileInfo working_dirinfo(working_dirname);
  if(working_dirname.isEmpty()){
    result += "no work directory chosen\n";
  } else if(!working_dirinfo.exists()){
    result += "work directory does not exist\n";
  } else if(!working_dirinfo.isDir()){
    result += "file chosen instead of directory for work directory\n";
  } else if(!working_dirinfo.isReadable()){
    result += "work directory is not readable\n";
  } else if(!working_dirinfo.isWritable()){
    result += "work directory is not writable\n";
  }

  if (!result.isEmpty()) return result;

  //m_current_genwrap->loadGenesis(genesis_fileinfo.absFilePath());
  m_current_genwrap->set(
    "GENESIS_PATH",
    genesis_filename
  );

  QString inst_set_filename = m_current_genwrap->qStringValue("INST_SET", ok);
  QFileInfo inst_set_fileinfo(inst_set_filename);
  if(inst_set_fileinfo.isRelative()){
    GenDebug(" --- inst_set_fileinfo.isRelative()");
    inst_set_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      inst_set_filename
    );
  }
  if(!ok){
    result += "programmer error:  INST_SET is an invalid key\n";
  } else if(inst_set_filename.isEmpty()){
    result
      += "instruction set file specified in world file is blank\n";
  } else if(!inst_set_fileinfo.exists()){
    result
      += "instruction set file specified in world file doesn't exist\n";
  } else if(!inst_set_fileinfo.isReadable()){
    result
      += "instruction set file specified in world file can't be read\n";
  }

  QString start_creature_filename
    = m_current_genwrap->qStringValue("START_CREATURE", ok);
  QFileInfo start_creature_fileinfo(start_creature_filename);
  if(start_creature_fileinfo.isRelative()){
    GenDebug(" --- start_creature_fileinfo.isRelative()");
    start_creature_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      start_creature_filename
    );
  }
  if(!ok){
    result += "programmer error:  START_CREATURE is an invalid key\n";
  } else if(start_creature_filename.isEmpty()){
    result
      += "start creature file specified in world file is blank\n";
  } else if(!start_creature_fileinfo.exists()){
    result
      += "start creature file specified in world file doesn't exist\n";
  } else if(!start_creature_fileinfo.isReadable()){
    result
      += "start creature file specified in world file can't be read\n";
  }

  QString event_file_filename
    = m_current_genwrap->qStringValue("EVENT_FILE", ok);
  QFileInfo event_file_fileinfo(event_file_filename);
  if(event_file_fileinfo.isRelative()){
    GenDebug(" --- event_file_fileinfo.isRelative()");
    event_file_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      event_file_filename
    );
  }
  if(!ok){
    result += "programmer error:  EVENT_FILE is an invalid key\n";
  } else if(event_file_filename.isEmpty()){
    result
      += "event file specified in world file is blank\n";
  } else if(!event_file_fileinfo.exists()){
    result
      += "event file specified in world file doesn't exist\n";
  } else if(!event_file_fileinfo.isReadable()){
    result
      += "event file specified in world file can't be read\n";
  }

  QString environment_filename
    = m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok);
  QFileInfo environment_fileinfo(environment_filename);
  if(environment_fileinfo.isRelative()){
    GenDebug(" --- environment_fileinfo.isRelative()");
    environment_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      environment_filename
    );
  }
  if(!ok){
    result += "programmer error:  ENVIRONMENT_FILE is an invalid key\n";
  } else if(environment_filename.isEmpty()){
    result
      += "environment file specified in world file is blank\n";
  } else if(!environment_fileinfo.exists()){
    result
      += "environment file specified in world file doesn't exist\n";
  } else if(!environment_fileinfo.isReadable()){
    result
      += "environment file specified in world file can't be read\n";
  }

  QDir::setCurrent(working_dirname);

  if (!result.isEmpty()) return result;

  m_current_genwrap->set(
    "GENESIS_PATH",
    genesis_filename
  );
  m_current_genwrap->set(
    "DEFAULT_DIR",
    working_dirname
  );
  m_current_genwrap->saveGenesis(genesis_filename);

  cSetupAssistantConfig::setDefaultDir(
    m_current_genwrap->qStringValue("DEFAULT_DIR", ok).latin1()
  );

  cSetupAssistantConfig::setInstFilename(
    m_current_genwrap->qStringValue("INST_SET", ok).latin1()
  );
  cSetupAssistantConfig::setEventFilename(
    m_current_genwrap->qStringValue("EVENT_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setAnalyzeFilename(
    m_current_genwrap->qStringValue("ANALYZE_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setEnvFilename(
    m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setStartCreatureFilename(
    m_current_genwrap->qStringValue("START_CREATURE", ok).latin1()
  );

  cSetupAssistantConfig::setMaxUpdates(
    m_current_genwrap->intValue("MAX_UPDATES", ok)
  );
  cSetupAssistantConfig::setMaxGenerations(
    m_current_genwrap->intValue("MAX_GENERATIONS", ok)
  );
  cSetupAssistantConfig::setEndConditionMode(
    m_current_genwrap->intValue("END_CONDITION_MODE", ok)
  );
  cSetupAssistantConfig::setWorldX(
    m_current_genwrap->intValue("WORLD-X", ok)
  );
  cSetupAssistantConfig::setWorldY(
    m_current_genwrap->intValue("WORLD-Y", ok)
  );

  cSetupAssistantConfig::setBirthMethod(
    m_current_genwrap->intValue("BIRTH_METHOD", ok)
  );
  cSetupAssistantConfig::setDeathMethod(
    m_current_genwrap->intValue("DEATH_METHOD", ok)
  );
  cSetupAssistantConfig::setAllocMethod(
    m_current_genwrap->intValue("ALLOC_METHOD", ok)
  );
  cSetupAssistantConfig::setDivideMethod(
    m_current_genwrap->intValue("DIVIDE_METHOD", ok)
  );
  cSetupAssistantConfig::setRequiredTask(
    m_current_genwrap->intValue("REQUIRED_TASK", ok)
  );
  cSetupAssistantConfig::setLineageCreationMethod(
    m_current_genwrap->intValue("LINEAGE_CREATION_METHOD", ok)
  );
  cSetupAssistantConfig::setGenerationIncMethod(
    m_current_genwrap->intValue("GENERATION_INC_METHOD", ok)
  );
  cSetupAssistantConfig::setAgeLimit(
    m_current_genwrap->intValue("AGE_LIMIT", ok)
  );
  cSetupAssistantConfig::setChildSizeRange(
  m_current_genwrap->dblValue("CHILD_SIZE_RANGE", ok)
  ); 
  cSetupAssistantConfig::setMinCopiedLines(
  m_current_genwrap->dblValue("MIN_COPIED_LINES", ok)
  );
  cSetupAssistantConfig::setMinExeLines(
  m_current_genwrap->dblValue("MIN_EXE_LINES", ok)
  );
  cSetupAssistantConfig::setRequireAllocate(
    m_current_genwrap->intValue("REQUIRE_ALLOCATE", ok)
  );

  cSetupAssistantConfig::setRevertFatal(
  m_current_genwrap->dblValue("REVERT_FATAL", ok)
  ); 
  cSetupAssistantConfig::setRevertNeg(
  m_current_genwrap->dblValue("REVERT_DETRIMENTAL", ok)
  );
  cSetupAssistantConfig::setRevertNeut(
  m_current_genwrap->dblValue("REVERT_NEUTRAL", ok)
  );
  cSetupAssistantConfig::setRevertPos(
  m_current_genwrap->dblValue("REVERT_BENEFICIAL", ok)
  );
  cSetupAssistantConfig::setSterilizeFatal(
  m_current_genwrap->dblValue("STERILIZE_FATAL", ok)
  );
  cSetupAssistantConfig::setSterilizeNeg(
  m_current_genwrap->dblValue("STERILIZE_DETRIMENTAL", ok)
  );
  cSetupAssistantConfig::setSterilizeNeut(
  m_current_genwrap->dblValue("STERILIZE_NEUTRAL", ok)
  );
  cSetupAssistantConfig::setSterilizePos(
  m_current_genwrap->dblValue("STERILIZE_BENEFICIAL", ok)
  );
  // watch-out.  call next function only after calling previous eight.
  cSetupAssistantConfig::setTestOnDivide();
  cSetupAssistantConfig::setFailImplicit(
    m_current_genwrap->intValue("FAIL_IMPLICIT", ok)
  );

  cSetupAssistantConfig::setSpeciesThreshold(
    m_current_genwrap->intValue("SPECIES_THRESHOLD", ok)
  );
  cSetupAssistantConfig::setThreshold(
    m_current_genwrap->intValue("THRESHOLD", ok)
  );
  cSetupAssistantConfig::setGenotypePrint(
    m_current_genwrap->intValue("GENOTYPE_PRINT", ok)
  );
  cSetupAssistantConfig::setSpeciesPrint(
    m_current_genwrap->intValue("SPECIES_PRINT", ok)
  );
  cSetupAssistantConfig::setSpeciesRecording(
    m_current_genwrap->intValue("SPECIES_RECORDING", ok)
  );
  cSetupAssistantConfig::setGenotypePrintDom(
    m_current_genwrap->intValue("GENOTYPE_PRINT_DOM", ok)
  );
  cSetupAssistantConfig::setTestCPUTimeMod(
    m_current_genwrap->intValue("TEST_CPU_TIME_MOD", ok)
  );
  cSetupAssistantConfig::setTrackMainLineage(
    m_current_genwrap->intValue("TRACK_MAIN_LINEAGE", ok)
  );

  cSetupAssistantConfig::setMaxCPUThreads(
    m_current_genwrap->intValue("MAX_CPU_THREADS", ok)
  );

  cSetupAssistantConfig::setSlicingMethod(
    m_current_genwrap->intValue("SLICING_METHOD", ok)
  );
  cSetupAssistantConfig::setSizeMeritMethod(
    m_current_genwrap->intValue("SIZE_MERIT_METHOD", ok)
  );
  cSetupAssistantConfig::setBaseSizeMerit(
    m_current_genwrap->intValue("BASE_SIZE_MERIT", ok)
  );
  cSetupAssistantConfig::setAveTimeSlice(
    m_current_genwrap->intValue("AVE_TIME_SLICE", ok)
  );
  cSetupAssistantConfig::setMeritTime(
    m_current_genwrap->intValue("MERIT_TIME", ok)
  );

  cSetupAssistantConfig::setTaskMeritMethod(
    m_current_genwrap->intValue("TASK_MERIT_METHOD", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setMaxNumTasksRewarded(
  //  m_current_genwrap->intValue("MAX_NUM_TASKS_REWARDED", ok)
  //);
  cSetupAssistantConfig::setMaxLabelExeSize(
    m_current_genwrap->intValue("MAX_LABEL_EXE_SIZE", ok)
  );

  cSetupAssistantConfig::setPointMutProb(
  m_current_genwrap->dblValue("POINT_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setCopyMutProb(
  m_current_genwrap->dblValue("COPY_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setInsMutProb(
  m_current_genwrap->dblValue("INS_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDelMutProb(
  m_current_genwrap->dblValue("DEL_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDivideMutProb(
  m_current_genwrap->dblValue("DIVIDE_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDivideInsProb(
  m_current_genwrap->dblValue("DIVIDE_INS_PROB", ok)
  );
  cSetupAssistantConfig::setDivideDelProb(
  m_current_genwrap->dblValue("DIVIDE_DEL_PROB", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setParentMutProb(
  //m_current_genwrap->dblValue("PARENT_MUT_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setCrossoverProb(
  //m_current_genwrap->dblValue("CROSSOVER_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setAlignedCrossoverProb(
  //m_current_genwrap->dblValue("ALIGNED_CROSS_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setExeErrProb(
  //m_current_genwrap->dblValue("EXE_ERROR_PROB", ok)
  //);

  cSetupAssistantConfig::setViewMode(
    m_current_genwrap->intValue("VIEW_MODE", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setViewerType(
  //  m_current_genwrap->intValue("VIEWER_TYPE", ok)
  //);

/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogBreedCount(
  //  m_current_genwrap->intValue("LOG_BREED_COUNT", ok)
  //);
  cSetupAssistantConfig::setLogCreatures(
    m_current_genwrap->intValue("LOG_CREATURES", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogPhylogeny(
  //  m_current_genwrap->intValue("LOG_PHYLOGENY", ok)
  //);

  cSetupAssistantConfig::setLogGenotypes(
    m_current_genwrap->intValue("LOG_GENOTYPES", ok)
  );
  // watch out.  only call after calling setLogGenotypes().
  cSetupAssistantConfig::setLogThresholdOnly();
  cSetupAssistantConfig::setLogThreshold(
    m_current_genwrap->intValue("LOG_THRESHOLD", ok)
  );
  cSetupAssistantConfig::setLogSpecies(
    m_current_genwrap->intValue("LOG_SPECIES", ok)
  );
  cSetupAssistantConfig::setLogLandscape(
    m_current_genwrap->intValue("LOG_LANDSCAPE", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogMutations(
  //  m_current_genwrap->intValue("LOG_MUTATIONS", ok)
  //);
  cSetupAssistantConfig::setLogLineages(
    m_current_genwrap->intValue("LOG_LINEAGES", ok)
  );

  cSetupAssistantConfig::setRandomSeed(
    m_current_genwrap->intValue("RANDOM_SEED", ok)
  );


  GenDebug(" --- done, happy.  current directory:  ")(QDir::currentDirPath());

  return result;
}


QString
TestWizard::modifiedSavegenesisPageSetup(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;
  int intval;
  double dblval;
  QString strval;

    // FIXME:  error-checks
    //strval = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
    //if(ok) m_modified_world_lb->setText(strval);
    //else GenDebug(" --- ");

    //strval = m_current_genwrap->qStringValue("DEFAULT_DIR", ok);
    //if(ok) m_modified_workdir_lb->setText(strval);
    //else GenDebug(" --- ");

    strval = m_current_genwrap->qStringValue("GENESIS_PATH", ok);
    if(ok) m_modified_world_lb->setText(strval);
    else GenDebug(" --- ");

    strval = m_current_genwrap->qStringValue("DEFAULT_DIR", ok);
    QFileInfo working_dirinfo(strval);
    if(ok){
      if(strval != QString::null){
        if(working_dirinfo.exists()){
          if(working_dirinfo.isDir()){
            if(working_dirinfo.isReadable()){
              if(working_dirinfo.isWritable()){
                m_modified_workdir_lb->setText(working_dirinfo.absFilePath());
              } else GenDebug(" --- !working_dirinfo.isWritable()");
            } else GenDebug(" --- !working_dirinfo.isReadable()");
          } else GenDebug(" --- !working_dirinfo.isDir()");
        } else GenDebug(" --- !working_dirinfo.exists()");
      } else GenDebug(" --- strval == QString::null");
    } else GenDebug(" --- bad key:  DEFAULT_DIR");

  result = title(page);
  return result;
}
QString
TestWizard::modifiedSavegenesisPageValidator(QWidget *page){
  GenDebug("entered.");

  QString result = QString::null;
  bool ok;

  QString genesis_filename = m_modified_world_lb->text();
  QFileInfo genesis_fileinfo(genesis_filename);
  if(genesis_filename.isEmpty()){
    result += "no world file chosen\n";
  } else if(!genesis_fileinfo.exists()){
    // okidoke, we'll just make a new one.
  //  result += "selected world file does not exist\n";
  } else if(!genesis_fileinfo.isWritable()){
    result += "selected world \"";
    result += genesis_filename;
    result += "\" is not writable\n";
  }

  QString working_dirname = m_modified_workdir_lb->text();
  QFileInfo working_dirinfo(working_dirname);
  if(working_dirname.isEmpty()){
    result += "no work directory chosen\n";
  } else if(!working_dirinfo.exists()){
    result += "work directory does not exist\n";
  } else if(!working_dirinfo.isDir()){
    result += "file chosen instead of directory for work directory\n";
  } else if(!working_dirinfo.isReadable()){
    result += "work directory is not readable\n";
  } else if(!working_dirinfo.isWritable()){
    result += "work directory is not writable\n";
  }

  if (!result.isEmpty()) return result;

  //m_current_genwrap->loadGenesis(genesis_fileinfo.absFilePath());

  QString inst_set_filename = m_current_genwrap->qStringValue("INST_SET", ok);
  QFileInfo inst_set_fileinfo(inst_set_filename);
  if(inst_set_fileinfo.isRelative()){
    GenDebug(" --- inst_set_fileinfo.isRelative()");
    inst_set_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      inst_set_filename
    );
  }
  if(!ok){
    result += "programmer error:  INST_SET is an invalid key\n";
  } else if(inst_set_filename.isEmpty()){
    result
      += "instruction set file specified in world file is blank\n";
  } else if(!inst_set_fileinfo.exists()){
    result
      += "instruction set file specified in world file doesn't exist\n";
  } else if(!inst_set_fileinfo.isReadable()){
    result
      += "instruction set file specified in world file can't be read\n";
  }

  QString start_creature_filename
    = m_current_genwrap->qStringValue("START_CREATURE", ok);
  QFileInfo start_creature_fileinfo(start_creature_filename);
  if(start_creature_fileinfo.isRelative()){
    GenDebug(" --- start_creature_fileinfo.isRelative()");
    start_creature_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      start_creature_filename
    );
  }
  if(!ok){
    result += "programmer error:  START_CREATURE is an invalid key\n";
  } else if(start_creature_filename.isEmpty()){
    result
      += "start creature file specified in world file is blank\n";
  } else if(!start_creature_fileinfo.exists()){
    result
      += "start creature file specified in world file doesn't exist\n";
  } else if(!start_creature_fileinfo.isReadable()){
    result
      += "start creature file specified in world file can't be read\n";
  }

  QString event_file_filename
    = m_current_genwrap->qStringValue("EVENT_FILE", ok);
  QFileInfo event_file_fileinfo(event_file_filename);
  if(event_file_fileinfo.isRelative()){
    GenDebug(" --- event_file_fileinfo.isRelative()");
    event_file_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      event_file_filename
    );
  }
  if(!ok){
    result += "programmer error:  EVENT_FILE is an invalid key\n";
  } else if(event_file_filename.isEmpty()){
    result
      += "event file specified in world file is blank\n";
  } else if(!event_file_fileinfo.exists()){
    result
      += "event file specified in world file doesn't exist\n";
  } else if(!event_file_fileinfo.isReadable()){
    result
      += "event file specified in world file can't be read\n";
  }

  QString environment_filename
    = m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok);
  QFileInfo environment_fileinfo(environment_filename);
  if(environment_fileinfo.isRelative()){
    GenDebug(" --- environment_fileinfo.isRelative()");
    environment_fileinfo.setFile(
      QDir(genesis_fileinfo.dirPath()),
      environment_filename
    );
  }
  if(!ok){
    result += "programmer error:  ENVIRONMENT_FILE is an invalid key\n";
  } else if(environment_filename.isEmpty()){
    result
      += "environment file specified in world file is blank\n";
  } else if(!environment_fileinfo.exists()){
    result
      += "environment file specified in world file doesn't exist\n";
  } else if(!environment_fileinfo.isReadable()){
    result
      += "environment file specified in world file can't be read\n";
  }

  QDir::setCurrent(working_dirname);

  if (!result.isEmpty()) return result;

  m_current_genwrap->set(
    "GENESIS_PATH",
    genesis_filename
  );
  m_current_genwrap->set(
    "DEFAULT_DIR",
    working_dirname
  );
  m_current_genwrap->saveGenesis(genesis_filename);

  cSetupAssistantConfig::setDefaultDir(
    m_current_genwrap->qStringValue("DEFAULT_DIR", ok).latin1()
  );

  cSetupAssistantConfig::setInstFilename(
    m_current_genwrap->qStringValue("INST_SET", ok).latin1()
  );
  cSetupAssistantConfig::setEventFilename(
    m_current_genwrap->qStringValue("EVENT_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setAnalyzeFilename(
    m_current_genwrap->qStringValue("ANALYZE_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setEnvFilename(
    m_current_genwrap->qStringValue("ENVIRONMENT_FILE", ok).latin1()
  );
  cSetupAssistantConfig::setStartCreatureFilename(
    m_current_genwrap->qStringValue("START_CREATURE", ok).latin1()
  );

  cSetupAssistantConfig::setMaxUpdates(
    m_current_genwrap->intValue("MAX_UPDATES", ok)
  );
  cSetupAssistantConfig::setMaxGenerations(
    m_current_genwrap->intValue("MAX_GENERATIONS", ok)
  );
  cSetupAssistantConfig::setEndConditionMode(
    m_current_genwrap->intValue("END_CONDITION_MODE", ok)
  );
  cSetupAssistantConfig::setWorldX(
    m_current_genwrap->intValue("WORLD-X", ok)
  );
  cSetupAssistantConfig::setWorldY(
    m_current_genwrap->intValue("WORLD-Y", ok)
  );

  cSetupAssistantConfig::setBirthMethod(
    m_current_genwrap->intValue("BIRTH_METHOD", ok)
  );
  cSetupAssistantConfig::setDeathMethod(
    m_current_genwrap->intValue("DEATH_METHOD", ok)
  );
  cSetupAssistantConfig::setAllocMethod(
    m_current_genwrap->intValue("ALLOC_METHOD", ok)
  );
  cSetupAssistantConfig::setDivideMethod(
    m_current_genwrap->intValue("DIVIDE_METHOD", ok)
  );
  cSetupAssistantConfig::setRequiredTask(
    m_current_genwrap->intValue("REQUIRED_TASK", ok)
  );
  cSetupAssistantConfig::setLineageCreationMethod(
    m_current_genwrap->intValue("LINEAGE_CREATION_METHOD", ok)
  );
  cSetupAssistantConfig::setGenerationIncMethod(
    m_current_genwrap->intValue("GENERATION_INC_METHOD", ok)
  );
  cSetupAssistantConfig::setAgeLimit(
    m_current_genwrap->intValue("AGE_LIMIT", ok)
  );
  cSetupAssistantConfig::setChildSizeRange(
  m_current_genwrap->dblValue("CHILD_SIZE_RANGE", ok)
  ); 
  cSetupAssistantConfig::setMinCopiedLines(
  m_current_genwrap->dblValue("MIN_COPIED_LINES", ok)
  );
  cSetupAssistantConfig::setMinExeLines(
  m_current_genwrap->dblValue("MIN_EXE_LINES", ok)
  );
  cSetupAssistantConfig::setRequireAllocate(
    m_current_genwrap->intValue("REQUIRE_ALLOCATE", ok)
  );

  cSetupAssistantConfig::setRevertFatal(
  m_current_genwrap->dblValue("REVERT_FATAL", ok)
  ); 
  cSetupAssistantConfig::setRevertNeg(
  m_current_genwrap->dblValue("REVERT_DETRIMENTAL", ok)
  );
  cSetupAssistantConfig::setRevertNeut(
  m_current_genwrap->dblValue("REVERT_NEUTRAL", ok)
  );
  cSetupAssistantConfig::setRevertPos(
  m_current_genwrap->dblValue("REVERT_BENEFICIAL", ok)
  );
  cSetupAssistantConfig::setSterilizeFatal(
  m_current_genwrap->dblValue("STERILIZE_FATAL", ok)
  );
  cSetupAssistantConfig::setSterilizeNeg(
  m_current_genwrap->dblValue("STERILIZE_DETRIMENTAL", ok)
  );
  cSetupAssistantConfig::setSterilizeNeut(
  m_current_genwrap->dblValue("STERILIZE_NEUTRAL", ok)
  );
  cSetupAssistantConfig::setSterilizePos(
  m_current_genwrap->dblValue("STERILIZE_BENEFICIAL", ok)
  );
  // watch-out.  call next function only after calling previous eight.
  cSetupAssistantConfig::setTestOnDivide();
  cSetupAssistantConfig::setFailImplicit(
    m_current_genwrap->intValue("FAIL_IMPLICIT", ok)
  );

  cSetupAssistantConfig::setSpeciesThreshold(
    m_current_genwrap->intValue("SPECIES_THRESHOLD", ok)
  );
  cSetupAssistantConfig::setThreshold(
    m_current_genwrap->intValue("THRESHOLD", ok)
  );
  cSetupAssistantConfig::setGenotypePrint(
    m_current_genwrap->intValue("GENOTYPE_PRINT", ok)
  );
  cSetupAssistantConfig::setSpeciesPrint(
    m_current_genwrap->intValue("SPECIES_PRINT", ok)
  );
  cSetupAssistantConfig::setSpeciesRecording(
    m_current_genwrap->intValue("SPECIES_RECORDING", ok)
  );
  cSetupAssistantConfig::setGenotypePrintDom(
    m_current_genwrap->intValue("GENOTYPE_PRINT_DOM", ok)
  );
  cSetupAssistantConfig::setTestCPUTimeMod(
    m_current_genwrap->intValue("TEST_CPU_TIME_MOD", ok)
  );
  cSetupAssistantConfig::setTrackMainLineage(
    m_current_genwrap->intValue("TRACK_MAIN_LINEAGE", ok)
  );

  cSetupAssistantConfig::setMaxCPUThreads(
    m_current_genwrap->intValue("MAX_CPU_THREADS", ok)
  );

  cSetupAssistantConfig::setSlicingMethod(
    m_current_genwrap->intValue("SLICING_METHOD", ok)
  );
  cSetupAssistantConfig::setSizeMeritMethod(
    m_current_genwrap->intValue("SIZE_MERIT_METHOD", ok)
  );
  cSetupAssistantConfig::setBaseSizeMerit(
    m_current_genwrap->intValue("BASE_SIZE_MERIT", ok)
  );
  cSetupAssistantConfig::setAveTimeSlice(
    m_current_genwrap->intValue("AVE_TIME_SLICE", ok)
  );
  cSetupAssistantConfig::setMeritTime(
    m_current_genwrap->intValue("MERIT_TIME", ok)
  );

  cSetupAssistantConfig::setTaskMeritMethod(
    m_current_genwrap->intValue("TASK_MERIT_METHOD", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setMaxNumTasksRewarded(
  //  m_current_genwrap->intValue("MAX_NUM_TASKS_REWARDED", ok)
  //);
  cSetupAssistantConfig::setMaxLabelExeSize(
    m_current_genwrap->intValue("MAX_LABEL_EXE_SIZE", ok)
  );

  cSetupAssistantConfig::setPointMutProb(
  m_current_genwrap->dblValue("POINT_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setCopyMutProb(
  m_current_genwrap->dblValue("COPY_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setInsMutProb(
  m_current_genwrap->dblValue("INS_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDelMutProb(
  m_current_genwrap->dblValue("DEL_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDivideMutProb(
  m_current_genwrap->dblValue("DIVIDE_MUT_PROB", ok)
  );
  cSetupAssistantConfig::setDivideInsProb(
  m_current_genwrap->dblValue("DIVIDE_INS_PROB", ok)
  );
  cSetupAssistantConfig::setDivideDelProb(
  m_current_genwrap->dblValue("DIVIDE_DEL_PROB", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setParentMutProb(
  //m_current_genwrap->dblValue("PARENT_MUT_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setCrossoverProb(
  //m_current_genwrap->dblValue("CROSSOVER_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setAlignedCrossoverProb(
  //m_current_genwrap->dblValue("ALIGNED_CROSS_PROB", ok)
  //);
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setExeErrProb(
  //m_current_genwrap->dblValue("EXE_ERROR_PROB", ok)
  //);

  cSetupAssistantConfig::setViewMode(
    m_current_genwrap->intValue("VIEW_MODE", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setViewerType(
  //  m_current_genwrap->intValue("VIEWER_TYPE", ok)
  //);

/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogBreedCount(
  //  m_current_genwrap->intValue("LOG_BREED_COUNT", ok)
  //);
  cSetupAssistantConfig::setLogCreatures(
    m_current_genwrap->intValue("LOG_CREATURES", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogPhylogeny(
  //  m_current_genwrap->intValue("LOG_PHYLOGENY", ok)
  //);

  cSetupAssistantConfig::setLogGenotypes(
    m_current_genwrap->intValue("LOG_GENOTYPES", ok)
  );
  // watch out.  only call after calling setLogGenotypes().
  cSetupAssistantConfig::setLogThresholdOnly();
  cSetupAssistantConfig::setLogThreshold(
    m_current_genwrap->intValue("LOG_THRESHOLD", ok)
  );
  cSetupAssistantConfig::setLogSpecies(
    m_current_genwrap->intValue("LOG_SPECIES", ok)
  );
  cSetupAssistantConfig::setLogLandscape(
    m_current_genwrap->intValue("LOG_LANDSCAPE", ok)
  );
/* FIXME -- removed from base class. -- k */
  //cSetupAssistantConfig::setLogMutations(
  //  m_current_genwrap->intValue("LOG_MUTATIONS", ok)
  //);
  cSetupAssistantConfig::setLogLineages(
    m_current_genwrap->intValue("LOG_LINEAGES", ok)
  );

  cSetupAssistantConfig::setRandomSeed(
    m_current_genwrap->intValue("RANDOM_SEED", ok)
  );


  GenDebug(" --- done, happy.  current directory:  ")(QDir::currentDirPath());

  return result;
}

