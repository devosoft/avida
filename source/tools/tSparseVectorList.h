
/*
 *  tSparseVectorList.h
 *  Avida
 *
 *  Created by David on 9/10/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef tSparseVectorList_h
#define tSparseVectorList_h

#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tIterator_h
#include "tIterator.h"
#endif

#define SEGMENT_SIZE 16

template <class T> class tSparseVectorList
{
private:
  int m_size;
  
  class cSparseVectorListIterator;
  struct sListSegment
  {
    int used;
    T* entries[SEGMENT_SIZE];
    sListSegment* next;
    
    sListSegment() : used(0), next(NULL) { ; }
    sListSegment(T* value, sListSegment* in_next) : used(1), next(in_next) { entries[0] = value; }
  };
  
  sListSegment* m_head_seg;
  sListSegment* m_tail_seg;
  tArray<cSparseVectorListIterator*> m_its;
  
  
public:
  tSparseVectorList() : m_size(0), m_head_seg(NULL), m_tail_seg(NULL) { ; }
  explicit tSparseVectorList(const tSparseVectorList& list);
  ~tSparseVectorList()
  {
    sListSegment* next = m_head_seg;
    while (next) {
      sListSegment* cur = next;
      next = cur->next;
      delete cur;
    }
  }
  
  int GetSize() const { return m_size; }
  
  T* GetFirst() { return (m_head_seg && m_head_seg->used) ? m_head_seg->entries[m_head_seg->used - 1] : NULL; }
    
  void Push(T* value)
  {
    if (m_head_seg && m_head_seg->used < SEGMENT_SIZE) {
      m_head_seg->entries[m_head_seg->used++] = value;
    } else {
      m_head_seg = new sListSegment(value, m_head_seg);
      if (!(m_head_seg->next)) m_tail_seg = m_head_seg;
    }
    
    m_size++;
  }
  
  void PushRear(T* value)
  {
    if (m_tail_seg && m_tail_seg->used < SEGMENT_SIZE) {
      for (int i = m_tail_seg->used; i > 0; i--) m_tail_seg->entries[i] = m_tail_seg->entries[i - 1];
      m_tail_seg->entries[0] = value;
      m_tail_seg->used++;
    } else if (m_tail_seg && m_tail_seg->used == SEGMENT_SIZE) {
      m_tail_seg->next = new sListSegment(value, NULL);
      m_tail_seg = m_tail_seg->next;
    } else {
      m_tail_seg = new sListSegment(value, NULL);
      m_head_seg = m_tail_seg;
    }
    
    m_size++;
  }
  
  void Remove(const T* value)
  {
    sListSegment* prev = NULL;
    sListSegment* cur = m_head_seg;
    while (cur) {
      for (int i = 0; i < cur->used; i++) {
        if (cur->entries[i] == value) {
          cur->used--;
          if (cur->used == 0) {
            // Last entry in this segment, remove segment
            if (prev) prev->next = cur->next;
            if (cur == m_head_seg) m_head_seg = cur->next;
            if (cur == m_tail_seg) m_tail_seg = prev;
            
            // Adjust any iterators to remain consistent
            for (int it = 0; it < m_its.GetSize(); it++) {
              if (m_its[it]->m_cur == cur) {
                m_its[it]->m_cur = prev;
                m_its[it]->m_pos = -1;
              }
            }
            delete cur;
          } else {
            // Adjust any iterators to remain consistent
            for (int it = 0; it < m_its.GetSize(); it++) {
              if (m_its[it]->m_cur == cur && m_its[it]->m_pos >= i) m_its[it]->m_pos--;
            }            
            
            // Shift remaining entries in this segment
            for (; i < cur->used; i++) cur->entries[i] = cur->entries[i + 1];
          }
          
          m_size--;
          return;
        }
      }
     
      prev = cur;
      cur = cur->next;
    }
  }
  
  tIterator<T>* Iterator() { return new cSparseVectorListIterator(this); }
  
private:
  class cSparseVectorListIterator : public tIterator<T>
  {
    friend class tSparseVectorList<T>;
  private:
    tSparseVectorList<T>* m_list;
    sListSegment* m_cur;
    int m_pos;
    
    cSparseVectorListIterator(); // @not_implemented
    cSparseVectorListIterator(const cSparseVectorListIterator&); // @not_implemented
    cSparseVectorListIterator& operator=(const cSparseVectorListIterator&); // @not_implemented
    
    
  public:
    cSparseVectorListIterator(tSparseVectorList<T>* list)
    : m_list(list), m_cur(list->m_head_seg), m_pos((list->m_head_seg) ? list->m_head_seg->used : -1)
    {
      m_list->m_its.Push(this);
    }
    ~cSparseVectorListIterator()
    {
      for (int i = 0; i < m_list->m_its.GetSize(); i++) {
        if (m_list->m_its[i] == this) {
          if (i != (m_list->m_its.GetSize() - 1)) {
            m_list->m_its[i] = m_list->m_its[m_list->m_its.GetSize() - 1];
          }
          m_list->m_its.Resize(m_list->m_its.GetSize() - 1);
        }
      }
    }
    
    T* Get() {
      if (m_cur && m_pos >= 0 && m_pos < m_cur->used) return m_cur->entries[m_pos];
      return NULL;
    }
    
    T* Next() {
      if (m_cur && m_pos > 0) {
        m_pos--;
        return m_cur->entries[m_pos];
      }
      if (m_cur && m_pos <= 0) {
        m_cur = m_cur->next;
        if (m_cur) {
          m_pos = m_cur->used - 1;
          return m_cur->entries[m_pos];
        }
      }
      return NULL;
    }
  };
  
  
};

#undef SEGMENT_SIZE

#endif
