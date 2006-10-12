/*
 *  cFixedBlock.h
 *  Avida
 *
 *  Called "fixed_block.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
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

  /**   
   * Serialize to and from archive.
   **/  
  template<class Archive>
  void serialize(Archive & a, const unsigned int version){
    a.ArkvObj("start_point", start_point);
  }   
};

#endif
