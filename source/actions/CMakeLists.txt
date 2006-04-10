INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libactions_a_SOURCES
  LandscapeActions.cc
)

ADD_LIBRARY(actions ${libactions_a_SOURCES})

