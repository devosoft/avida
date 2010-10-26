
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

#ifndef cEntryHandle_h
#include "cEntryHandle.h"
#endif
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
  int m_segs;
  
  class cSparseVectorListIterator;
  class cSparseVectorListHandle;
  struct sListSegment
  {
    tSparseVectorList<T>* list;
    int used;
    T* entries[SEGMENT_SIZE];
    sListSegment* prev;
    sListSegment* next;
    cSparseVectorListHandle* handles[SEGMENT_SIZE];
    
    sListSegment() : used(0), next(NULL) { ; }
    sListSegment(tSparseVectorList<T>* in_list, T* value, sListSegment* in_next, sListSegment* in_prev = NULL)
    : list(in_list), used(1), prev(in_prev), next(in_next)
    {
      entries[0] = value; handles[0] = NULL;
    }
  };
  
  sListSegment* m_head_seg;
  sListSegment* m_tail_seg;
  tArray<cSparseVectorListIterator*> m_its;
  
  
public:
  tSparseVectorList() : m_size(0), m_segs(0), m_head_seg(NULL), m_tail_seg(NULL) { ; }
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
  
  
  void Push(T* value, cEntryHandle** handle = NULL)
  {
    if (handle) delete *handle;
    if (m_head_seg && m_head_seg->used < SEGMENT_SIZE) {
      int idx = m_head_seg->used;
      m_head_seg->used++;
      m_head_seg->entries[idx] = value;
      if (handle) {
        *handle = m_head_seg->handles[idx] = new cSparseVectorListHandle(m_head_seg, value);
      } else {
        m_head_seg->handles[idx] = NULL;
      }
    } else {
      m_head_seg = new sListSegment(this, value, m_head_seg);
      m_segs++;
      if (m_head_seg->next) {
        m_head_seg->next->prev = m_head_seg;
      } else {
        m_tail_seg = m_head_seg;
      }
      if (handle) *handle = m_head_seg->handles[0] = new cSparseVectorListHandle(m_head_seg, value);
    }
    
    m_size++;
  }
  
  void PushRear(T* value, cEntryHandle** handle = NULL)
  {
    if (handle) delete *handle;
    if (m_tail_seg && m_tail_seg->used < SEGMENT_SIZE) {
      for (int i = m_tail_seg->used; i > 0; i--) m_tail_seg->entries[i] = m_tail_seg->entries[i - 1];
      for (int i = m_tail_seg->used; i > 0; i--) m_tail_seg->handles[i] = m_tail_seg->handles[i - 1];
      m_tail_seg->entries[0] = value;
      m_tail_seg->used++;
      if (handle) {
        *handle = m_tail_seg->handles[0] = new cSparseVectorListHandle(m_tail_seg, value);
      } else {
        m_tail_seg->handles[0] = NULL;
      }      
    } else if (m_tail_seg && m_tail_seg->used == SEGMENT_SIZE) {
      m_tail_seg->next = new sListSegment(this, value, NULL, m_tail_seg);
      m_segs++;
      m_tail_seg = m_tail_seg->next;
      if (handle) *handle = m_tail_seg->handles[0] = new cSparseVectorListHandle(m_tail_seg, value);
    } else {
      m_tail_seg = new sListSegment(this, value, NULL, NULL);
      m_segs++;
      m_head_seg = m_tail_seg;
      if (handle) *handle = m_tail_seg->handles[0] = new cSparseVectorListHandle(m_tail_seg, value);
    }
    
    m_size++;
  }
  
  void Remove(const T* value)
  {
    sListSegment* cur = m_head_seg;
    while (cur) {
      if (removeFromSeg(value, cur)) return;
      cur = cur->next;
    }
    assert(false);
  }
  
  tIterator<T>* Iterator() { return new cSparseVectorListIterator(this); }
  
  int GetDataSize() const { return sizeof(T*) * m_size; }
  int GetMemSize() const { return sizeof(sListSegment) * m_segs; }
  
