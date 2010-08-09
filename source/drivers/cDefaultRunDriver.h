/*
 *  cDefaultRunDriver.h
 *  Avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cDefaultRunDriver_h
#define cDefaultRunDriver_h

#include "cAvidaDriver.h"
#include "cWorldDriver.h"

class cString;
class cWorld;


class cDefaultRunDriver : public cAvidaDriver, public cWorldDriver
{
private:
  cDefaultRunDriver(); // @not_implemented
  cDefaultRunDriver(const cDefaultRunDriver&); // @not_implemented
  cDefaultRunDriver& operator=(const cDefaultRunDriver&); // @not_implemented

protected:
  cWorld* m_world;
  bool m_done;  // This is set to true when run should finish.
  bool m_fastforward;
  double m_last_generation;
  int m_generation_same_update_count;
  int m_generation_update_fastforward_threshold;
  int m_population_fastforward_threshold;
  
public:
  cDefaultRunDriver(cWorld* world);
  ~cDefaultRunDriver();
  
  void Run();
  
  // Driver Actions
  void SignalBreakpoint() { return; }
  void SetDone() { m_done = true; }
  void SetPause() { return; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);
  
  void ClearFastForward() { m_fastforward = false; m_generation_same_update_count = 0; }
  void UpdateFastForward (double inGeneration, int population);
  bool GetFastForward() { return m_fastforward; }

};

#endif
