/*
 *  cTextViewerDriver_Base.h
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

#ifndef cTextViewerDriver_Base_h
#define cTextViewerDriver_Base_h

#ifndef cAvidaDriver_h
#include "cAvidaDriver.h"
#endif

#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

#include <sstream>
#include <iostream>

class cAvidaContext;
class cView_Base;
class cWorld;

class cTextViewerDriver_Base : public cAvidaDriver, public cWorldDriver {
protected:
  cWorld* m_world;
  cView_Base* m_view;
  bool m_done;  // This is set to true when run should finish.
  bool m_pause;

  std::stringstream out_stream;
  std::stringstream err_stream;

  // Protected constructor.
  cTextViewerDriver_Base(cWorld * world);
  ~cTextViewerDriver_Base();

public:
  // Override standard output and error.
  bool IsInteractive() { return true; }
  void Flush();

  bool ProcessKeypress(cAvidaContext& ctx, int keypress);
};

#endif
