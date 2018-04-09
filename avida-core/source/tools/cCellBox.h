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
  
  //cCellBox(); // Pre-C++11 style (put it in private)  the compiler still makes it, but no one can use it.
  //cCellBox() = delete; //C++11 and beyound style)
  
  //The compiler automatically will provide the ability to
  /*
   cCellBox a_box;
   cCellBox b_box(a_box); //copy constructor   (also a move contructor)
   cCellBox c_box;
   c_box = a_box; //operator=  the assignment operator
   cCellBox::~cCellBox   //destructor
   */
  
  /*
   Add in the parser to the part of the code where requisites are read in
   Actually add the cRequistieCellBox to the cRequisite
   
   Add in default constructor that initializes xx, yy, width, height to -1
   Add in a public method to check if a cellID falls within hour cell box
   cellID -> X-coordinate = cellID % WORLD_X
   cellID -> Y-coordinate = cellID / WORLD_X
   True if
   m_x != -1  assume that if true m_Y also not != -1
   OR
   [
   m_y <= y-coordinate <= m_y + m_height
   m_x <= x-coordinate <= m_x + m_width
   ]
   Thinking
   bool InCellBox(int cell_id, int world_x, int world_y)
   where we get world_x and world_y passed via the TestRequistie as well
   
   */
  
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
  
  cCellBox(int xx, int yy, int width=1, int height=1)
  : m_x(xx)
  , m_y(yy)
  , m_width(width)
  , m_height(height)
  {
    assert(m_x >= 0 && m_y >= 0 && m_width > 0 && m_height > 0);
  }
  
  cCellBox(const cCellBox& cbox)
  {
    *this = cbox;
  }
  
  cCellBox& operator=(const cCellBox& cbox)
  {
    m_x = cbox.m_x;
    m_y = cbox.m_y;
    m_width = cbox.m_width;
    m_height = cbox.m_height;
    return *this;
  }
  
  
  int GetX() const
  {
    return m_x;
  }
  
  int GetY() const
  {
    return m_y;
  }
  
  int GetWidth() const
  {
    return m_width;
  }
  
  int GetHeight() const
  {
    return m_height;
  }
  
  // method to see if cell is in box.
  bool InCellBox(int cellID, int world_x, int world_y) const
  {
    if (0 > m_x  || 0 > m_y || 0 >= m_width || 0 >= m_height ) return true;
    int cell_x = cellID % world_x;
    int cell_y = cellID / world_x;
    if (m_x <= cell_x && cell_x < m_x + m_width && m_y <= cell_y && cell_y < m_y + m_height) {
      return true;
    }
    return false;
  }
};


class cWorldCellBox : public cCellBox
{
protected:
  int m_world_x, m_world_y;

public:
  cWorldCellBox()
  : cCellBox(-1,-1,-1,-1)
  , m_world_x(-1)
  , m_world_y(-1)
  {
  }
  
  cWorldCellBox(int world_x, int world_y, int xx, int yy, int width, int height)
  : cCellBox(xx, yy, width, height)
  , m_world_x(world_x)
  , m_world_y(world_y)
  {
  }
  
  cWorldCellBox(const cWorldCellBox& cbox)
  : cCellBox(cbox)
  {
    m_world_x = cbox.m_world_x;
    m_world_y = cbox.m_world_y;
  }
  
  cWorldCellBox& operator=(const cWorldCellBox& cbox)
  {
    cCellBox::operator=(cbox);
    m_world_x = cbox.m_world_x;
    m_world_y = cbox.m_world_y;
    return *this;
  }
  
  bool InCellBox(int x, int y) const
  {
    int cell_id = m_world_x * y + x;
    if (m_world_x == -1 || m_world_y == -1)
      return true;
    return cCellBox::InCellBox(cell_id, m_world_x, m_world_y);
  }
  
  bool InCellBox(int cell_id) const
  {
    return cCellBox::InCellBox(cell_id, m_world_x, m_world_y);
  }
  
  int GetWorldX() const 
  {
    return m_world_x;
  }
  
  int GetWorldY() const
  {
    return m_world_y;
  }
};


#endif /* cCellBox_h */
