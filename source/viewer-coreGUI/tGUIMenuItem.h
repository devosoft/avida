/*
 *  tGUIMenuItem.h
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

// A single item from a menu that should be able to handle what happens when it is chosen.

#ifndef tGUIMenuItem_h
#define tGUIMenuItem_h

#include "cGUIMenuItem.h"

template <class T, class ARG> class tGUIMenuItem : public cGUIMenuItem {
public:
  typedef void (T::*yCallback)(ARG);

protected:
  T * m_target;
  yCallback m_callback;
  ARG m_arg;

public:
  tGUIMenuItem() : m_target(NULL), m_callback(NULL), m_arg(-1.0) { ; }
  tGUIMenuItem(const cString & name, T * target, void (T::*cb_fun)(ARG), ARG arg=0.0)
    : cGUIMenuItem(name), m_target(target), m_callback(cb_fun), m_arg(arg) { ; }

  T & GetTarget() { return *m_target; }
  const T & GetTarget() const { return *m_target; }
  yCallback GetCallback() const { return m_callback; }
  ARG GetArg() const { return m_arg; }

  void SetCallback(T * target, void (T::*cb_fun)(ARG), ARG arg=0.0) {
    m_target = target;
    m_callback = cb_fun;
    m_arg = arg;
  }

  virtual void Trigger() { if (m_callback != NULL) (m_target->*(m_callback))(m_arg); }
  virtual void Trigger(ARG arg) { if (m_callback != NULL) (m_target->*(m_callback))(arg); }
};

#endif
