
SET(tcmalloc_SOURCES
  system-alloc.cc
  tcmalloc-logging.cc
  tcmalloc.cc
)

ADD_LIBRARY(tcmalloc ${tcmalloc_SOURCES})
