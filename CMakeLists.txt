PROJECT(AVIDA)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)


IF(COMMAND cmake_policy)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# The following three variables must be after the PROJECT statement, otherwise
# newer versions of cmake will (correctly) use an empty value of
# PROJECT_BINARY_DIR, since the project didn't exist yet.

# Default location for installed software/configs/ docs is the build directory.
SET(CMAKE_INSTALL_PREFIX
  "${PROJECT_BINARY_DIR}"
  CACHE PATH
  "Install path prefix, prepended onto install directories."
  FORCE
)

# Final software is built directly into the work subdirectory.
SET(EXECUTABLE_OUTPUT_PATH
  "${PROJECT_BINARY_DIR}/bin"
  CACHE PATH
  "Single output directory for building all executables."
)

SET(LIBRARY_OUTPUT_PATH
  "${PROJECT_BINARY_DIR}/lib"
  CACHE PATH
  "Built libraries are placed here before installation."
)



# This section defines default builtin compiler options
# ------------------------------------------------------------------------------
IF(UNIX)
  IF (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")
    SET(COMPILER_WARNING_FLAGS "")
    SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -OPT:Olimit=0")
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")
    IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
      SET(COMPILER_WARNING_FLAGS "")
      SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -mp1 -pc64")
    ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
      SET(COMPILER_WARNING_FLAGS "-Wextra -Wno-unknown-pragmas -Wno-trigraphs")
      SET(COMPILER_OPTIMIZATION_FLAGS "-funroll-loops -fstrict-aliasing -ftree-vectorize -fvisibility-inlines-hidden")
    ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*pathCC.*")

  INCLUDE(CheckCSourceCompiles)
  IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(HAVE_FUSED_MADD FALSE)
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(CMAKE_REQUIRED_FLAGS "-mno-fused-madd")
    CHECK_C_SOURCE_COMPILES("int main() { return 0; }" HAVE_FUSED_MADD)
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
  SET(CMAKE_REQUIRED_FLAGS "")

  IF(HAVE_FUSED_MADD)
    SET(COMPILER_OPTIMIZATION_FLAGS "-mno-fused-madd ${COMPILER_OPTIMIZATION_FLAGS}")
  ENDIF(HAVE_FUSED_MADD)

  # Four types of c++ compilations:
  # - debug (Debug)
  # - minimum-size release (MinSizeRel)
  # - release (Release)
  # - release with debug info (RelWithDebInfo)
  SET(CMAKE_CXX_FLAGS_DEBUG
    "-g ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during debug builds." FORCE)
  SET(CMAKE_CXX_FLAGS_MINSIZEREL
    "-Os ${COMPILER_WARNING_FLAGS} -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release minsize builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-O2 -g ${COMPILER_OPTIMIZATION_FLAGS} ${COMPILER_WARNING_FLAGS} -DDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELEASE
    "-O3 ${COMPILER_OPTIMIZATION_FLAGS} ${COMPILER_WARNING_FLAGS} -DNDEBUG"
    CACHE STRING "Flags used by the compiler during release builds." FORCE)
ENDIF(UNIX)


# Default build mode compiles c++ and c code with debug info and no
# optimizations.
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE
    Release
    CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    FORCE
  )
ENDIF(NOT CMAKE_BUILD_TYPE)



# Build Instructions for the Avida Core functionality
# - Below are groups of sources, based on directory.  Each appends the source
# - files to the master AVIDA_CORE_SOURCES list that is used to build a
# - static library that is linked into relevant targets.
# ------------------------------------------------------------------------------
SET(AVIDA_CORE_SOURCES)
SET(ALL_INC_DIRS
  ${PROJECT_SOURCE_DIR}/source
)


