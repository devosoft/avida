//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef SPECIES_QUEUE_HH
#define SPECIES_QUEUE_HH

class cSpecies;
class cSpeciesQueue {
private:
  int size;
  cSpecies * first;
public:
  cSpeciesQueue();
  ~cSpeciesQueue();

  void InsertRear(cSpecies & new_species);
  void Remove(cSpecies & in_species);
  void Adjust(cSpecies & in_species);
  void Purge();
  bool OK(int queue_type);

  inline int GetSize() const { return size; }
  inline cSpecies * GetFirst() const { return first; }
};

#endif
