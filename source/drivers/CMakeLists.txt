INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

SET(libdrivers_a_SOURCES
  cDefaultAnalyzeDriver.cc
  cDefaultRunDriver.cc
  cDriverManager.cc
  cFallbackWorldDriver.cc
)

ADD_LIBRARY(drivers ${libdrivers_a_SOURCES})

