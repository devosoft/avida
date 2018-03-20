//
//  cCellBox.h
//  Avida-Core
//
//  Created by Diane Blackwood on 1/13/18.
//

#ifndef cCellBox_h
#define cCellBox_h

/*
 cCellBox simply defines a box located at coordinates (x,y) with a 
 non-zero absolute width and height if it is defined and allows a
 check to see if an x or y coordinate or array-offset mapped with
 total grid x and y-size falls within the CellBox when provided with
 a grid x and y total size by a user.
 */

class cCellBox {
  
protected:
  int m_x;   //X-coordinate
  int m_y;   //Y-corrdinate
  int m_width;  //Box width (absolute value > 0 if defined)
  int m_height; //Box height (absolute value > 0 if defined)
  
  
  
public:
  
  
  // The default constructor defines an "undefined" CellBox that always
  // returns true for all coordinates checked for membership.
  cCellBox()
  :  m_x(-1)    
  ,  m_y(-1)
  ,  m_width(0)
  ,  m_height(0)
  {
  }
  
  // Copy constructor
  cCellBox(const cCellBox& _in)
  {
    (*this) = _in;
  }
  
  // Assignment operator
  cCellBox& operator=(const cCellBox& _in)
  {
    m_x = _in.m_x;
    m_y = _in.m_y;
    m_width = _in.m_width;
    m_height = _in.m_height;
    return *this;
  }
  
  // Parameterized constructor
  // Because the full size of the grid in which
  // this cell box is created is not known, it is
  // necessary to assume by the user that the xx and yy
  // coordinates will be the lowest values for their
  cCellBox(int xx, int yy, int width=1, int height=1)
  : m_x(xx)
  , m_y(yy)
  , m_width(width)
  , m_height(height)
  {
    assert(m_x >= 0 && m_y >= 0 && std::abs(m_width) > 0 && std::abs(m_height) > 0);
  }
  
  // If the cell box is *not* defined, then all coordinates sent to InCellBox 
  // will return true.  See above note about reaction requisites.
  bool IsDefined() const
  {
    return m_x >= 0 && m_y >= 0 && std::abs(m_width) > 0 && std::abs(m_height) > 0;
  }
  
  inline int GetX() const { return m_x; }
  inline int GetY() const { return m_y; }
  inline int GetWidth() const { return m_width; }
  inline int GetHeight() const { return m_height; }
  
  
  // method to see if cell is in box.
  bool InCellBox(int cellID, int world_x, int world_y) const 
  {
    if (!IsDefined())  //Undefined boxes always reutrn true
      return true;
      
    int cell_x = cellID % world_x;
    int cell_y = cellID / world_x;
    return InCellBox(cell_x, cell_y, world_x, world_y);
  }
  
  bool InCellBox(int xx, int yy, int world_x, int world_y) const
  {
    if (!IsDefined())  //Undefined boxes always return true
      return true;
    
    // Transform coordinates and width/height such that widths
    // and heights must be positive and the same range is
    // maintained.
    int x_l = (m_width > 0) ? m_x : (m_x + m_width) % world_x;    //Reset x,y coordinates if
    int y_l = (m_height > 0) ? m_y : (m_y + m_height) % world_y;  //width or height are negative
    int w_l = std::abs(m_width);
    int h_l = std::abs(m_height);
    int x_p = (xx-x_l) % world_x;  //Translate our x-coordinate
    int y_p = (yy-y_l) % world_y;  //Translate our y-coordinate
    bool in_x = x_p >= 0 && x_p < w_l;  // Translated x must be within ragne [0,width)
    bool in_y = y_p >= 0 && y_p < h_l;  // Translated y must be within range [0,height)
    return in_x && in_y;  //True if we're in range for both x and y
  }
};


class cOffsetCellBox : public cCellBox
{
protected:
  int m_size_x;
  int m_size_y;
  
public:
  cOffsetCellBox()
  : cCellBox(-1,-1,0,0)
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
  
  
  int GetSizeX() const
  {
    return m_size_x;
  }
  
  
  int GetSizeY() const
  {
    return m_size_y;
  }
  
  bool InCellBox(int xx, int yy) const
  {
    return cCellBox::InCellBox(xx, yy, m_size_x, m_size_y);
  }
  
  bool InCellBox(int cell_id) const
  {
    return cCellBox::InCellBox(cell_id, m_size_x, m_size_y);
  }
};


#endif /* cCellBox_h */
