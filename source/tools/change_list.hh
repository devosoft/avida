//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2005 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CHANGE_LIST_HH
#define CHANGE_LIST_HH

#ifndef TARRAY_HH
#include "tArray.hh"
#endif

/**
 * This class provides an array of indices of changes to some list of
 * objects. Entries in the array of indices have the same order as
 * registered changes.
 **/ 

class cChangeList {
protected:
  /*
  Note that size of m_change_list is size of m_change_tracking, and that
  0 <= m_change_count <= size of m_change_tracking.
  */
  // Number of changes.
  unsigned int m_change_count;
  /*
  List of changed indices. When n changes are listed, the first n
  entries of m_change_list store the indices, and the remaining entries
  are invalid.
  */
  tArray<unsigned int> m_change_list;
  // m_change_tracking[i] is true iff i is in m_change_list.
  tArray<bool> m_change_tracking;

public:
  void ResizeClear(unsigned int capacity){
    m_change_list.ResizeClear(capacity);
    m_change_tracking.ResizeClear(capacity);
    m_change_list.SetAll(0);
    m_change_tracking.SetAll(false);
    m_change_count = 0;
  }

public:
  // Constructor.
  explicit cChangeList(unsigned int capacity = 0)
  : m_change_list(0)
  , m_change_tracking(0)
  , m_change_count(0)
  { ResizeClear(capacity); }

  //// Assignment operator.
  //cChangeList & operator= (const cChangeList & rhs) {
  //  m_change_list = rhs.m_change_list;
  //  m_change_tracking = rhs.m_change_tracking;
  //  m_change_count = rhs.m_change_count;
  //}

  //// Copy constructor.
  //explicit cChangeList(const cChangeList & rhs) : cChangeList(0) {
  //  this->operator=(rhs);
  //}

  // Destructor.
  //virtual ~cChangeList(){}

  // Interface Methods ///////////////////////////////////////////////////////

  unsigned int GetSize() const { return m_change_list.GetSize(); }

  unsigned int GetChangeCount() const { return m_change_count; }

  // Note that decreasing size invalidates stored changes.
  void Resize(unsigned int capacity) {
    if (capacity < m_change_list.GetSize()){
      ResizeClear(capacity);
    } else {
      m_change_list.Resize(capacity);
      m_change_tracking.Resize(capacity, false);
    }
  }
  
  // Unsafe version : assumes index is within change count.
  unsigned int GetChangeAt(unsigned int index) const {
    return m_change_list[index];
  }

  // Safe version : returns -1 if index is outside change count.
  int CheckChangeAt(unsigned int index) const {
    return (index < m_change_count) ? ((int) GetChangeAt(index)) : (-1);
  }

  // Unsafe version : assumes changed_index is within capacity.
  void MarkChange(unsigned int changed_index) {
    if (!m_change_tracking[changed_index]) {
      m_change_tracking[changed_index] = true;
      m_change_list[m_change_count++] = changed_index;
    }
  }

  // Safe version : will resize to accommodate changed_index greater
  // than capacity.
  void PushChange(unsigned int changed_index) {
    if (m_change_list.GetSize() <= changed_index) {
      Resize(changed_index + 1);
    }
    MarkChange(changed_index);
  }

  void Reset() {
    for (unsigned int i = 0; i < m_change_count; i++) {
      m_change_tracking[m_change_list[i]] = false;
    }
    m_change_count = 0;
  }
};

#endif
