//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TRETAINABLE_LIST_PROTO_HH
#define TRETAINABLE_LIST_PROTO_HH

template <class T> class tRetainableList
: public tRList<T, pUseRetains>, public cRetainable {};

template <class T> class tRetainableListIter
: public tRListIterator<T, pUseRetains>, public cRetainable {
public:
  explicit tRetainableListIter(
    tRetainableList<T> & _list
  ):tRListIterator<T, pUseRetains>(_list){}
};

template <class T> class tRetainableConstListIter
: public tConstRListIterator<T, pUseRetains>, public cRetainable {
public:
  explicit tRetainableConstListIter(
    const tRetainableList<T> & _list
  ):tConstRListIterator<T, pUseRetains>(_list){}
};

#endif /* TRETAINABLE_LIST_PROTO_HH */
