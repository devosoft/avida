//
//  cCellBox.h
//  Avida-Core
//
//  Created by Diane Blackwood on 1/13/18.
//

#ifndef cCellBox_h
#define cCellBox_h

class cCellBox {
  private:
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
  
  
    // method to see if cell is in box.
    bool InCellBox(int cellID, int world_x, int world_y) {
      if (0 > m_x  || 0 > m_y || 0 >= m_width || 0 >= m_height ) return true;
      int cell_x = cellID % world_x;
      int cell_y = cellID / world_x;
      if (m_x <= cell_x && cell_x < m_x + m_width && m_y <= cell_y && cell_y < m_y + m_height) {
        return true;
      }
      return false;
    }
};


#endif /* cCellBox_h */
