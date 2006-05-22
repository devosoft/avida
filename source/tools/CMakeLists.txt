SET(libtools_a_SOURCES
  cChangeList.cc
  cConstSchedule.cc
  cDataEntry.cc
  cDataFile.cc
  cDataFileManager.cc
  cDataManager_Base.cc
  cDefaultMessageDisplay.cc
  cDoubleSum.cc
  cFile.cc
  cFixedCoords.cc
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

IF(AVD_SERIALIZATION)
  SET(libtools_a_SOURCES ${libtools_a_SOURCES} nTemplateTests.cc)
ENDIF(AVD_SERIALIZATION)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(tools ${libtools_a_SOURCES})
