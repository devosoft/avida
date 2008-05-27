/*
 *  cGUIWidget.h
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

// This is a base class for all GUI widgets used in Avida.

#ifndef cGUIWidget_h
#define cGUIWidget_h

#include "cString.h"
#include "cGUIEvent.h"

class cGUIContainer;
class cGUIEvent;

class cGUIWidget {
  friend class cGUIContainer;
protected:
  cGUIContainer * m_parent;
  cString m_name;
  int m_x;
  int m_y;
  int m_width;
  int m_height;

  //  int m_font_size;
public:
  cGUIWidget();
  cGUIWidget(int x, int y, int width=0, int height=0, const cString & name="");
  cGUIWidget(cGUIContainer & parent, int x, int y, int width=0, int height=0, const cString & name="");
  virtual ~cGUIWidget() { ; }

  // This method should be run when the widget is setup and its time to build it.
  virtual void Create() { ; }

  // This method should deal with GUI events.
  virtual bool Handle(cGUIEvent & event) { (void) event; return false; }

  cGUIContainer * GetParent() { return m_parent; }
  const cString & GetName() { return m_name; }
  int GetX() const { return m_x; }
  int GetY() const { return m_y; }
  int GetWidth() const { return m_width; }
  int GetHeight() const { return m_height; }
  //  int GetFontSize() const { return m_font_size; }
  
  void SetName(const cString & _name) { m_name = _name; }
  //  void SetFontSize(int _size) { m_font_size = _size; }
  void Resize(int new_w, int new_h) { m_width = new_w; m_height = new_h; }
};

#endif
