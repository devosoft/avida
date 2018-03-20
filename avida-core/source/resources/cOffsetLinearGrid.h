//
//  cOffsetLinearGrid.h
//  Avida-Core
//
//  Created by Matthew Rupp on 1/15/18.
//

#ifndef cOffsetLinearGrid_h
#define cOffsetLinearGrid_h

#include "cCellBox.h"

/*
  An OffsetLinearGrid stores information that may be a subset of an entire grid.
  
  From the outside-user, an offset linear grid appears to occupy an entire grid,
  however data may be stored for a particular subregion only, with a default value
  being returned for data that exists outside of that subregion.
*/

template<class T>
class cOffsetLinearGrid
{ 
  protected:
    cOffsetCellBox m_cellbox;  //Describes where in the entire grid data is collected
    T m_default_value;         //The default value to return when data is requested outside of 
                               //the cell box
    Apto::Array<T> m_array;    //The array storing the data in a linearized manner
    
    int LinearNdx(int x, int y);  // Transform global x and y into a linear lookup index
    
      
  public:
    cOffsetLinearGrid(const cOffsetCellBox& cbox, const T& default_value)
    : m_cellbox(cbox)
    , m_default_value(default_value)
    {
      assert(m_cellbox.IsDefined());
      m_array.ResizeClear(std::abs(cbox.GetWidth()) * std::abs(cbox.GetHeight()));
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
      return *this;
    }
    
    
    void Clear()
    {
      m_array.SetAll(m_default_value);
    }
    
    /*
      The index i in this case refers to a cellID in the global sense.
    */
    inline T& operator[](int i)
    {
      int y =  i / m_cellbox.GetSizeX();
      int x =  i % m_cellbox.GetSizeY();
      return (*this)(x,y);
    }
    
    inline T& operator()(int i)
    {
      return (*this)[i];
    }
    
    /*
      The x and y-coordinates here are relative to the gobal grid
    */
    inline T& operator()(int x, int y)
    {
      if (!m_cellbox.InCellBox(x,y))
        return m_default_value;
      
      int ndx = LinearNdx(x,y);
      assert(ndx > 0 && ndx < m_array.GetSize());
      return m_array[ndx];
    }
    
    
    inline T operator[](int i) const
    {
      int y =  i / m_cellbox.GetSizeX();
      int x =  i % m_cellbox.GetSizeY();
      return (*this)(x,y);
    }
    
    inline T operator()(int i) const
    {
      return (*this)[i];
    }
    
    inline T operator()(int x, int y) const
    {
      if (!m_cellbox.InCellBox(x,y))
        return m_default_value;
        
      int ndx = LinearNdx(x,y);
      assert(ndx > 0 && ndx < m_array.GetSize());
      return m_array[ndx];
    }
    
    
    cOffsetCellBox GetOCellBox() const 
    {
      return m_cellbox;
    }
    
    
    inline int LinearNdx(int x, int y) const
    {
      if (!m_cellbox.InCellBox(x,y)){
        return -1;
      } else{
        int x_offset = (x - m_cellbox.GetX()) % m_cellbox.GetSizeX();
        int y_offset = (y - m_cellbox.GetY()) % m_cellbox.GetSizeY();
        int ndx = y_offset * std::abs(m_cellbox.GetWidth()) + x_offset;
        return ndx;
      }
    }
    
    inline int GetHeight() const
    {
      return m_cellbox.GetHeight();
    }
    
    inline int GetWidth() const
    {
      return m_cellbox.GetWidth();
    }
    
    
    inline int GetSize() const
    {
      return m_cellbox.GetWidth() * m_cellbox.GetHeight();
    }
};
#endif /* cOffsetLinearGrid_h */
