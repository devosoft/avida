#ifndef FILE_FIXUPS_HH
#define FILE_FIXUPS_HH

#ifndef FILE_HH
#include "cFile.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif

int cFileOpenWrapper(cFile &file, cString _filename);

#endif
