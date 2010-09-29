/*
 *  cTextViewerDriver_Base.cc
 *  Avida
 *
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include "cTextViewerDriver_Base.h"

#include "cAnalyze.h"
#include "cString.h"
#include "cStringList.h"
#include "cView_Base.h"
#include "cWorld.h"

cTextViewerDriver_Base::cTextViewerDriver_Base(cWorld * world)
  : m_world(world), m_view(NULL), m_done(false)
{
  std::cout.rdbuf(out_stream.rdbuf());
  std::cerr.rdbuf(err_stream.rdbuf());
}

cTextViewerDriver_Base::~cTextViewerDriver_Base()
{
  delete m_world;
}


void cTextViewerDriver_Base::Flush()
{
  // @CAO -- this is currently inefficient!

  // Pull everything from the streams and put it into strings.
  cString out_string(out_stream.str().c_str());
  cString err_string(err_stream.str().c_str());

  // Clear the streams.
  out_stream.str("");
  err_stream.str("");

  // Split it into lines.
  cStringList out_list(out_string, '\n');
  cStringList err_list(err_string, '\n');
  const int new_line_count = out_list.GetSize() + err_list.GetSize();

  // And notify the output...
  while (out_list.GetSize() > 0) {
    m_view->NotifyOutput(out_list.Pop());
  }

  while (err_list.GetSize() > 0) {
    cString cur_string(err_list.Pop());
    cur_string.Insert("! ");
    m_view->NotifyOutput(cur_string);
  }

  if (new_line_count > 0) m_view->NotifyOutput("");
}

bool cTextViewerDriver_Base::ProcessKeypress(cAvidaContext& ctx, int keypress)
{
  return m_view->ProcessKeypress(ctx, keypress);
}
