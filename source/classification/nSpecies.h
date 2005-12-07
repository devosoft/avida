/*
 *  nSpecies.h
 *  Avida
 *
 *  Created by David on 10/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#ifndef nSpecies_h
#define nSpecies_h

namespace nSpecies {
  enum tRecording { RECORD_OFF = 0, RECORD_FULL = 1, RECORD_LIMITED = 2 };
  enum { QUEUE_NONE = 0, QUEUE_ACTIVE, QUEUE_INACTIVE, QUEUE_GARBAGE };
  static const int MAX_DISTANCE = 20;
}

#endif
