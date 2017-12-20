/*
 *  cReactionRequisite.h
 *  Avida
 *
 *  Called "reaction_requisite.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cReactionRequisite_h
#define cReactionRequisite_h

#include <climits>
#include <cassert>

#ifndef tList_h
#include "tList.h"
#endif

class cReaction;

class cRequisiteCellBox {
  private:
    int m_x;
    int m_y;
    int m_width;
    int m_height;
  
  //cRequisiteCellBox(); // Pre-C++11 style (put it in private)  the compiler still makes it, but no one can use it.
  //cRequisiteCellBox() = delete; //C++11 and beyound style)
  
  //The compiler automatically will provide the ability to
  /*
  cRequisiteCellBox a_box;
  cRequisiteCellBox b_box(a_box); //copy constructor   (also a move contructor)
  cRequisiteCellBox c_box;
  c_box = a_box; //operator=  the assignment operator
  cRequisiteCellBox::~cRequisiteCellBox   //destructor
  */
  
  /*
  For today:
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
  cRequisiteCellBox()
  :   m_x(-1)
  ,  m_y(-1)
  ,  m_width(-1)
  ,  m_height(-1)
  {
  }
  
  
    cRequisiteCellBox(int xx, int yy, int width=1, int height=1)
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

class cReactionRequisite
{
private:
  tList<cReaction> prior_reaction_list;
  tList<cReaction> prior_noreaction_list;
  int min_task_count;
  int max_task_count;
  int min_reaction_count;
  int max_reaction_count;
  int min_tot_reaction_count;
  int max_tot_reaction_count;
  int divide_only;
  int parasite_only;
  cRequisiteCellBox cell_box;


  cReactionRequisite(const cReactionRequisite&); // @not_implemented
  cReactionRequisite& operator=(const cReactionRequisite&);

public:
  cReactionRequisite() : min_task_count(0) , max_task_count(INT_MAX),
    min_reaction_count(0) , max_reaction_count(INT_MAX),
	  min_tot_reaction_count(0), max_tot_reaction_count(INT_MAX), divide_only(0), parasite_only(0) { ; }
  ~cReactionRequisite() { ; }

  const tList<cReaction>& GetReactions() const { return prior_reaction_list; }
  const tList<cReaction>& GetNoReactions() const { return prior_noreaction_list; }
  int GetMinTaskCount() const { return min_task_count; }
  int GetMaxTaskCount() const { return max_task_count; }
  int GetMinReactionCount() const { return min_reaction_count; }
  int GetMaxReactionCount() const { return max_reaction_count; }
  int GetDivideOnly() const { return divide_only; }
  int GetMinTotReactionCount() const { return min_tot_reaction_count; }
  int GetMaxTotReactionCount() const { return max_tot_reaction_count; }
  int GetParasiteOnly() const { return parasite_only; }
  cRequisiteCellBox GetCellBox() const {return cell_box; }

  void AddReaction(cReaction* in_reaction) {
    prior_reaction_list.PushRear(in_reaction);
  }
  void AddNoReaction(cReaction* in_reaction) {
    prior_noreaction_list.PushRear(in_reaction);
  }
  void SetMinTaskCount(int min) { min_task_count = min; }
  void SetMaxTaskCount(int max) { max_task_count = max; }
  void SetMinReactionCount(int min) { min_reaction_count = min; }
  void SetMaxReactionCount(int max) { max_reaction_count = max; }
  void SetDivideOnly(int div) { divide_only = div; }
  void SetMinTotReactionCount(int min) { min_tot_reaction_count = min; }
  void SetMaxTotReactionCount(int max) { max_tot_reaction_count = max; }
  void SetParasiteOnly(int para) { parasite_only = para; }
  void SetCellBox(int xx, int yy, int width, int height) {
    cell_box = cRequisiteCellBox(xx, yy, width, height);   //So calling the non-default and opperator=; in c++11 and beyound it will be a move]
  }
  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cReactionRequisite& in) const { return &in == this; }
};

#endif
