#ifndef FILE_FIXUPS_HH
#define FILE_FIXUPS_HH

#ifndef FILE_HH
#include "cFile.h"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif

int cFileOpenWrapper(cFile &file, cString _filename);

#endif
