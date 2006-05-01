SET(libarchive_a_SOURCES
  cXMLArchive.cc
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(archive ${libarchive_a_SOURCES})
