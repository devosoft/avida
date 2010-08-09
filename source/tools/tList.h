/*
 *  tList.h
 *  Avida
 *
 *  Called "tList.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef tList_h
#define tList_h

#ifndef NULL
#define NULL 0
#endif

template<class T> class tList;
template<class T> class tBaseIterator;
template<class T> class tListIterator;
template<class T> class tConstListIterator;
template<class T> class tLWConstListIterator;


template <class T> class tListNode
{
public:
  T* data;
  tListNode<T>* next;
  tListNode<T>* prev;
  
  // @DMB - Visual Studio doesn't like usage of 'this' in initializers 
  //        and throws a lot of useless warnings. 
  tListNode() : data(NULL) { next = this; prev = this; } 
};


template <class T> class tList
{
  friend class tBaseIterator<T>;
  friend class tListIterator<T>;
  friend class tConstListIterator<T>;
  friend class tLWConstListIterator<T>;

protected:
  tListNode<T> root;                     // Data root
  int size;
  mutable tListNode<tBaseIterator<T> > it_root; // Iterator root
  mutable int it_count;
  
  T* RemoveNode(tListNode<T>* out_node)
  {
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
    T* out_data = out_node->data;
    out_node->prev->next = out_node->next;
    out_node->next->prev = out_node->prev;
    
    // Cleanup and return
    size--;
    delete out_node;
    return out_data;
  }
  
  // To be called from iterator constructor only!
  void AddIterator(tBaseIterator<T>* new_it) const
  {
    tListNode< tBaseIterator<T> >* new_node =
    new tListNode< tBaseIterator<T> >;
    new_node->data = new_it;
    new_node->next = it_root.next;
    new_node->prev = &it_root;
    it_root.next->prev = new_node;
    it_root.next = new_node;
    it_count++;
  }
  
  // To be called from iterator destructor only!
  void RemoveIterator(tBaseIterator<T>* old_it) const
  {
    tListNode< tBaseIterator<T> >* test_it = it_root.next;
    while (test_it->data != old_it) test_it = test_it->next;
    test_it->prev->next = test_it->next;
    test_it->next->prev = test_it->prev;
    delete test_it;
    it_count--;
  }
  

public:
  tList() : size(0), it_count(0) { }
  explicit tList(const tList& in_list) : size(0), it_count(0) { Append(in_list); }
  ~tList() { Clear(); }

  inline T* Pop() { return RemoveNode(root.next); }
  inline T* PopRear() { return RemoveNode(root.prev); }
  
  void Clear() { while (size > 0) Pop(); }
  
  void Append(const tList<T>& in_list)
  {
    tListNode<T>* cur_node = in_list.root.next;
    while (cur_node != &(in_list.root)) {
      PushRear(cur_node->data);
      cur_node = cur_node->next;
    }
  }
  
  void Copy(const tList<T> & in_list) {
    Clear();
    Append(in_list);
  }
  
  inline tList& operator=(const tList& list) { Copy(list); return *this; }

  void Push(T* _in) {
    tListNode<T>* new_node = new tListNode<T>;
    new_node->data = _in;
    new_node->next = root.next;
    new_node->prev = &root;
    root.next->prev = new_node;
    root.next = new_node;
    size++;
  }
  
  tListNode<T>* PushRear(T* _in) {
    tListNode<T>* new_node = new tListNode<T>;
    new_node->data = _in;
    new_node->next = &root;
    new_node->prev = root.prev;
    root.prev->next = new_node;
    root.prev = new_node;
    size++;
	return new_node;
  }
  
  inline const T* GetFirst() const { return root.next->data; }
  inline const T* GetLast()  const { return root.prev->data; }
  inline T* GetFirst()             { return root.next->data; }
  inline T* GetLast()              { return root.prev->data; }
  
  T* GetPos(int pos)
  {
    if (pos >= GetSize()) return NULL;
    tListNode<T>* test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }
  
  const T* GetPos(int pos) const
  {
    if (pos >= GetSize()) return NULL;
    tListNode<T>* test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }
  
  inline void CircNext() { if (size > 0) PushRear(Pop()); }
  inline void CircPrev() { if (size > 0) Push(PopRear()); }
  
  T* Insert(tListIterator<T>& list_it, T* in_data)
  {
    tListNode<T>* cur_node = list_it.node;
    
    // Build the new node for the list...
    tListNode<T>* new_node = new tListNode<T>;
    new_node->data = in_data;
    
    // Insert the new node before the iterator...
    new_node->next = cur_node;
    new_node->prev = cur_node->prev;
    cur_node->prev->next = new_node;
    cur_node->prev = new_node;
    size++;
    
    return in_data;
  }
  
  
  T* Remove(tListIterator<T>& other)
  {
    if (&(other.list) != this) return NULL; // @CAO make this an assert?
    return RemoveNode(other.node);
  }
  

  T* Remove(T* other)
  {
    tListNode<T>* test = root.next;
    while (test != &root) {
      if (test->data == other) {
        RemoveNode(test);
        return other;
      }
      test = test->next;
    }
    
    return NULL;
  }
  
  inline int GetSize() const { return size; }
  
  
  // Empty out another list, transferring its contents to the end of this one.
  void Transfer(tList<T>& other_list)
  {
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
  T* Find(T* _in) const
  {
		if (size == 0) return NULL;
    tListNode<T>* test = root.next;
    while (test != &root) {
      if ( *(test->data) == *(_in) ) return test->data;
      test = test->next;
    }
    return NULL;
  }
  
  // Find by Pointer
  T* FindPtr(T* _in) const
  {
    tListNode<T>* test = root.next;
    while (test != &root) {
      if ( test->data == _in ) return test->data;
      test = test->next;
    }
    return NULL;
  }
  
  // Find the position of the node by its pointer 
  int FindPosPtr(T* _in) const
  {
    tListNode<T>* test = root.next;
    int pos = 0;	
    while (test != &root) {
      if ( test->data == _in ) return pos;
      test = test->next;
      pos++;
    }
    return 0;
  }
  
  
  // Remove by position
  T* PopPos(int pos)
  {
    if (pos >= GetSize()) return NULL;
    tListNode<T>* test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return RemoveNode(test_node);
  }
  
};



// This is an extended version of tList that contains extra functions to
// allow method pointers associated with the object type being listed.
template <class T> class tListPlus : public tList<T>
{
public:
  tListPlus() : tList<T>() { ; }
  explicit tListPlus(const tList<T>& in_list) : tList<T>(in_list) { ; }
  explicit tListPlus(const tListPlus& in_list) : tList<T>(in_list) { ; }
  
  
  
  template<typename V> T* FindValue(V (T::*fun)() const, V value) const
  {
    tListNode<T>* node;
    if (FindNode(fun, value, node)) return node->data;
    return NULL;
  }
  
  template<typename V> T* PopValue(V (T::*fun)() const, V value)
  {
    tListNode<T>* node;
    if (FindNode(fun, value, node)) return RemoveNode(node);
    return NULL;
  }
  
  template<typename V> T* FindMax(V (T::*fun)() const) const
  {
    tListNode<T>* node;
    if (FindMax(fun, node)) return node->data;
    return NULL;
  }
  
  template<typename V> T* PopMax(V (T::*fun)() const)
  {
    tListNode<T>* node;
    if (FindMax(fun, node)) return RemoveNode(node);
    return NULL;
  }
  
  
  // Find by summing values until a specified total is reached.  
  T* FindSummedValue(int sum, int (T::*fun)() const) const
  {
    if (this->size == 0) return NULL;
    
    int total = 0;
    tListNode<T>* test = this->root.next;
    while (test != &(this->root) && total < sum) {
      total += (test->data->*fun)();
      test = test->next;
    }
    return test->data;
  }
  
  
  int Count(int (T::*fun)() const) const
  {
    int total = 0;
    tListNode<T> * test = this->root.next;
    while (test != &(this->root)) {
      total += (test->data->*fun)();
      test = test->next;
    }
    return total;
  }
  
  
  
private:  
  template<typename V> bool FindNode(V (T::*fun)() const, V value, tListNode<T>*& node) const
  {
    node = this->root.next;
    while (node != &(this->root)) {
      if ((node->data->*fun)() == value) return true;
      node = node->next;
    }
    
    return false;
  }
  

  template<typename V> bool FindMax(V (T::*fun)() const, tListNode<T>*& best) const
  {
    if (this->size == 0) return false;
    
    tListNode<T>* test = this->root.next;
    best = test;
    V max_val = (test->data->*fun)();
    while (test != &(this->root)) {
      const V cur_val = (test->data->*fun)();
      if (cur_val > max_val) {
        max_val = cur_val;
        best = test;
      }
      test = test->next;
    }
    
    return true;
  }
};









template <class T> class tBaseIterator
{
  friend class tList<T>;
  
protected:
  virtual const tList<T>& GetConstList() = 0;
  virtual const tListNode<T>* GetConstNode() = 0;
  
public:
  tBaseIterator() { ; }
  virtual ~tBaseIterator() { ; }
  
  virtual void Set(tListNode<T>* in_node) = 0;
  virtual void Reset() = 0;
  
  virtual const T* GetConst() = 0;
  virtual const T* NextConst() = 0;
  virtual const T* PrevConst() = 0;
  
  virtual bool AtRoot() const = 0;
  virtual bool AtEnd() const = 0;
};



template <class T> class tListIterator : public tBaseIterator<T>
{
  friend class tList<T>;
  
private:
  tList<T>& list;
  tListNode<T>* node;
  
  const tList<T>& GetConstList() { return list; }
  const tListNode<T>* GetConstNode() { return node; }
  
public:
  explicit inline tListIterator(tList<T>& _list) : list(_list), node(&(_list.root)) { list.AddIterator(this); }
  explicit inline tListIterator(tList<T>& _list, tListNode<T>* start) : list(_list), node(start) { list.AddIterator(this); }
  inline tListIterator(const tListIterator<T>& _tli) : tBaseIterator<T>(), list(_tli.list), node(_tli.node)
  {
    list.AddIterator(this);
  }
  
  inline ~tListIterator() { list.RemoveIterator(this); }
  
  void Set(tListNode<T>* in_node) { node = in_node; }
  void Reset() { node = &(list.root); }
  tListNode<T>* GetPos() { return node; }
  
  T* Get() { return node->data; }
  T* Next() { node = node->next; return node->data; }
  T* Prev() { node = node->prev; return node->data; }
  
  const T* GetConst() { return Get(); }
  const T* NextConst() { return Next(); }
  const T* PrevConst() { return Prev(); }
  
  bool Find(T* test_data);
  
  bool AtRoot() const { return (node == &(list.root)); }
  bool AtEnd() const { return (node->next == &(list.root)); }
  
  // Unique methods...
  T* Remove() { return list.RemoveNode(node); }
};


template <class T> class tConstListIterator : public tBaseIterator<T>
{
  friend class tList<T>;
  
private:
  const tList<T>& list;
  const tListNode<T>* node;
  
  const tList<T>& GetConstList() { return list; }
  const tListNode<T>* GetConstNode() { return node; }
  
public:
  explicit tConstListIterator(const tList<T>& _list) : list(_list), node(&(_list.root)) { list.AddIterator(this); }
  explicit tConstListIterator(const tList<T>& _list, const tListNode<T>* start_node)
    : list(_list), node(start_node) { list.AddIterator(this); }
  ~tConstListIterator() { list.RemoveIterator(this); }
  
  void Set(tListNode<T>* in_node) { node = in_node; }
  void Reset() { node = &(list.root); }
  
  T* Get() { return node->data; }
  T* Next() { node = node->next; return node->data; }
  T* Prev() { node = node->prev; return node->data; }
  
  const T* GetConst() { return Get(); }
  const T* NextConst() { return Next(); }
  const T* PrevConst() { return Prev(); }
  bool Find(const T * test_data);
  
  bool AtRoot() const { return (node == &(list.root)); }
  bool AtEnd() const { return (node->next == &(list.root)); }
};


template <class T> class tLWConstListIterator : public tBaseIterator<T>
{
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
  void Reset() { node = &(list.root); }
  
  T* Get() { return node->data; }
  T* Next() { node = node->next; return node->data; }
  T* Prev() { node = node->prev; return node->data; }
  
  const T* GetConst() { return Get(); }
  const T* NextConst() { return Next(); }
  const T* PrevConst() { return Prev(); }
  bool Find(const T* test_data);
  
  bool AtRoot() const { return (node == &(list.root)); }
  bool AtEnd() const { return (node->next == &(list.root)); }
};




template <class T> bool tListIterator<T>::Find(T* test_data)
{
  for (node = list.root.next; node != &(list.root); node = node->next) if (node->data == test_data) return true;
  return false;
}



template <class T> bool tConstListIterator<T>::Find(const T* test_data)
{
  for (node = list.root.next; node != &(list.root); node = node->next) if (node->data == test_data) return true;
  return false;
}



template <class T> bool tLWConstListIterator<T>::Find(const T* test_data)
{
  for (node = list.root.next; node != &(list.root); node = node->next) if (node->data == test_data) return true;
  return false;
}

#endif
