/*
 *  cAvidaContext.h
 *  Avida
 *
 *  Created by David on 3/13/06.
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
 *  along with this program; if not, write to the Free Software Foundation, 
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*! Class to to hold a random number generator.  Used to keep random
    number creation in a given context seperate from rest of the program
    helps with reapeatablity???? */

#ifndef cAvidaContext_h
#define cAvidaContext_h


class cRandom;
class cWorld;

class cAvidaContext
{
private:
  cWorld* m_world;
  cRandom* m_rng;
  bool m_analyze;
  bool m_testing;
  bool m_org_faults;
  
public:
  cAvidaContext(cWorld* world, cRandom& rng) : m_world(world), m_rng(&rng), m_analyze(false), m_testing(false), m_org_faults(false) { ; }
  cAvidaContext(cWorld* world, cRandom* rng) : m_world(world), m_rng(rng), m_analyze(false), m_testing(false), m_org_faults(false) { ; }
  ~cAvidaContext() { ; }
  
  cWorld* GetWorld() { return m_world; }
  
  void SetRandom(cRandom& rng) { m_rng = &rng; }  
  void SetRandom(cRandom* rng) { m_rng = rng; }  
  cRandom& GetRandom() { return *m_rng; }
  
  void SetAnalyzeMode() { m_analyze = true; }
  void ClearAnalyzeMode() { m_analyze = false; }
  bool GetAnalyzeMode() { return m_analyze; }
  
  void SetTestMode()   { m_testing = true; }   //@MRR  Some modifications I've made need to distinguish
  void ClearTestMode() { m_testing = false; }  //      when we're running a genotype through a test-cpu
  bool GetTestMode()   { return m_testing; }   //      versus when we're not when dealing with reactions rewards.

  void EnableOrgFaultReporting() { m_org_faults = true; }
  void DisableOrgFaultReporting() { m_org_faults = false; }
  bool OrgFaultReporting() { return m_org_faults; }
};

#endif
