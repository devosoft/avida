SET(libtools_a_SOURCES
  cBlockStruct.cc
  cChangeList.cc
  cConstSchedule.cc
  cDataFile.cc
  cDataFileManager.cc
  cDataManager_Base.cc
  cDefaultMessageDisplay.cc
  cDoubleSum.cc
  cFile.cc
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
  cRunningAverage.cc
  cSchedule.cc
  cString.cc
  cStringIterator.cc
  cStringList.cc
  cStringUtil.cc
  cThread.cc
  cTools.cc
  cWeightedIndex.cc
  cCycleCheck.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(tools ${libtools_a_SOURCES})

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