# The actions directory
SET(ACTIONS_DIR ${PROJECT_SOURCE_DIR}/source/actions)
SET(ACTIONS_SOURCES
  ${ACTIONS_DIR}/cActionLibrary.cc
  ${ACTIONS_DIR}/DriverActions.cc
  ${ACTIONS_DIR}/EnvironmentActions.cc
  ${ACTIONS_DIR}/LandscapeActions.cc
  ${ACTIONS_DIR}/PopulationActions.cc
  ${ACTIONS_DIR}/PrintActions.cc
  ${ACTIONS_DIR}/SaveLoadActions.cc
)
SOURCE_GROUP(actions FILES ${ACTIONS_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${ACTIONS_SOURCES})
LIST(APPEND ALL_INC_DIRS ${ACTIONS_DIR})


# The analyze directory
SET(ANALYZE_DIR ${PROJECT_SOURCE_DIR}/source/analyze)
SET(ANALYZE_SOURCES
  ${ANALYZE_DIR}/cAnalyze.cc
  ${ANALYZE_DIR}/cAnalyzeGenotype.cc
  ${ANALYZE_DIR}/cAnalyzeTreeStats_CumulativeStemminess.cc
  ${ANALYZE_DIR}/cAnalyzeTreeStats_Gamma.cc
  ${ANALYZE_DIR}/cAnalyzeJobQueue.cc
  ${ANALYZE_DIR}/cAnalyzeJobWorker.cc
  ${ANALYZE_DIR}/cGenotypeBatch.cc
  ${ANALYZE_DIR}/cGenotypeData.cc
  ${ANALYZE_DIR}/cModularityAnalysis.cc
  ${ANALYZE_DIR}/cMutationalNeighborhood.cc
  ${ANALYZE_DIR}/cPhenPlastSummary.h
)
SOURCE_GROUP(analyze FILES ${ANALYZE_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${ANALYZE_SOURCES})
LIST(APPEND ALL_INC_DIRS ${ANALYZE_DIR})


# The classification directory
SET(CLASSIFICATION_DIR ${PROJECT_SOURCE_DIR}/source/classification)
SET(CLASSIFICATION_SOURCES
  ${CLASSIFICATION_DIR}/cBGGenotype.cc
  ${CLASSIFICATION_DIR}/cBGGenotypeManager.cc
  ${CLASSIFICATION_DIR}/cBioGroup.cc
  ${CLASSIFICATION_DIR}/cBioGroupListener.cc
  ${CLASSIFICATION_DIR}/cBioGroupManager.cc
  ${CLASSIFICATION_DIR}/cBioUnit.cc
  ${CLASSIFICATION_DIR}/cClassificationManager.cc
  ${CLASSIFICATION_DIR}/cGenomeTestMetrics.cc
  ${CLASSIFICATION_DIR}/cMutationSteps.cc
  ${CLASSIFICATION_DIR}/cSexualAncestry.cc
)
SOURCE_GROUP(classification FILES ${CLASSIFICATION_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${CLASSIFICATION_SOURCES})
LIST(APPEND ALL_INC_DIRS ${CLASSIFICATION_DIR})


# The core viewer directory
SET(COREVIEW_DIR ${PROJECT_SOURCE_DIR}/source/viewer-core)
SET(COREVIEW_SOURCES
  ${COREVIEW_DIR}/cCoreView_Info.cc
  ${COREVIEW_DIR}/cCoreView_Map.cc
)
SOURCE_GROUP(coreview FILES ${COREVIEW_SOURCES})
LIST(APPEND ALL_INC_DIRS ${COREVIEW_DIR})


# The core GUI  directory
SET(COREGUI_DIR ${PROJECT_SOURCE_DIR}/source/viewer-coreGUI)
SET(COREGUI_SOURCES
  ${COREGUI_DIR}/cColor.cc
  ${COREGUI_DIR}/cGUIWidget.cc
)
SOURCE_GROUP(coregui FILES ${COREGUI_SOURCES})
LIST(APPEND ALL_INC_DIRS ${COREGUI_DIR})


# The cpu directory
SET(CPU_DIR ${PROJECT_SOURCE_DIR}/source/cpu)
SET(CPU_SOURCES
  ${CPU_DIR}/cCodeLabel.cc
  ${CPU_DIR}/cCPUMemory.cc
  ${CPU_DIR}/cCPUStack.cc
  ${CPU_DIR}/cCPUTestInfo.cc
  ${CPU_DIR}/cHardwareBase.cc
  ${CPU_DIR}/cHardwareCPU.cc
  ${CPU_DIR}/cHardwareExperimental.cc
  ${CPU_DIR}/cHardwareGX.cc
  ${CPU_DIR}/cHardwareManager.cc
  ${CPU_DIR}/cHardwareSMT.cc
  ${CPU_DIR}/cHardwareStatusPrinter.cc
  ${CPU_DIR}/cHardwareTransSMT.cc
  ${CPU_DIR}/cHeadCPU.cc
  ${CPU_DIR}/cInstSet.cc
  ${CPU_DIR}/cTestCPU.cc
  ${CPU_DIR}/cTestCPUInterface.cc
)
SOURCE_GROUP(cpu FILES ${CPU_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${CPU_SOURCES})
LIST(APPEND ALL_INC_DIRS ${CPU_DIR})


# The drivers directory
SET(DRIVERS_DIR ${PROJECT_SOURCE_DIR}/source/drivers)
SET(DRIVERS_SOURCES
  ${DRIVERS_DIR}/cDefaultAnalyzeDriver.cc
  ${DRIVERS_DIR}/cDefaultRunDriver.cc
  ${DRIVERS_DIR}/cDriverManager.cc
  ${DRIVERS_DIR}/cDriverStatusConduit.cc
  ${DRIVERS_DIR}/cFallbackWorldDriver.cc
)
SOURCE_GROUP(drivers FILES ${DRIVERS_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${DRIVERS_SOURCES})
LIST(APPEND ALL_INC_DIRS ${DRIVERS_DIR})


# The main directory
SET(MAIN_DIR ${PROJECT_SOURCE_DIR}/source/main)
SET(MAIN_SOURCES
  ${MAIN_DIR}/Avida.cc
  ${MAIN_DIR}/cAvidaConfig.cc
  ${MAIN_DIR}/cBirthChamber.cc
  ${MAIN_DIR}/cBirthDemeHandler.cc
  ${MAIN_DIR}/cBirthGenomeSizeHandler.cc
  ${MAIN_DIR}/cBirthGlobalHandler.cc
  ${MAIN_DIR}/cBirthGridLocalHandler.cc
  ${MAIN_DIR}/cBirthMateSelectHandler.cc
  ${MAIN_DIR}/cBirthNeighborhoodHandler.cc
  ${MAIN_DIR}/cBirthSelectionHandler.cc
  ${MAIN_DIR}/cDeme.cc
  ${MAIN_DIR}/cDemeNetwork.cc
  ${MAIN_DIR}/cDemeTopologyNetwork.cc
  ${MAIN_DIR}/cDemeCellEvent.cc
  ${MAIN_DIR}/cEnvironment.cc
  ${MAIN_DIR}/cEventList.cc
  ${MAIN_DIR}/cGenome.cc
  ${MAIN_DIR}/cGenomeUtil.cc
  ${MAIN_DIR}/cInstruction.cc
  ${MAIN_DIR}/cLandscape.cc
  ${MAIN_DIR}/cMutationRates.cc
  ${MAIN_DIR}/cOrganism.cc
  ${MAIN_DIR}/cOrgMessage.cc
  ${MAIN_DIR}/cParasite.cc
  ${MAIN_DIR}/cPhenotype.cc
  ${MAIN_DIR}/cPhenPlastGenotype.cc
  ${MAIN_DIR}/cPhenPlastUtil.cc
  ${MAIN_DIR}/cPlasticPhenotype.cc
  ${MAIN_DIR}/cPopulation.cc
  ${MAIN_DIR}/cPopulationCell.cc
  ${MAIN_DIR}/cPopulationInterface.cc
  ${MAIN_DIR}/cReaction.cc
  ${MAIN_DIR}/cReactionLib.cc
  ${MAIN_DIR}/cReactionResult.cc
  ${MAIN_DIR}/cResource.cc
  ${MAIN_DIR}/cResourceCount.cc
  ${MAIN_DIR}/cResourceHistory.cc
  ${MAIN_DIR}/cResourceLib.cc
  ${MAIN_DIR}/cSequence.cc
  ${MAIN_DIR}/cSpatialCountElem.cc
  ${MAIN_DIR}/cSpatialResCount.cc
  ${MAIN_DIR}/cStats.cc
  ${MAIN_DIR}/cTaskLib.cc
  ${MAIN_DIR}/cWorld.cc
)
SOURCE_GROUP(main FILES ${MAIN_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${MAIN_SOURCES})
LIST(APPEND ALL_INC_DIRS ${MAIN_DIR})


# The tools directory
SET(TOOLS_DIR ${PROJECT_SOURCE_DIR}/source/tools)
SET(TOOLS_SOURCES
  ${TOOLS_DIR}/AvidaTools.cc
  ${TOOLS_DIR}/cArgContainer.cc
  ${TOOLS_DIR}/cArgSchema.cc
  ${TOOLS_DIR}/cBitArray.cc
  ${TOOLS_DIR}/cChangeList.cc
  ${TOOLS_DIR}/cConstBurstSchedule.cc
  ${TOOLS_DIR}/cConstSchedule.cc
  ${TOOLS_DIR}/cDataFile.cc
  ${TOOLS_DIR}/cDataFileManager.cc
  ${TOOLS_DIR}/cDataManager_Base.cc
  ${TOOLS_DIR}/cDemeProbSchedule.cc
  ${TOOLS_DIR}/cFile.cc
  ${TOOLS_DIR}/cHelpAlias.cc
  ${TOOLS_DIR}/cHelpManager.cc
  ${TOOLS_DIR}/cHelpType.cc
  ${TOOLS_DIR}/cHistogram.cc
  ${TOOLS_DIR}/cInitFile.cc
  ${TOOLS_DIR}/cIntSum.cc
  ${TOOLS_DIR}/cIntegratedSchedule.cc
  ${TOOLS_DIR}/cIntegratedScheduleNode.cc
  ${TOOLS_DIR}/cMerit.cc
  ${TOOLS_DIR}/cProbDemeProbSchedule.cc
  ${TOOLS_DIR}/cProbSchedule.cc
  ${TOOLS_DIR}/cRandom.cc
  ${TOOLS_DIR}/cRunningAverage.cc
  ${TOOLS_DIR}/cSchedule.cc
  ${TOOLS_DIR}/cString.cc
  ${TOOLS_DIR}/cStringIterator.cc
  ${TOOLS_DIR}/cStringList.cc
  ${TOOLS_DIR}/cStringUtil.cc
  ${TOOLS_DIR}/cWeightedIndex.cc
)
SOURCE_GROUP(tools FILES ${TOOLS_SOURCES})
LIST(APPEND AVIDA_CORE_SOURCES ${TOOLS_SOURCES})
LIST(APPEND ALL_INC_DIRS ${TOOLS_DIR})

# The platform directory
SET(PLATFORM_DIR ${PROJECT_SOURCE_DIR}/source/tools/platform)
SET(PLATFORM_SOURCES
  ${PLATFORM_DIR}/cThread.cc
  ${PLATFORM_DIR}/Platform.cc
)
LIST(APPEND AVIDA_CORE_SOURCES ${PLATFORM_SOURCES})
LIST(APPEND ALL_INC_DIRS ${PLATFORM_DIR})




# Create the static library from the master source list
INCLUDE_DIRECTORIES(${ALL_INC_DIRS})
ADD_LIBRARY(avidacore ${AVIDA_CORE_SOURCES})



# Build Instructions for the TCMalloc library
# ------------------------------------------------------------------------------
IF(UNIX)
  IF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(AVD_ENABLE_TCMALLOC FALSE)
  ELSE (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
    SET(AVD_ENABLE_TCMALLOC TRUE)
  ENDIF (CMAKE_CXX_COMPILER MATCHES ".*icpc.*")
ELSE(UNIX)
  SET(AVD_ENABLE_TCMALLOC FALSE)
ENDIF(UNIX)
IF(AVD_ENABLE_TCMALLOC)
  SET(TCMALLOC_DIR source/platform/tcmalloc-1.4)
  SET(TCMALLOC_SOURCES
    ${TCMALLOC_DIR}/atomicops-internals-x86.cc
    ${TCMALLOC_DIR}/central_freelist.cc
    ${TCMALLOC_DIR}/common.cc
    ${TCMALLOC_DIR}/internal_logging.cc
    ${TCMALLOC_DIR}/malloc_extension.cc
    ${TCMALLOC_DIR}/page_heap.cc
    ${TCMALLOC_DIR}/span.cc
    ${TCMALLOC_DIR}/spinlock.cc
    ${TCMALLOC_DIR}/static_vars.cc
    ${TCMALLOC_DIR}/sysinfo.cc
    ${TCMALLOC_DIR}/system-alloc.cc
    ${TCMALLOC_DIR}/tcmalloc.cc
    ${TCMALLOC_DIR}/thread_cache.cc
  )
  ADD_LIBRARY(tcmalloc ${TCMALLOC_SOURCES})
ENDIF(AVD_ENABLE_TCMALLOC)



# Target Processing
# - For each enabled target, process its build instructions.  Must occur after
# - avidacore has been defined.
# ------------------------------------------------------------------------------
OPTION(AVD_CMDLINE
  "Enable building standard command line Avida (fastest version)."
  ON
)
IF(AVD_CMDLINE)
  SET(AVIDA_CMDLINE_DIR source/targets/avida)
  SET(AVIDA_CMDLINE_SOURCES ${AVIDA_CMDLINE_DIR}/primitive.cc)
  SOURCE_GROUP(target\\avida FILES ${AVIDA_CMDLINE_SOURCES})
  ADD_EXECUTABLE(avida ${AVIDA_CMDLINE_SOURCES})

  SET(AVIDA_CMDLINE_LIBS avidacore)  
  IF(NOT MSVC)
    LIST(APPEND AVIDA_CMDLINE_LIBS pthread)
  ENDIF(NOT MSVC)
  IF(AVD_ENABLE_TCMALLOC)
    LIST(APPEND AVIDA_CMDLINE_LIBS tcmalloc)
  ENDIF(AVD_ENABLE_TCMALLOC)  
  TARGET_LINK_LIBRARIES(avida ${AVIDA_CMDLINE_LIBS})
  
  INSTALL_TARGETS(/work avida)
ENDIF(AVD_CMDLINE)


# By default, do not build the console interface to Avida.
OPTION(AVD_GUI_NCURSES
  "Enable building Avida console interface."
  OFF
)
# Make sure requisites are present for build of console interface.  Give
# user feedback if they're missing.
IF(AVD_GUI_NCURSES)
  # Locate the ncurses screen handling package (for Avida's console
  # interface) and the Qt graphics API (for Avida's graphic interface).
  INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)
  IF(NOT NCURSES_INCLUDE_PATH)
    MESSAGE("Unable to locate header files for the ncurses CRT screen handling package.  Please set the advanced variable NCURSES_INCLUDE_PATH to their location.")
  ENDIF(NOT NCURSES_INCLUDE_PATH)
  IF(NOT NCURSES_LIBRARY)
    MESSAGE("Unable to locate 'libncurses'.  Please set the advanced variable NCURSES_LIBRARY to its location.")
  ENDIF(NOT NCURSES_LIBRARY)

  IF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)

    INCLUDE_DIRECTORIES(${NCURSES_INCLUDE_PATH})
    
    SET(AVIDA_VIEWER_DIR source/targets/avida-viewer)
    SET(AVIDA_VIEWER_SOURCES
      ${AVIDA_VIEWER_DIR}/cAnalyzeScreen.cc
      ${AVIDA_VIEWER_DIR}/cAnalyzeView.cc
      ${AVIDA_VIEWER_DIR}/cBarScreen.cc
      ${AVIDA_VIEWER_DIR}/cEnvironmentScreen.cc
      ${AVIDA_VIEWER_DIR}/cHistScreen.cc
      ${AVIDA_VIEWER_DIR}/cMapScreen.cc
      ${AVIDA_VIEWER_DIR}/cMenuWindow.cc
      ${AVIDA_VIEWER_DIR}/cOptionsScreen.cc
      ${AVIDA_VIEWER_DIR}/cStatsScreen.cc
      ${AVIDA_VIEWER_DIR}/cSymbolUtil.cc
      ${AVIDA_VIEWER_DIR}/cTextViewerAnalyzeDriver.cc
      ${AVIDA_VIEWER_DIR}/cTextViewerDriver.cc
      ${AVIDA_VIEWER_DIR}/cTextViewerDriver_Base.cc
      ${AVIDA_VIEWER_DIR}/cTextWindow.cc
      ${AVIDA_VIEWER_DIR}/cView.cc
      ${AVIDA_VIEWER_DIR}/cViewInfo.cc
      ${AVIDA_VIEWER_DIR}/cZoomScreen.cc
      ${AVIDA_VIEWER_DIR}/viewer.cc
    )
    SOURCE_GROUP(targets\\avida-viewer FILES ${AVIDA_VIEWER_SOURCES})
    ADD_EXECUTABLE(avida-viewer ${AVIDA_VIEWER_SOURCES})
    
    SET(AVIDA_VIEWER_LIBS avidacore ${NCURSES_LIBRARY})
    IF(NOT MSVC)
      LIST(APPEND AVIDA_VIEWER_LIBS pthread)
    ENDIF(NOT MSVC)
    IF(AVD_ENABLE_TCMALLOC)
      LIST(APPEND AVIDA_VIEWER_LIBS tcmalloc)
    ENDIF(AVD_ENABLE_TCMALLOC)
    TARGET_LINK_LIBRARIES(avida-viewer ${AVIDA_VIEWER_LIBS})
    
    INSTALL_TARGETS(/work avida-viewer)

  ENDIF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)
ENDIF(AVD_GUI_NCURSES)



# By default, do not build the console interface to Avida.
OPTION(AVD_GUI_PROTO_TEXT
  "Enable building new Avida text interface."
  OFF
)
# Make sure requisites are present for build of console interface.  Give
# user feedback if they're missing.
IF(AVD_GUI_PROTO_TEXT)
  # Locate the ncurses screen handling package (for Avida's console
  # interface) and the Qt graphics API (for Avida's graphic interface).
  INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindNcurses.cmake)
  IF(NOT NCURSES_INCLUDE_PATH)
    MESSAGE("Unable to locate header files for the ncurses CRT screen handling package.  Please set the advanced variable NCURSES_INCLUDE_PATH to their location.")
  ENDIF(NOT NCURSES_INCLUDE_PATH)
  IF(NOT NCURSES_LIBRARY)
    MESSAGE("Unable to locate 'libncurses'.  Please set the advanced variable NCURSES_LIBRARY to its location.")
  ENDIF(NOT NCURSES_LIBRARY)

  IF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)

    INCLUDE_DIRECTORIES(${NCURSES_INCLUDE_PATH})
    
    SET(AVIDA_TEXT_VIEWER_DIR source/targets/viewer-text)
    SET(AVIDA_TEXT_VIEWER_SOURCES
      ${AVIDA_TEXT_VIEWER_DIR}/cScreen_Map.cc
      ${AVIDA_TEXT_VIEWER_DIR}/cTextWindow.cc
      ${AVIDA_TEXT_VIEWER_DIR}/cDriver_TextViewer.cc
      ${AVIDA_TEXT_VIEWER_DIR}/viewer-text.cc
    )
    SOURCE_GROUP(targets\\viewer-text FILES ${AVIDA_TEXT_VIEWER_SOURCES})
    ADD_EXECUTABLE(avida-text ${AVIDA_TEXT_VIEWER_SOURCES} ${COREVIEW_SOURCES} ${COREGUI_SOURCES})
    
    SET(AVIDA_TEXT_VIEWER_LIBS avidacore ${NCURSES_LIBRARY})
    IF(NOT MSVC)
      LIST(APPEND AVIDA_TEXT_VIEWER_LIBS pthread)
    ENDIF(NOT MSVC)
    IF(AVD_ENABLE_TCMALLOC)
      LIST(APPEND AVIDA_TEXT_VIEWER_LIBS tcmalloc)
    ENDIF(AVD_ENABLE_TCMALLOC)
    TARGET_LINK_LIBRARIES(avida-text ${AVIDA_TEXT_VIEWER_LIBS})
    
    INSTALL_TARGETS(/work avida-text)

  ENDIF(NCURSES_LIBRARY AND NCURSES_INCLUDE_PATH)
ENDIF(AVD_GUI_PROTO_TEXT)



# By default, do not build the console interface to Avida.
OPTION(AVD_GUI_PROTO_FLTK
  "Enable building new Avida FLTK graphical interface."
  OFF
)
# Make sure requisites are present for build of console interface.  Give
# user feedback if they're missing.
IF(AVD_GUI_PROTO_FLTK)
  # Locate the fltk GUI package 
  INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/FindFLTK.cmake)
  IF(NOT FLTK_INCLUDE_PATH)
    MESSAGE("Unable to locate header files for the FLTK graphics package.  Please set the advanced variable FLTK_INCLUDE_PATH to their location.")
  ENDIF(NOT FLTK_INCLUDE_PATH)
  IF(NOT FLTK_BASE_LIBRARY)
    MESSAGE("Unable to locate 'fltk.lib'.  Please set the advanced variable FLTK_BASE_LIBRARY to its location.")
  ENDIF(NOT FLTK_BASE_LIBRARY)

  IF(FLTK_BASE_LIBRARY AND FLTK_INCLUDE_PATH)

    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_PATH} source/targets/viewer-fltk)
    
    SET(AVIDA_FLTK_VIEWER_DIR source/targets/viewer-fltk)
    SET(AVIDA_FLTK_VIEWER_SOURCES
      ${AVIDA_FLTK_VIEWER_DIR}/cDriver_FLTKViewer.cc
      ${AVIDA_FLTK_VIEWER_DIR}/viewer-fltk.cc
      ${AVIDA_FLTK_VIEWER_DIR}/fltk-defs.cc
      ${AVIDA_FLTK_VIEWER_DIR}/cFLTKGridView.cc
      ${AVIDA_FLTK_VIEWER_DIR}/cFLTKLegend.cc
    )
    SOURCE_GROUP(targets\\viewer-fltk FILES ${AVIDA_FLTK_VIEWER_SOURCES})
    ADD_EXECUTABLE(avida-fltk ${AVIDA_FLTK_VIEWER_SOURCES} ${COREVIEW_SOURCES} ${COREGUI_SOURCES})
    
    SET(AVIDA_FLTK_VIEWER_LIBS avidacore ${FLTK_BASE_LIBRARY} ${FLTK_IMAGE_LIBRARY} ${FLTK_JPEG_LIBRARY})
    IF(NOT MSVC)
      LIST(APPEND AVIDA_FLTK_VIEWER_LIBS pthread)
    ENDIF(NOT MSVC)
    TARGET_LINK_LIBRARIES(avida-fltk ${AVIDA_FLTK_VIEWER_LIBS})
    SET_TARGET_PROPERTIES(avida-fltk PROPERTIES LINK_FLAGS ${FLTK_PLATFORM_DEPENDENT_LIBS})

    IF(APPLE AND NOT FLTK_APPLE_X11)
       SET( LINK_FLAGS_avida_fltk ${LINK_FLAGS} "-framework Carbon -framework Cocoa -framework ApplicationServices -lz")
    ENDIF(APPLE AND NOT FLTK_APPLE_X11)

    INSTALL_TARGETS(/work avida-fltk)

  ENDIF(FLTK_BASE_LIBRARY AND FLTK_INCLUDE_PATH)
