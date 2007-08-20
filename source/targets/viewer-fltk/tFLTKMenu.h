/*
 *  tFLTKMenu.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

// This is a class to manage the FLTK GUI menus...

#ifndef tFLTKMenu_h
#define tFLTKMenu_h

#include "tGUIMenu.h"
#include "fltk-defs.h"

#include "FL/Fl_Choice.H"

template <class T> class tFLTKMenu : public tGUIMenu<T, int> {
protected:
  Fl_Choice * m_menu;

  void SetupOption(int opt_id) {
    tGUIMenuItem<T,int> * menu_item = tGUIMenu<T,int>::m_menu_options[opt_id];
    m_menu->add(menu_item->GetName(), 0, (Fl_Callback*) GenericMenuCallback, menu_item);
  }

public:
  tFLTKMenu(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="")
    : tGUIMenu<T, int>(parent, x, y, width, height, name)
    , m_menu(new Fl_Choice(x, y, width, height, name))
  {
    m_menu = new Fl_Choice(x, y, width, height, name);
    // m_menu->callback((Fl_Callback*) GenericMenuCallback, (void*)(this));
  }
  ~tFLTKMenu() { delete m_menu; }

  void SetActive(int id) { m_menu->value(id); }

  void BindKey(int key) { (void) key; }
};


#endif