private:
  
  const T* removeFromSeg(const T* value, sListSegment* cur)
  {
    for (int entry_idx = 0; entry_idx < cur->used; entry_idx++) {
      if (cur->entries[entry_idx] == value) {
        cur->used--;
        if (cur->used == 0) {
          // Last entry in this segment, remove segment
          if (cur->prev) cur->prev->next = cur->next;
          if (cur->next) cur->next->prev = cur->prev;
          if (cur == m_head_seg) m_head_seg = cur->next;
          if (cur == m_tail_seg) m_tail_seg = cur->prev;
          
          // Adjust any iterators to remain consistent
          for (int it = 0; it < m_its.GetSize(); it++) {
            if (m_its[it]->m_cur == cur) {
              m_its[it]->m_cur = cur->prev;
              m_its[it]->m_pos = -1;
            }
          }
          m_segs--;
          if (cur->handles[0]) cur->handles[0]->m_seg = NULL;
          delete cur;
        } else if (cur->next && (cur->used + cur->next->used) <= SEGMENT_SIZE) {
          // Adjust any iterators to remain consistent
          for (int it_idx = 0; it_idx < m_its.GetSize(); it_idx++) {
            cSparseVectorListIterator* it = m_its[it_idx];
            if (it->m_cur == cur) {
              if (it->m_pos >= entry_idx) it->m_pos--;
              it->m_pos += cur->next->used;
              it->m_cur = cur->next;
            }
          }       
          
          // Pack the remaining entries in this segment onto the next
          for (int j = 0; j < entry_idx && j < cur->used; j++) cur->next->entries[cur->next->used + j] = cur->entries[j];
          for (int j = entry_idx; j < cur->used; j++) cur->next->entries[cur->next->used + j] = cur->entries[j + 1];
          
          // Move and update segment handles associated with moved entries
          for (int j = 0; j < entry_idx && j < cur->used; j++) {
            cSparseVectorListHandle* handle = cur->handles[j];
            cur->next->handles[cur->next->used + j] = handle;
            if (handle) handle->m_seg = cur->next;
          }
          if (cur->handles[entry_idx]) cur->handles[entry_idx]->m_seg = NULL;
          for (int j = entry_idx; j < cur->used; j++) {
            cSparseVectorListHandle* handle = cur->handles[j + 1];
            cur->next->handles[cur->next->used + j] = handle;
            if (handle) handle->m_seg = cur->next;
          }
          
          // Update used count on packed segment
          cur->next->used += cur->used;
          
          // Remove now empty segment
          if (cur->prev) cur->prev->next = cur->next;
          if (cur->next) cur->next->prev = cur->prev;
          if (cur == m_head_seg) m_head_seg = cur->next;
          m_segs--;
          delete cur;
        } else {
          // Adjust any iterators to remain consistent
          for (int it = 0; it < m_its.GetSize(); it++) {
            if (m_its[it]->m_cur == cur && m_its[it]->m_pos >= entry_idx) m_its[it]->m_pos--;
          }            
          
          // Shift remaining entries in this segment
          for (int j = entry_idx; j < cur->used; j++) cur->entries[j] = cur->entries[j + 1];
          if (cur->handles[entry_idx]) cur->handles[entry_idx]->m_seg = NULL;
          for (int j = entry_idx; j < cur->used; j++) cur->handles[j] = cur->handles[j + 1];
          assert((cur->entries[cur->used] = NULL) == NULL);
        }
        
        m_size--;
        return value;
      }
    }
    return NULL;
  }
  
  
  
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
      list->m_its.Push(this);
    }
    ~cSparseVectorListIterator()
    {
      tArray<cSparseVectorListIterator*>& its = m_list->m_its;
      for (int i = 0; i < its.GetSize(); i++) {
        if (its[i] == this) {
          if (i != (its.GetSize() - 1)) its[i] = its[its.GetSize() - 1];
          its.Resize(its.GetSize() - 1);
          break;
        }
      }
    }
    
    T* Get() {
      if (m_cur && m_pos >= 0 && m_pos < m_cur->used) {
        assert(m_cur->entries[m_pos]);
        return m_cur->entries[m_pos];
      }
      return NULL;
    }
    
    T* Next() {
      if (m_cur && m_pos > 0) {
        m_pos--;
        assert(m_cur->entries[m_pos]);
        return m_cur->entries[m_pos];
      }
      if (m_cur && m_pos <= 0) {
        m_cur = m_cur->next;
        if (m_cur) {
          m_pos = m_cur->used - 1;
          assert(m_cur->entries[m_pos]);
          return m_cur->entries[m_pos];
        }
      }
      return NULL;
    }
  };
  
  
  class cSparseVectorListHandle : public cEntryHandle
  {
    friend class tSparseVectorList<T>;
  private:
    sListSegment* m_seg;
    T* m_entry;
    
    cSparseVectorListHandle(); // @not_implemented
    cSparseVectorListHandle(const cSparseVectorListHandle&); // @not_implemented
    cSparseVectorListHandle& operator=(const cSparseVectorListHandle&); // @not_implemented
    
    
    cSparseVectorListHandle(sListSegment* seg, T* entry) : m_seg(seg), m_entry(entry) { ; }
    
  public:
    bool IsValid() const { return (m_seg); }
    void Remove()
    {
      if (!m_seg) return;
      m_seg->list->removeFromSeg(m_entry, m_seg);
      m_seg = NULL;
    }
  };
};

#undef SEGMENT_SIZE

#endif
