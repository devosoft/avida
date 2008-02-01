/*
 *  cGUIWidget.cc
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

#include "cGUIWidget.h"
#include "cGUIContainer.h"

cGUIWidget::cGUIWidget()
  : m_parent(NULL), m_x(0), m_y(0), m_width(0), m_height(0)
{
}

cGUIWidget::cGUIWidget(int x, int y, int width, int height, const cString & name)
    : m_parent(NULL), m_name(name), m_x(x), m_y(y), m_width(width), m_height(height)
{
}

cGUIWidget::cGUIWidget(cGUIContainer & parent, int x, int y, int width, int height, const cString & name)
  : m_parent(&parent), m_name(name), m_x(x), m_y(y), m_width(width), m_height(height)
{
  parent.Add(this);
}
