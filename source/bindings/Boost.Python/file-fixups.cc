#ifndef FILE_FIXUPS_HH
#include "file-fixups.hh"
#endif

int cFileOpenWrapper(cFile &file, cString _filename){
  return file.Open(_filename);
}
