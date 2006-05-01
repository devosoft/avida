/*
 *  tList.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef tList_h
#define tList_h

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif

#ifndef NULL
#define NULL 0
#endif

template <class T> class tListNode {
#if USE_tMemTrack
  tMemTrack<tListNode<T> > mt;
#endif
public:
  T * data;
  tListNode<T> * next;
  tListNode<T> * prev;
  
  tListNode() : data(NULL), next(this), prev(this) { ; }

  template<class Archive>
  void serialize(Archive & a, const unsigned int version){
    a.ArkvObj("data", data);
  }
};

template <class T> class tList;

template <class T> class tBaseIterator {
#if USE_tMemTrack
  tMemTrack<tBaseIterator<T> > mt;
#endif
  friend class tList<T>;
protected:
  virtual const tList<T> & GetConstList() = 0;
  virtual const tListNode<T> * GetConstNode() = 0;
public:
    tBaseIterator() { ; }
  virtual ~tBaseIterator() { ; }
  
  virtual void Set(tListNode<T> * in_node) = 0;
  virtual void Reset() = 0;
  
  virtual const T * GetConst() = 0;
  virtual const T * NextConst() = 0;
  virtual const T * PrevConst() = 0;
  
  virtual bool AtRoot() const = 0;
  virtual bool AtEnd() const = 0;
};

template <class T> class tListIterator : public tBaseIterator<T> {
#if USE_tMemTrack
  tMemTrack<tListIterator<T> > mt;
#endif
  friend class tList<T>;
private:
  tList<T> & list;
  tListNode<T> * node;
  
  const tList<T> & GetConstList() { return list; }
  const tListNode<T> * GetConstNode() { return node; }
public:
    explicit tListIterator(tList<T> & _list);
  explicit tListIterator(tList<T> & _list, tListNode<T> * start_node);
  ~tListIterator();
  
  void Set(tListNode<T> * in_node) { node = in_node; }
  void Reset();
  tListNode<T> * GetPos() { return node; }
  
  T * Get();
  T * Next();
  T * Prev();
  const T * GetConst() { return Get(); }
  const T * NextConst() { return Next(); }
  const T * PrevConst() { return Prev(); }
  
  bool Find(T * test_data);
  
  bool AtRoot() const;
  bool AtEnd() const;
  
  // Unique methods...
  T * Remove();
};

template <class T> class tConstListIterator : public tBaseIterator<T> {
#if USE_tMemTrack
  tMemTrack<tConstListIterator<T> > mt;
#endif
  friend class tList<T>;
private:
  const tList<T> & list;
  const tListNode<T> * node;
  
  const tList<T> & GetConstList() { return list; }
  const tListNode<T> * GetConstNode() { return node; }
public:
    explicit tConstListIterator(const tList<T> & _list);
  explicit tConstListIterator(const tList<T> & _list,
                              const tListNode<T> * start_node);
  ~tConstListIterator();
  
  void Set(tListNode<T> * in_node) { node = in_node; }
  void Reset();
  
  const T * Get();
  const T * Next();
  const T * Prev();
  const T * GetConst() { return Get(); }
  const T * NextConst() { return Next(); }
  const T * PrevConst() { return Prev(); }
  bool Find(const T * test_data);
  
  bool AtRoot() const;
  bool AtEnd() const;
};

template <class T> class tLWConstListIterator : public tBaseIterator<T>
{
#if USE_tMemTrack
  tMemTrack<tLWConstListIterator<T> > mt;
#endif
  friend class tList<T>;
private:
  const tList<T>& list;
  const tListNode<T>* node;
  
  const tList<T>& GetConstList() { return list; }
  const tListNode<T>* GetConstNode() { return node; }

public:
  explicit tLWConstListIterator(const tList<T>& _list) : list(_list), node(&(_list.root)) { ; }
  explicit tLWConstListIterator(const tList<T>& _list, const tListNode<T>* start_node) : list(_list), node(start_node) { ; }
  ~tLWConstListIterator() { ; }
  
  void Set(tListNode<T>* in_node) { node = in_node; }
  void Reset();
  
  const T* Get();
  const T* Next();
  const T* Prev();
  const T* GetConst() { return Get(); }
  const T* NextConst() { return Next(); }
  const T* PrevConst() { return Prev(); }
  bool Find(const T* test_data);
  
  bool AtRoot() const;
  bool AtEnd() const;
};

template <class T> class tList {
#if USE_tMemTrack
  tMemTrack<tList<T> > mt;
#endif
  friend class tBaseIterator<T>;
  friend class tListIterator<T>;
  friend class tConstListIterator<T>;
  friend class tLWConstListIterator<T>;
protected:
    tListNode<T> root;                     // Data root
  int size;
  mutable tListNode< tBaseIterator<T> > it_root; // Iterator root
  mutable int it_count;
  
  T * RemoveNode(tListNode<T> * out_node) {
    // Make sure we're not trying to delete the root node!
    if (out_node == &root) return NULL;
    
    // Adjust any iterators on the deleted node.
    tListNode< tBaseIterator<T> > * test_it = it_root.next;
    while (test_it != &it_root) {
      // If this iterator is on this node, move it back one.
      if (test_it->data->GetConstNode() == out_node) {
        test_it->data->PrevConst();
      }
      test_it = test_it->next;
    }
    
    // Save the data and patch up the linked list.
    T * out_data = out_node->data;
    out_node->prev->next = out_node->next;
    out_node->next->prev = out_node->prev;
    
    // Cleanup and return
    size--;
    delete out_node;
    return out_data;
  }
  
  // To be called from iterator constructor only!
  void AddIterator(tBaseIterator<T> * new_it) const {
    tListNode< tBaseIterator<T> > * new_node =
    new tListNode< tBaseIterator<T> >;
    new_node->data = new_it;
    new_node->next = it_root.next;
    new_node->prev = &it_root;
    it_root.next->prev = new_node;
    it_root.next = new_node;
    it_count++;
  }
  
  // To be called from iterator destructor only!
  void RemoveIterator(tBaseIterator<T> * old_it) const {
    tListNode< tBaseIterator<T> > * test_it = it_root.next;
    while (test_it->data != old_it) test_it = test_it->next;
    test_it->prev->next = test_it->next;
    test_it->next->prev = test_it->prev;
    delete test_it;
    it_count--;
  }
  
public:
    T * Pop() { return RemoveNode(root.next); }
  T * PopRear() { return RemoveNode(root.prev); }
  
  void Clear() { while (size > 0) Pop(); }
  
  void Append(const tList<T> & in_list) {
    tListNode<T> * cur_node = in_list.root.next;
    while (cur_node != &(in_list.root)) {
      PushRear(cur_node->data);
      cur_node = cur_node->next;
    }
  }
  
  void Copy(const tList<T> & in_list) {
    Clear();
    Append(in_list);
  }
  
  void Push(T * _in) {
    tListNode<T> * new_node = new tListNode<T>;
    new_node->data = _in;
    new_node->next = root.next;
    new_node->prev = &root;
    root.next->prev = new_node;
    root.next = new_node;
    size++;
  }
  
  void PushRear(T * _in) {
    tListNode<T> * new_node = new tListNode<T>;
    new_node->data = _in;
    new_node->next = &root;
    new_node->prev = root.prev;
    root.prev->next = new_node;
    root.prev = new_node;
    size++;
  }
  
  const T * GetFirst() const { return root.next->data; }
  const T * GetLast()  const { return root.prev->data; }
  T * GetFirst()             { return root.next->data; }
  T * GetLast()              { return root.prev->data; }
  
  T * GetPos(int pos) {
    if (pos >= GetSize()) return NULL;
    tListNode<T> * test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }
  
  const T * GetPos(int pos) const {
    if (pos >= GetSize()) return NULL;
    tListNode<T> * test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }
  
  void CircNext() { if (size > 0) PushRear(Pop()); }
  void CircPrev() { if (size > 0) Push(PopRear()); }
  
  T * Remove(tListIterator<T> & other) {
    if (&(other.list) != this) return NULL; // @CAO make this an assert?
    return RemoveNode(other.node);
  }
  
  T * Insert(tListIterator<T> & list_it, T * in_data) {
    tListNode<T> * cur_node = list_it.node;
    
    // Build the new node for the list...
    tListNode<T> * new_node = new tListNode<T>;
    new_node->data = in_data;
    
    // Insert the new node before the iterator...
    new_node->next = cur_node;
    new_node->prev = cur_node->prev;
    cur_node->prev->next = new_node;
    cur_node->prev = new_node;
    size++;
    
    return in_data;
  }
  
  
  //bool Remove(T * other) {
  T * Remove(T * other) {
    tListNode<T> * test = root.next;
    while (test != &root) {
      if (test->data == other) {
        RemoveNode(test);
        //return true;
        return other;
      }
      test = test->next;
    }
    //return false;
    return NULL;
  }
  
  int GetSize() const { return size; }
  
  // Copy another list onto the end of this one.
  void Append(tList<T> & other_list) {
    tListIterator<T> other_it(other_list);
    while (other_it.Next() != NULL) PushRear(other_it.Get());
  }
  
  // Empty out another list, transferring its contents to the end of this one.
  void Transfer(tList<T> & other_list) {
    // If the other list is empty, stop here.
    if (other_list.GetSize() == 0) return;
    
    // Hook this list into the other one.
    other_list.root.next->prev = root.prev;
    other_list.root.prev->next = &root;
    root.prev->next = other_list.root.next;
    root.prev       = other_list.root.prev;
    
    // Clean up the other list so it has no entries.
    other_list.root.next = &(other_list.root);
    other_list.root.prev = &(other_list.root);
    
    // Update the size
    size += other_list.size;
    other_list.size = 0;
    
    // Update all iterators in the other list to point at the root.
    tListNode< tBaseIterator<T> > * test_it = other_list.it_root.next;
    while (test_it != &other_list.it_root) {
      test_it->data->Reset();
      test_it = test_it->next;
    }
  }
  
  // Find by value
  T * Find(T * _in) const {
    tListNode<T> * test = root.next;
    while (test != &root) {
      if ( *(test->data) == *(_in) ) return test->data;
      test = test->next;
    }
    return NULL;
  }
  
  // Find by Pointer
  T * FindPtr(T * _in) const {
    tListNode<T> * test = root.next;
    while (test != &root) {
      if ( test->data == _in ) return test->data;
      test = test->next;
    }
    return NULL;
  }
  
  // Find the position of the node by its pointer 
  int FindPosPtr(T * _in) const { 
    tListNode<T> * test = root.next;
    int pos = 0;	
    while (test != &root) {
      if ( test->data == _in ) return pos;
      test = test->next;
      pos++;
    }
    return 0;
  }
  
  
  // Remove by position
  T * PopPos(int pos) {
    if (pos >= GetSize()) return NULL;
    tListNode<T> * test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return RemoveNode(test_node);
  }
  
  
public:
  tList() : size(0), it_count(0) { }
  ~tList() { Clear(); }


  // Save to archive
  template<class Archive>
  void save(Archive & a, const unsigned int version) const {
    // Save number of elements.
    unsigned int count = size;
    a.ArkvObj("count", count);
    // Save elements.
    const tListNode<T> * node = &root;
    while(count-- > 0){
      node = node->next;
      a.ArkvObj("item", node);
    }
  }


  // Load from archive
  template<class Archive>
  void load(Archive & a, const unsigned int version){
    Clear();
    // Retrieve number of elements.
    unsigned int count;
    a.ArkvObj("count", count);
    // Retrieve elements.
    while(count-- > 0){
      tListNode<T> * new_node(0);
      a.ArkvObj("item", new_node);

      new_node->next = &root;
      new_node->prev = root.prev;
      root.prev->next = new_node;
      root.prev = new_node;
      size++;
    }
  }


  // Ask archive to handle loads and saves separately
  template<class Archive>
  void serialize(Archive & a, const unsigned int version){
    a.SplitLoadSave(*this, version);
  }

private:
  tList(tList & _list) { ; }  // Never should be used...
};


// This is an extended version of tList that contains extra functions to
// allow method pointer associated with the object type being listed.
template <class T> class tListPlus : public tList<T> {
#if USE_tMemTrack
  tMemTrack<tListPlus<T> > mt;
#endif
private:
public:
  
  // Find by summing values until a specified total is reached.  
  T * FindSummedValue(int sum, int (T::*fun)() const) {
    int total = 0;
    tListNode<T> * test = this->root.next;
    while (test != &(this->root) && total < sum) {
      total += (test->data->*fun)();
      test = test->next;
    }
    return test->data;
  }
  
  T * PopIntValue(int (T::*fun)() const, int value) {
    tListNode<T> * test = this->root.next;
    while (test != &(this->root)) {
      if ( (test->data->*fun)() == value) return RemoveNode(test);
      test = test->next;
    }
    return NULL;
  }
  
  T * PopIntMax(int (T::*fun)() const) { 
    if (this->size == 0) return NULL;
    tListNode<T> * test = this->root.next;
    tListNode<T> * best = test;
    int max_val = (test->data->*fun)();
    while (test != &(this->root)) {
      const int cur_val = (test->data->*fun)();
      if ( cur_val > max_val ) {
        max_val = cur_val;
        best = test;
      }
      test = test->next;
    }
    return RemoveNode(best);
  }
  
  T * PopDoubleMax(double (T::*fun)() const) {
    if (this->size == 0) return NULL;
    tListNode<T> * test = this->root.next;
    tListNode<T> * best = test;
    double max_val = (test->data->*fun)();
    while (test != &(this->root)) {
      const double cur_val = (test->data->*fun)();
      if ( cur_val > max_val ) {
        max_val = cur_val;
        best = test;
      }
      test = test->next;
    }
    return RemoveNode(best);
  }
  
  int Count(int (T::*fun)() const) {
    int total = 0;
    tListNode<T> * test = this->root.next;
    while (test != &(this->root)) {
      total += (test->data->*fun)();
      test = test->next;
    }
    return total;
  }
  
};


////////////////////
//  tListIterator

template <class T> tListIterator<T>::tListIterator(tList<T> & _list)
: list(_list), node(&(_list.root))
{
  list.AddIterator(this);
}

template <class T> tListIterator<T>::tListIterator(tList<T> & _list,
                                                   tListNode<T> * start_node)
: list(_list), node(start_node)
{
  list.AddIterator(this);
}

template <class T> tListIterator<T>::~tListIterator()
{
  list.RemoveIterator(this);
}

template <class T> void tListIterator<T>::Reset()
{
  node = &(list.root);
}

template <class T> T * tListIterator<T>::Get()
{
  return node->data;
}

template <class T> T * tListIterator<T>::Next()
{
  node = node->next;
  return node->data;
}

template <class T> T * tListIterator<T>::Prev()
{
  node = node->prev;
  return node->data;
}

template <class T> bool tListIterator<T>::Find(T * test_data)
{
  for (node = list.root.next;
       node != &(list.root);
       node = node->next) {
    if (node->data == test_data) return true;
  }
  return false;
}

template <class T> bool tListIterator<T>::AtRoot() const
{
  return (node == &(list.root));
}

template <class T> bool tListIterator<T>::AtEnd() const
{
  return (node->next == &(list.root));
}

template <class T> T* tListIterator<T>::Remove()
{
  return list.RemoveNode(node);
}

/////////////////////////
//  tConstListIterator

template <class T> tConstListIterator<T>::tConstListIterator(const tList<T> & _list)
: list(_list), node(&(_list.root))
{
  list.AddIterator(this);
}

template <class T> tConstListIterator<T>::tConstListIterator(const tList<T> & _list, const tListNode<T> * start_node)
: list(_list), node(start_node)
{
  list.AddIterator(this);
}

template <class T> tConstListIterator<T>::~tConstListIterator()
{
  list.RemoveIterator(this);
}

template <class T> void tConstListIterator<T>::Reset()
{
  node = &(list.root);
}

template <class T> const T * tConstListIterator<T>::Get()
{
  return node->data;
}

template <class T> const T * tConstListIterator<T>::Next()
{
  node = node->next;
  return node->data;
}

template <class T> const T * tConstListIterator<T>::Prev()
{
  node = node->prev;
  return node->data;
}

template <class T> bool tConstListIterator<T>::Find(const T * test_data)
{
  for (node = list.root.next;
       node != &(list.root);
       node = node->next) {
    if (node->data == test_data) return true;
  }
  return false;
}

template <class T> bool tConstListIterator<T>::AtRoot() const
{
  return (node == &(list.root));
}

template <class T> bool tConstListIterator<T>::AtEnd() const
{
  return (node->next == &(list.root));
}

/////////////////////////
//  tLWConstListIterator

template <class T> void tLWConstListIterator<T>::Reset()
{
  node = &(list.root);
}

template <class T> const T* tLWConstListIterator<T>::Get()
{
  return node->data;
}

template <class T> const T* tLWConstListIterator<T>::Next()
{
  node = node->next;
  return node->data;
}

template <class T> const T* tLWConstListIterator<T>::Prev()
{
  node = node->prev;
  return node->data;
}

template <class T> bool tLWConstListIterator<T>::Find(const T* test_data)
{
  for (node = list.root.next; node != &(list.root); node = node->next) {
    if (node->data == test_data) return true;
  }
  return false;
}

template <class T> bool tLWConstListIterator<T>::AtRoot() const
{
  return (node == &(list.root));
}

template <class T> bool tLWConstListIterator<T>::AtEnd() const
{
  return (node->next == &(list.root));
}

#endif
