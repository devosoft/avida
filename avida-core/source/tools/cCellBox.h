//
//  cCellBox.h
//  Avida-Core
//
//  Created by Diane Blackwood on 1/13/18.
//

#ifndef cCellBox_h
#define cCellBox_h

class cCellBox {

protected:
  int m_x;
  int m_y;
  int m_width;
  int m_height;
  
public:
  
  //Add in default contructor
  //We're adding it to make sure that nay requistie that is *not* a
  //cellBox will not fail . . . will allow the requisite to be met
  cCellBox()
  :   m_x(-1)
  ,  m_y(-1)
  ,  m_width(-1)
  ,  m_height(-1)
  {
  }
  
  cCellBox(const cCellBox& _in)
  {
    (*this) = _in;
  }
  
  cCellBox& operator=(const cCellBox& _in)
  {
    m_x = _in.m_x;
    m_y = _in.m_y;
    m_width = _in.m_width;
    m_height = _in.m_height;
    return *this;
  }
  
  cCellBox(int xx, int yy, int width=1, int height=1)
  : m_x(xx)
  , m_y(yy)
  , m_width(width)
  , m_height(height)
  {
    assert(m_x >= 0 && m_y >= 0 && m_width > 0 && m_height > 0);
  }
  
  bool GetX() const { return m_x; }
  bool GetY() const { return m_y; }
  bool GetWidth() const { return m_width; }
  bool GetHeight() const { return m_height; }
  
  bool InCellBox(int xx, int yy) const
  {
    if (!IsDefined())
      return true;
    return (m_x <= xx && xx < m_x + m_width) && (m_y <= yy && yy < m_y + m_height);
  }
  
  // method to see if cell is in box.
  bool InCellBox(int cellID, int world_x, int world_y) const 
  {
    if (!IsDefined())
      return true;
    int cell_x = cellID % world_x;
    int cell_y = cellID / world_x;
    return InCellBox(cell_x, cell_y);
  }
  
  bool IsDefined() const
  {
    return m_x >= 0 && m_y >= 0 && m_width > 0 && m_height > 0;
  }
};


class cOffsetCellBox : public cCellBox
{
  protected:
    int m_size_x;
    int m_size_y;
  
  public:
    cOffsetCellBox()
    : cCellBox(-1,-1,-1,-1)
    , m_size_x(-1)
    , m_size_y(-1)
    {
    }
    
    cOffsetCellBox(int size_x, int size_y, int x, int y, int width, int height)
    : cCellBox(x,y,width,height)
    , m_size_x(size_x)
    , m_size_y(size_y)
    {
      assert(size_x > 0 && size_y > 0);
    }
    
    cOffsetCellBox(const cOffsetCellBox& _in)
    {
      (*this) = _in;
    }
    
    cOffsetCellBox& operator=(const cOffsetCellBox& _in)
    {
      cCellBox::operator=(_in);
      m_size_x = _in.m_size_x;
      m_size_y = _in.m_size_y;
      return *this;
    }
    
    bool InCellBox(int xx, int yy) const
    {
      if (!IsDefined())
        return true;
      
      return (m_size_x + m_x <= xx && xx < m_size_x + m_x + m_width &&
              m_size_y + m_y <= yy && yy < m_size_y + m_y + m_height) ? 
              true : false;
    }
    
    bool InCellBox(int cell_id) const
    {
      int xx = cell_id % m_size_x;
      int yy = cell_id / m_size_x;
      return InCellBox(xx,yy);
    }
};


#endif /* cCellBox_h */
