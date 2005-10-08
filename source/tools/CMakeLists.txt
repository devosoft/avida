SET(libtools_a_SOURCES
  cBlockStruct.cc
  cChangeList.cc
  cConstSchedule.cc
  cCountTracker.cc
  cDataEntry.cc
  cDataFile.cc
  cDataFileManager.cc
  cDataManager_Base.cc
  cDefaultMessageDisplay.cc
  cDoubleSum.cc
  cFile.cc
  cFixedBlock.cc
	cGenesis.cc
  cHelpAlias.cc
  cHelpManager.cc
  cHelpType.cc
  cHistogram.cc
  cIndexedBlockStruct.cc
	cInitFile.cc
  cIntSum.cc
  cIntegratedSchedule.cc
  cIntegratedScheduleNode.cc
  cMessageDisplay.cc
  cMerit.cc
  cProbSchedule.cc
  cRandom.cc
  cRefBlock.cc
  cRunningAverage.cc
  cScaledBlock.cc
  cSchedule.cc
  cString.cc
  cStringIterator.cc
  cStringList.cc
  cStringUtil.cc
  tools.cc
  weighted_index.cc

  cCycleCheck.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(tools ${libtools_a_SOURCES})

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

