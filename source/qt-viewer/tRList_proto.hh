//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TRLIST_PROTO_HH
#define TRLIST_PROTO_HH

#ifndef NULL
#define NULL 0
#endif

template <class T> class pUseRetains {
public:
  T * Retain(T * _in){ if(_in) _in->retain(); return _in; }
  T * Release(T * _in){ if(_in) _in->release(); return _in; }
};

template <class T> class pNoRetains {
public:
  T * Retain(T * _in){ return _in; }
  T * Release(T * _in){ return _in; }
};

template <
  class T,
	template <class> class RetainPolicy = pNoRetains
> class tRListNode {
public:
  T * data;
  tRListNode<T, RetainPolicy> * next;
  tRListNode<T, RetainPolicy> * prev;

  tRListNode() : data(NULL), next(this), prev(this) { ; }
};

template <
  class T,
	template <class> class RetainPolicy
> class tRList;

template <
  class T,
	template <class> class RetainPolicy = pNoRetains
> class tRBaseIterator {
  friend class tRList<T, RetainPolicy>;
protected:
  virtual const tRList<T, RetainPolicy> & GetConstList() = 0;
  virtual const tRListNode<T, RetainPolicy> * GetConstNode() = 0;
public:
  tRBaseIterator() { ; }
  virtual ~tRBaseIterator() { ; }

  virtual const T * GetConst() = 0;
  virtual const T * NextConst() = 0;
  virtual const T * PrevConst() = 0;

  virtual bool AtRoot() const = 0;
  virtual bool AtEnd() const = 0;
};

template <
  class T,
	template <class> class RetainPolicy = pNoRetains
