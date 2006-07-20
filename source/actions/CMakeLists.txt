INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libactions_a_SOURCES
  cActionLibrary.cc
  DriverActions.cc
  LandscapeActions.cc
  PopulationActions.cc
  PrintActions.cc
  SaveLoadActions.cc
)

ADD_LIBRARY(actions ${libactions_a_SOURCES})

