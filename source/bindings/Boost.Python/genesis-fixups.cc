#ifndef GENESIS_FIXUPS_HH
#include "genesis-fixups.hh"
#endif

int cGenesisOpenWrapper(cGenesis &file, cString _filename){
  return file.Open(_filename);
}

