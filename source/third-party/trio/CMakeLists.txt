SET(libtrio_a_SOURCES
  trio.c
  triostr.c
  trionan.c
)
ADD_LIBRARY(trio ${libtrio_a_SOURCES})
