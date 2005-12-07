/*
 *  cFixedBlock.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cFixedBlock_h
#define cFixedBlock_h

/**
 * Class used by @ref cBlockStruct.
 **/

class cFixedBlock {
private:
  int start_point;   // Starting point in the cScaledBlock array.
public:
  cFixedBlock() : start_point(0) { ; }
  ~cFixedBlock() { ; }

  inline int GetStart() { return start_point; }
  inline void SetStart(int in_sp) { start_point = in_sp; }
};

#endif