ENDIF(AVD_GUI_PROTO_FLTK)



OPTION(AVD_TASK_EVENT_GEN
  "Enable building the task_event_gen utility"
  OFF
)
IF(AVD_TASK_EVENT_GEN)
  SET(UTILS_DIR source/utils)
  SET(TASK_EVENT_GEN_SOURCES
    ${TOOLS_DIR}/cFile.cc
    ${TOOLS_DIR}/cRandom.cc
    ${TOOLS_DIR}/cString.cc
    ${TOOLS_DIR}/cInitFIle.cc
    ${TOOLS_DIR}/cStringIterator.cc
    ${TOOLS_DIR}/cStringList.cc
    ${UTILS_DIR}/task_events/task_event_gen.cc
  )
  ADD_EXECUTABLE(task_event_gen ${TASK_EVENT_GEN_SOURCES})
  INSTALL_TARGETS(/work task_event_gen)
ENDIF(AVD_TASK_EVENT_GEN)


OPTION(AVD_UNIT_TESTS
  "Enable the unit-tests executable.  Running this target will test various low level functionality."
  OFF
)
IF(AVD_UNIT_TESTS)
  SET(UNIT_TESTS_DIR source/targets/unit-tests)
  SET(UNIT_TESTS_SOURCES
    ${UNIT_TESTS_DIR}/main.cc
    ${TOOLS_DIR}/cBitArray.cc
  )
  ADD_EXECUTABLE(unit-tests ${UNIT_TESTS_SOURCES})
  INSTALL_TARGETS(/work unit-tests)
ENDIF(AVD_UNIT_TESTS)


# Default Configuration Files
# - Installed into the work directory alongside selected targets
# ------------------------------------------------------------------------------
SET(CFG_FILES_DIR support/config)
SET(CFG_FILES
  ${CFG_FILES_DIR}/analyze.cfg
  ${CFG_FILES_DIR}/avida.cfg
  ${CFG_FILES_DIR}/environment.cfg
  ${CFG_FILES_DIR}/events.cfg
  ${CFG_FILES_DIR}/instset-heads.cfg
  ${CFG_FILES_DIR}/instset-heads-sex.cfg
  ${CFG_FILES_DIR}/instset-transsmt.cfg
  ${CFG_FILES_DIR}/default-heads.org
  ${CFG_FILES_DIR}/default-heads-sex.org
  ${CFG_FILES_DIR}/default-transsmt.org 
)
INSTALL_FILES(/work FILES ${CFG_FILES})
