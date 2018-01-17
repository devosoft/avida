//
//  cOffsetLinearGrid.h
//  Avida-Core
//
//  Created by Matthew Rupp on 1/15/18.
//

#ifndef cOffsetLinearGrid_h
#define cOffsetLinearGrid_h

#include "cCellBox.h"

template<class T>
class cOffsetLinearGrid
{ 
  protected:
    int m_size_x;
    int m_size_y;
    cCellBox m_cellbox;
    T m_default_value;
    Apto::Array<T> m_array;
    
    
  
  public:
    cOffsetLinearGrid(int size_x, int size_y, const cCellBox& cellbox,
        const T& default_value)
    : m_size_x(size_x)
    , m_size_y(size_y)
    , m_cellbox(cellbox)
    , m_default_value(default_value)
    {
      assert(!cellbox.IsDefined() || \
        (cellbox.IsDefined() && size_x < cellbox.GetX() + cellbox.GetWidth()));
      
      assert(!cellbox.IsDefined() || \
        (cellbox.IsDefined() && size_y < cellbox.GetY() + cellbox.GetHeight()));
      
      m_array.ResizeClear(cellbox.GetWidth(), cellbox.GetHeight());
      m_array.SetAll(m_default_value);
    }
    
    cOffsetLinearGrid(const cOffsetLinearGrid<T>& _in)
    {
      *this = _in;
    }
    
    cOffsetLinearGrid& operator=(cOffsetLinearGrid<T> _in)
    {
      m_array = _in.m_array;
      m_cellbox = _in.m_cellbox;
      m_size_x = _in.m_size_x;
      m_size_y = _in.m_size_y;
      return *this;
    }
    
    
    void Clear()
    {
      m_array.SetAll(m_default_value);
    }
    
    inline T& operator[](int i)
    {
      int y = (i / m_size_x) - m_cellbox.GetY();
      int x = (i % m_size_x) - m_cellbox.GetX();
      return (*this)(x,y);
    }
    
    inline T& operator()(int i)
    {
      return (*this)[i];
    }
    
    inline T& operator()(int x, int y)
    {
      if ( y < 0 || y >= m_size_y || x < 0 || x >= m_size_x)
        return m_default_value;
        
      int j = y * m_size_x + x;
      return m_array[j];
    }
    
    inline T operator[](int i) const
    {
      int y = (i / m_size_x) - m_cellbox.GetY();
      int x = (i % m_size_x) - m_cellbox.GetX();
      return (*this)(x,y);
    }
    
    inline T operator()(int i) const
    {
      return (*this)[i];
    }
    
    inline T operator()(int x, int y) const
    {
      if ( y < 0 || y >= m_size_y || x < 0 || x >= m_size_x)
        return m_default_value;
        
      int j = y * m_size_x + x;
      return m_array[j];
    }
    
    inline int GetSize() const
    {
      return m_array.GetSize();
    }
    
    inline int GetSizeX() const
    {
      return m_size_x;
    }
    
    inline int GetSizeY() const
    {
      return m_size_y;
    }
};
#endif /* cOffsetLinearGrid_h */
