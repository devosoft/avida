//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_CONFIG_HH
#define SETUP_WIZARD_CONFIG_HH

#include "config.hh"

class cSetupAssistantConfig : public cConfig {
public:
  static void Setup(cGenesis &genesis);
  static void SetupFiles(cGenesis &genesis);
  static void SetupFiles(
    cString default_dir,
    cString inst_filename,
    cString event_filename,
    cString analyze_filename,
    cString env_filename,
    cString start_creature
  );
  static void ProcessConfiguration(
    int argc,
    char * argv[],
    cGenesis & genesis
  );

  static void setDefaultDir(cString);

  static void setInstFilename(cString);
  static void setEventFilename(cString);
  static void setAnalyzeFilename(cString);
  static void setEnvFilename(cString);
  static void setStartCreatureFilename(cString);

  static void setMaxUpdates(int);
  static void setMaxGenerations(int);
  static void setEndConditionMode(int);
  static void setWorldX(int);
  static void setWorldY(int);

  static void setBirthMethod(int);
  static void setDeathMethod(int);
  static void setAllocMethod(int);
  static void setDivideMethod(int);
  static void setRequiredTask(int);
  static void setLineageCreationMethod(int);
  static void setGenerationIncMethod(int);
  static void setAgeLimit(int);
  static void setChildSizeRange(double);
  static void setMinCopiedLines(double);
  static void setMinExeLines(double);
  static void setRequireAllocate(int);

  static void setRevertFatal(double);
  static void setRevertNeg(double);
  static void setRevertNeut(double);
  static void setRevertPos(double);
  static void setSterilizeFatal(double);
  static void setSterilizeNeg(double);
  static void setSterilizeNeut(double);
  static void setSterilizePos(double);
  // watch-out.  call next function only after calling previous eight.
  static void setTestOnDivide();
  static void setFailImplicit(int);

  static void setSpeciesThreshold(int);
  static void setThreshold(int);
  static void setGenotypePrint(int);
  static void setSpeciesPrint(int);
  static void setSpeciesRecording(int);
  static void setGenotypePrintDom(int);
  static void setTestCPUTimeMod(int);
  static void setTrackMainLineage(int);

  static void setMaxCPUThreads(int);

  static void setSlicingMethod(int);
  static void setSizeMeritMethod(int);
  static void setBaseSizeMerit(int);
  static void setAveTimeSlice(int);
  static void setMeritTime(int);

  static void setTaskMeritMethod(int);
  /* FIXME -- removed from base class. -- k */
  //static void setMaxNumTasksRewarded(int);
  static void setMaxLabelExeSize(int);

  static void setPointMutProb(double);
  static void setCopyMutProb(double);
  static void setInsMutProb(double);
  static void setDelMutProb(double);
  static void setDivideMutProb(double);
  static void setDivideInsProb(double);
  static void setDivideDelProb(double);
/* FIXME -- removed from base class. -- k */
  //static void setParentMutProb(double);
/* FIXME -- removed from base class. -- k */
  //static void setCrossoverProb(double);
/* FIXME -- removed from base class. -- k */
  //static void setAlignedCrossoverProb(double);
/* FIXME -- removed from base class. -- k */
  //static void setExeErrProb(double);

  static void setViewMode(int);
/* FIXME -- removed from base class. -- k */
  //static void setViewerType(int);

/* FIXME -- removed from base class. -- k */
  //static void setLogBreedCount(int);
  static void setLogCreatures(int);
/* FIXME -- removed from base class. -- k */
  //static void setLogPhylogeny(int);

  static void setLogGenotypes(int);
  // watch out.  only call after calling setLogGenotypes().
  static void setLogThresholdOnly();
  static void setLogThreshold(int);
  static void setLogSpecies(int);
  static void setLogLandscape(int);
  static void setLogMutations(int);
  static void setLogLineages(int);

  static void setRandomSeed(int);
};

#endif /* !SETUP_WIZARD_CONFIG_HH */