> class tRListIterator
: public tRBaseIterator<T, RetainPolicy>
{
  friend class tRList<T, RetainPolicy>;
private:
  tRList<T, RetainPolicy> & list;
  tRListNode<T, RetainPolicy> * node;

  const tRList<T, RetainPolicy> & GetConstList() { return list; }
  const tRListNode<T, RetainPolicy> * GetConstNode() { return node; }
public:
  explicit tRListIterator(tRList<T, RetainPolicy> & _list);
  ~tRListIterator();

  void Reset();

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

template <
  class T,
	template <class> class RetainPolicy = pNoRetains
> class tConstRListIterator : public tRBaseIterator<T, RetainPolicy> {
  friend class tRList<T, RetainPolicy>;
private:
  const tRList<T, RetainPolicy> & list;
  const tRListNode<T, RetainPolicy> * node;

  const tRList<T, RetainPolicy> & GetConstList() { return list; }
  const tRListNode<T, RetainPolicy> * GetConstNode() { return node; }
public:
  explicit tConstRListIterator(const tRList<T, RetainPolicy> & _list);
  ~tConstRListIterator();

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

template <
  class T,
	template <class> class RetainPolicy = pNoRetains
> class tRList : public RetainPolicy<T> {
  friend class tRBaseIterator<T, RetainPolicy>;
  friend class tRListIterator<T, RetainPolicy>;
  friend class tConstRListIterator<T, RetainPolicy>;
protected:
  tRListNode<T, RetainPolicy> root;                     // Data root
  int size;
  mutable tRListNode< tRBaseIterator<T, RetainPolicy>, RetainPolicy > it_root; // Iterator root
  mutable int it_count;

  T * RemoveNode(tRListNode<T, RetainPolicy> * out_node) {
    // Make sure we're not trying to delete the root node!
    if (out_node == &root) return NULL;

    // Adjust any iterators on the deleted node.
    tRListNode< tRBaseIterator<T, RetainPolicy>, RetainPolicy > * test_it = it_root.next;
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
    Release(out_data);
    return out_data;
  }

  // To be called from iterator constructor only!
  void AddIterator(tRBaseIterator<T, RetainPolicy> * new_it) const {
    tRListNode< tRBaseIterator<T, RetainPolicy>, RetainPolicy > * new_node =
      new tRListNode< tRBaseIterator<T, RetainPolicy>, RetainPolicy >;
    new_node->data = new_it;
    new_node->next = it_root.next;
    new_node->prev = &it_root;
    it_root.next->prev = new_node;
    it_root.next = new_node;
    it_count++;
  }

  // To be called from iterator destructor only!
  void RemoveIterator(tRBaseIterator<T, RetainPolicy> * old_it) const {
    tRListNode< tRBaseIterator<T, RetainPolicy>, RetainPolicy > * test_it =
      it_root.next;
    while (test_it->data != old_it) test_it = test_it->next;
    test_it->prev->next = test_it->next;
    test_it->next->prev = test_it->prev;
    delete test_it;
    it_count--;
  }

public:
  T * Pop() { return RemoveNode(root.next); }
  T * PopRear() { return RemoveNode(root.prev); }

  void Push(T * _in) {
    Retain(_in);
    tRListNode<T, RetainPolicy> * new_node = new tRListNode<T, RetainPolicy>;
    new_node->data = _in;
    new_node->next = root.next;
    new_node->prev = &root;
    root.next->prev = new_node;
    root.next = new_node;
    size++;
  }

  void PushRear(T * _in) {
    Retain(_in);
    tRListNode<T, RetainPolicy> * new_node = new tRListNode<T, RetainPolicy>;
    new_node->data = _in;
    new_node->next = &root;
    new_node->prev = root.prev;
    root.prev->next = new_node;
    root.prev = new_node;
    size++;
  }

  void Clear() { while (size > 0) Pop(); }

  const T * GetFirst() const { return root.next->data; }
  const T * GetLast()  const { return root.prev->data; }
  T * GetFirst()             { return root.next->data; }
  T * GetLast()              { return root.prev->data; }

  T * GetPos(int pos) {
    if (pos >= GetSize()) return NULL;
    tRListNode<T, RetainPolicy> * test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }

  const T * GetPos(int pos) const {
    if (pos >= GetSize()) return NULL;
    tRListNode<T, RetainPolicy> * test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }

  void CircNext() { if (size > 0) PushRear(Pop()); }
  void CircPrev() { if (size > 0) Push(PopRear()); }

  T * Remove(tRListIterator<T, RetainPolicy> & other) {
    if (&(other.list) != this) return NULL;
    return RemoveNode(other.node);
  }

  T * Insert(tRBaseIterator<T, RetainPolicy> & list_it, T * in_data) {
    Retain(in_data);
    tRListNode<T, RetainPolicy> * new_node = new tRListNode<T, RetainPolicy>;
    new_node->data = in_data;
    new_node->next = list_it.node->next;
    new_node->prev = list_it.node;

    list_it.node->next->prev = new_node;
    list_it.node->next = new_node;
    size++;
    return in_data;
  }


  bool Remove(T * other) {
    tRListNode<T, RetainPolicy> * test = root.next;
    while (test != &root) {
      if (test->data == other) {
	      RemoveNode(test);
	      return true;
      }
      test = test->next;
    }
    return false;
  }

  int GetSize() const { return size; }

  void Append(tRList<T, RetainPolicy> & other_list) {
    tRListIterator<T, RetainPolicy> other_it(other_list);
    while (other_it.Next() != NULL) PushRear(other_it.Get());
  }

  T * Find(T * _in) const {
    tRListNode<T, RetainPolicy> * test = root.next;
    while (test != &root) {
      if ( *(test->data) == *(_in) ) return test->data;
      test = test->next;
    }
    return NULL;
  }

  T * PopIntValue(int (T::*fun)() const, int value) {
    tRListNode<T, RetainPolicy> * test = root.next;
    while (test != &root) {
      if ( (test->data->*fun)() == value) return RemoveNode(test);
      test = test->next;
    }
    return NULL;
  }

  T * PopIntMax(int (T::*fun)() const) { 
    if (size == 0) return NULL;
    tRListNode<T, RetainPolicy> * test = root.next;
    tRListNode<T, RetainPolicy> * best = test;
    int max_val = (test->data->*fun)();
    while (test != &root) {
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
    if (size == 0) return NULL;
    tRListNode<T, RetainPolicy> * test = root.next;
    tRListNode<T, RetainPolicy> * best = test;
    double max_val = (test->data->*fun)();
    while (test != &root) {
      const double cur_val = (test->data->*fun)();
      if ( cur_val > max_val ) {
	      max_val = cur_val;
	      best = test;
      }
      test = test->next;
    }
    return RemoveNode(best);
  }
public:
  tRList() : size(0), it_count(0) { }
  ~tRList() { Clear(); }
private:
  tRList(tRList & _list) { ; }  // Never should be used...
};

////////////////////
//  tRListIterator

template <
  class T,
  template <class> class RetainPolicy
> tRListIterator<T, RetainPolicy>::tRListIterator(
  tRList<T, RetainPolicy> & _list)
  : list(_list), node(&(_list.root))
{
  list.AddIterator(this);
}

template <
  class T,
	template <class> class RetainPolicy
> tRListIterator<T, RetainPolicy>::~tRListIterator()
{
  list.RemoveIterator(this);
}

template <
  class T,
	template <class> class RetainPolicy
> void tRListIterator<T, RetainPolicy>::Reset()
{
  node = &(list.root);
}

template <
  class T,
	template <class> class RetainPolicy
> T * tRListIterator<T, RetainPolicy>::Get()
{
  return node->data;
}

template <
  class T,
	template <class> class RetainPolicy
> T * tRListIterator<T, RetainPolicy>::Next()
{
  node = node->next;
  return node->data;
}

template <
  class T,
	template <class> class RetainPolicy
> T * tRListIterator<T, RetainPolicy>::Prev()
{
  node = node->prev;
  return node->data;
}

template <
  class T,
	template <class> class RetainPolicy
> bool tRListIterator<T, RetainPolicy>::Find(T * test_data)
{
  for (node = list.root.next;
       node != &(list.root);
       node = node->next) {
    if (node->data == test_data) return true;
  }
  return false;
}

template <
  class T,
	template <class> class RetainPolicy
> bool tRListIterator<T, RetainPolicy>::AtRoot() const
{
  return (node == &(list.root));
}

template <
  class T,
	template <class> class RetainPolicy
> bool tRListIterator<T, RetainPolicy>::AtEnd() const
{
  return (node->next == &(list.root));
}

template <
  class T,
	template <class> class RetainPolicy
> T* tRListIterator<T, RetainPolicy>::Remove()
{
  return list.RemoveNode(node);
}

/////////////////////////
//  tConstRListIterator

template <
  class T,
	template <class> class RetainPolicy
> tConstRListIterator<T, RetainPolicy>::tConstRListIterator(
  const tRList<T, RetainPolicy> & _list)
  : list(_list), node(&(_list.root))
{
  list.AddIterator(this);
}

template <
  class T,
	template <class> class RetainPolicy
> tConstRListIterator<T, RetainPolicy>::~tConstRListIterator()
{
  list.RemoveIterator(this);
}

template <
  class T,
	template <class> class RetainPolicy
> void tConstRListIterator<T, RetainPolicy>::Reset()
{
  node = &(list.root);
}

template <
  class T,
	template <class> class RetainPolicy
> const T * tConstRListIterator<T, RetainPolicy>::Get()
{
  return node->data;
}

template <
  class T,
	template <class> class RetainPolicy
> const T * tConstRListIterator<T, RetainPolicy>::Next()
{
  node = node->next;
  return node->data;
}

template <
  class T,
	template <class> class RetainPolicy
> const T * tConstRListIterator<T, RetainPolicy>::Prev()
{
  node = node->prev;
  return node->data;
}

template <
  class T,
	template <class> class RetainPolicy
> bool tConstRListIterator<T, RetainPolicy>::Find(const T * test_data)
{
  for (node = list.root.next;
       node != &(list.root);
       node = node->next) {
    if (node->data == test_data) return true;
  }
  return false;
}

template <
  class T,
	template <class> class RetainPolicy
> bool tConstRListIterator<T, RetainPolicy>::AtRoot() const
{
  return (node == &(list.root));
}

template <
  class T,
	template <class> class RetainPolicy
> bool tConstRListIterator<T, RetainPolicy>::AtEnd() const
{
  return (node->next == &(list.root));
}

#endif
