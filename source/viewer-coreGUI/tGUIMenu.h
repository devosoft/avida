/*
 *  tGUIMenu.h
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

// This is a base class/template for all GUI widgets that act as menus.

#ifndef tGUIMenu_h
#define tGUIMenu_h

#include "cGUIMenu.h"
#include "tArray.h"
#include "tGUIMenuItem.h"

template <class T, class ARG> class tGUIMenu : public cGUIMenu {
protected:
  tArray< tGUIMenuItem<T,ARG> * > m_menu_options;

  virtual void SetupOption(int opt_id) = 0;

public:
  tGUIMenu(cGUIContainer & parent, int x, int y, int width, int height, const cString & name="")
    : cGUIMenu(parent, x, y, width, height, name)
  { ; }
  virtual ~tGUIMenu() { 
    for (int i = 0; i < m_menu_options.GetSize(); i++) delete m_menu_options[i];
  }

  int GetSize() { return m_menu_options.GetSize(); }
  const cString & GetName(int id) const { return m_menu_options[id]->GetName(); }
  T & GetTarget(int id) { return *(m_menu_options[id]->GetTarget()); }
  ARG GetArg(int id) { return m_menu_options[id]->GetArg(); }
  const cGUIMenuItem & GetOption(int id) const { return *(m_menu_options[id]); }

  void AddOption(const cString & name, T * target, void (T::*cb_fun)(ARG), ARG arg=0) {
    int new_id = m_menu_options.GetSize();
    m_menu_options.Push( new tGUIMenuItem<T,ARG>(name, target, cb_fun, arg) );
    SetupOption(new_id);
  }

  virtual void Trigger(int id) {
    if (m_menu_options[id]->GetCallback() != NULL) {
      ARG cur_arg = m_menu_options[id]->GetArg();
      T & cur_target = m_menu_options[id]->GetTarget();
      ( cur_target.*(m_menu_options[id]->GetCallback()) )( cur_arg );
    }
  }
  virtual void Trigger(int id, ARG arg) {
    if (m_menu_options[id]->GetCallback() != NULL) {
      (m_menu_options[id]->GetTarget().*(m_menu_options[id]->GetCallback()))(arg);
    }
  }
};

#endif
