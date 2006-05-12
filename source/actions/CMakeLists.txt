INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libactions_a_SOURCES
  LandscapeActions.cc
  PrintActions.cc
)

ADD_LIBRARY(actions ${libactions_a_SOURCES})

