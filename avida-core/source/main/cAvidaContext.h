/*
 *  cAvidaContext.h
 *  Avida
 *
 *  Created by David on 3/13/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*! Class to to hold a random number generator.  Used to keep random
    number creation in a given context seperate from rest of the program
    helps with reapeatablity???? */

#ifndef cAvidaContext_h
#define cAvidaContext_h

#include "avida/core/Types.h"

class cWorld;


class cAvidaContext
{
private:
  Avida::WorldDriver* m_driver;
  Apto::Random* m_rng;

  bool m_analyze;
  bool m_testing;
  bool m_org_faults;
  
public:
  cAvidaContext(Avida::WorldDriver* driver, Apto::Random& rng) : m_driver(driver), m_rng(&rng), m_analyze(false), m_testing(false), m_org_faults(false) { ; }
  cAvidaContext(Avida::WorldDriver* driver, Apto::Random* rng) : m_driver(driver), m_rng(rng), m_analyze(false), m_testing(false), m_org_faults(false) { ; }
  ~cAvidaContext() { ; }
  
  Avida::WorldDriver& Driver() { return *m_driver; }
  bool HasDriver() const { return (m_driver != NULL); }
  
  void SetRandom(Apto::Random& rng) { m_rng = &rng; }
  void SetRandom(Apto::Random* rng) { m_rng = rng; }
  Apto::Random& GetRandom() { return *m_rng; }
  
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
