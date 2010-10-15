/*
 *  tBufferedList.h
 *  Avida
 *
 *  Created by David on 12/17/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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

#ifndef tBufferedList_h
#define tBufferedList_h

#ifndef platform_h
#include "Platform.h"
#endif
#ifndef tManagedPointerArray_h
#include "tManagedPointerArray.h"
#endif

#ifndef NULL
#define NULL 0
#endif


template <class T> class tBufferedList
{
protected:
  typedef union {
    unsigned int index;
    struct {
#if AVIDA_PLATFORM(LITTLE_ENDIAN)
      unsigned int offset:6;
      unsigned int num:26;
#else
      unsigned int num:26;
      unsigned int offset:6;
#endif
    } buffer;
  } ListIndex;

  class cNode
  {
  public:
    T* data;
    cNode* next;
    cNode* prev;
  };
  
  class cBuf
  {
  private:
    cNode* m_nodes;
    
  public:
    cBuf() { m_nodes = new cNode[64]; }
    ~cBuf() { delete [] m_nodes; }
    
    cNode& operator[](int idx) { return m_nodes[idx]; }
    const cNode& operator[](int idx) const { return m_nodes[idx]; }
  };
  
  
  tManagedPointerArray<cBuf> m_bufs;
  ListIndex m_next;
  cNode m_root;
  int m_size;
  

public:
  tBufferedList() : m_bufs(1), m_size(0) { m_next.index = 0; m_root.next = &m_root; m_root.prev = &m_root; }
  explicit tBufferedList(const tBufferedList& in_list) : m_size(0) { Clear(); Append(in_list); }
  ~tBufferedList() { Clear(); }
  
  
  inline int GetSize() const { return m_size; }
    
  inline void Clear()
  {
    m_bufs.Resize(1);
    m_next.index = 0;
    m_root.next = &m_root;
    m_root.prev = &m_root;
    m_size = 0;
  }
  
  
  inline const T* GetFirst() const { return m_root.next->data; }
  inline const T* GetLast()  const { return m_root.prev->data; }
  inline T* GetFirst()             { return m_root.next->data; }
  inline T* GetLast()              { return m_root.prev->data; }
  
  T* GetPos(int pos)
  {
    if (pos >= m_size) return NULL;
    cNode* test_node = m_root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }
  
  const T* GetPos(int pos) const
  {
    if (pos >= GetSize()) return NULL;
    cNode* test_node = m_root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }
  
  
  inline T* Pop() { return removeNode(m_root.next); }
  inline T* PopRear() { return removeNode(m_root.prev); }
  inline T* PopPos(int pos)
  {
    if (pos >= m_size) return NULL;
    cNode* test_node = m_root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return removeNode(test_node);
  }
  
  
  void Push(T* val)
  {
    cNode& node = m_bufs[m_next.buffer.num][m_next.buffer.offset];
    node.data = val;
    node.next = m_root.next;
    node.prev = &m_root;
    m_root.next->prev = &node;
    m_root.next = &node;
    incSize();
  }
  
  void PushRear(T* val)
  {
    cNode& node = m_bufs[m_next.buffer.num][m_next.buffer.offset];
    node.data = val;
    node.next = &m_root;
    node.prev = m_root.prev;
    m_root.prev->next = &node;
    m_root.prev = &node;
    incSize();
  }
  
  
  inline void CircNext() { if (m_size > 0) PushRear(Pop()); }
  inline void CircPrev() { if (m_size > 0) Push(PopRear()); }
  
  
  T* Remove(T* ptr)
  {
		if (m_size == 0) return NULL;
    ListIndex i;
    for (i.index = 0; i.index < m_size; i.index++) {
      if (m_bufs[i.buffer.num][i.buffer.offset].data == ptr) return removeNode(&m_bufs[i.buffer.num][i.buffer.offset]);
    }
    return NULL;
  }
  
  
  // Find by Pointer
  T* FindPtr(T* ptr) const
  {
		if (m_size == 0) return NULL;
    ListIndex i;
    for (i.index = 0; i.index < m_size; i.index++) {
      if (m_bufs[i.buffer.num][i.buffer.offset].data == ptr) return ptr;
    }
    return NULL;
  }

  // Find by Value
  T* Find(T* val) const
  {
		if (m_size == 0) return NULL;
    ListIndex i;
    for (i.index = 0; i.index < m_size; i.index++) {
      if (*m_bufs[i.buffer.num][i.buffer.offset].data == *val) return m_bufs[i.buffer.num][i.buffer.offset].data;
    }
    return NULL;
  }
  
  
  void Append(const tBufferedList<T>& in_list)
  {
    cNode* cur_node = in_list.root.next;
    while (cur_node != &(in_list.root)) {
      PushRear(cur_node->data);
      cur_node = cur_node->next;
    }
  }
  
  inline void Copy(const tBufferedList<T>& in_list) { Clear(); Append(in_list); }
  inline tBufferedList& operator=(const tBufferedList& list) { Copy(list); return *this; }


protected:
  T* removeNode(cNode* out_node)
  {
    // Make sure we're not trying to delete the root node!
    if (out_node == &m_root) return NULL;
    
    // Save the data and patch up the linked list.
    T* out_data = out_node->data;
    out_node->prev->next = out_node->next;
    out_node->next->prev = out_node->prev;
    
    m_next.index--;
    cNode* next_node = &m_bufs[m_next.buffer.num][m_next.buffer.offset];
    if (next_node != out_node) {
      *out_node = *next_node;
      out_node->next->prev = out_node;
      out_node->prev->next = out_node;
    }
    m_size--;
    
    if (m_bufs.GetSize() > (m_next.buffer.num + 1) && m_next.buffer.offset < 61) m_bufs.Resize(m_bufs.GetSize() - 1);
    
    return out_data;
  }
  
  
  void incSize()
  {
    m_next.index++;
    if (m_bufs.GetSize() <= m_next.buffer.num) m_bufs.Resize(m_bufs.GetSize() + 1);
    m_size++;
  }
  
};

#endif
