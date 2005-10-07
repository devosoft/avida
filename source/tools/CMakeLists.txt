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
  help_alias.cc
  help_manager.cc
  help_type.cc
  histogram.cc
  indexed_block_struct.cc
	init_file.cc
  int_sum.cc
  integrated_schedule.cc
  integrated_schedule_node.cc
  message_display.cc
  merit.cc
  prob_schedule.cc
  random.cc
  ref_block.cc
  running_average.cc
  scaled_block.cc
  schedule.cc
  string.cc
  string_iterator.cc
  string_list.cc
  string_util.cc
  tools.cc
  weighted_index.cc

  cCycleCheck.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(tools ${libtools_a_SOURCES})

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

