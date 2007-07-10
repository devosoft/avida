/*
 *  cGUIDriver.cc
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

#include "cGUIDriver.h"

#include "cWorld.h"

#include "cGUIBox.h"
#include "cGUIWindow.h"

cGUIDriver::cGUIDriver(cWorld* world)
  : m_world(world)
  , m_info(world->GetPopulation(), 12)
  , m_done(false)
  , m_main_window(NULL)
  , m_update_box(NULL)
{
}


cGUIDriver::~cGUIDriver()
{
  if (m_main_window != NULL) delete m_main_window;

  delete m_world;
}


bool cGUIDriver::LaunchViewer()
{
  m_main_window = BuildWindow(800, 600, "Avida Viewer");
  m_main_window->SetSizeRange(800, 600);

  m_update_box = BuildBox(m_main_window, 10, 10, 200, 50, "Update: 0");
  m_update_box->SetType(cGUIBox::BOX_FLAT);
  m_update_box->SetFontSize(30);
  m_update_box->Refresh();

  m_main_window->Finalize();

  return true;
}


bool cGUIDriver::UpdateViewer()
{
}
