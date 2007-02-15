/*
 *  cHardwareManager.h
 *  Avida
 *
 *  Created by David on 10/18/05.
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

#ifndef cHardwareManager_h
#define cHardwareManager_h

#ifndef cInstSet_h
#include "cInstSet.h"
#endif
#ifndef cTestCPU_h
#include "cTestCPU.h"
#endif

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


class cOrganism;
class cHardwareBase;
class cWorld;

class cHardwareManager
{
#if USE_tMemTrack
  tMemTrack<cHardwareManager> mt;
#endif
private:
  cWorld* m_world;
  cInstSet m_inst_set;
  int m_type;
//  cTestResources m_testres;
  
  void LoadInstSet(cString filename);
  
  cHardwareManager(); // @not_implemented
  cHardwareManager(const cHardwareManager&); // @not_implemented
  cHardwareManager& operator=(const cHardwareManager&); // @not_implemented
  

public:
  cHardwareManager(cWorld* world);
  ~cHardwareManager() { ; }
  
  cHardwareBase* Create(cOrganism* in_org);
  cTestCPU* CreateTestCPU() { return new cTestCPU(m_world /*, &m_testres*/); }

  const cInstSet& GetInstSet() const { return m_inst_set; }
  cInstSet& GetInstSet() { return m_inst_set; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nHardwareManager {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
