/*
 *  cCycleCheck.cc
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology.
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

#include "cCycleCheck.h"
#include "cID.h"

#include <set>

/*!
\brief Internal class; used by cCycleCheck to store private lists of classes or objects encountered in recursive tests.

\remarks
This class is used internally to prevent cycles in recursive operations on collections of classes or objects.

\par See also
cCycleCheck

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting

\ingroup recursiveTesting
*/
struct cCycleCheck::cCycleCheckPrv {
  /*!
  \brief Internal class; used by cCycleCheck::cCycleCheckPrv to provide a comparator for comparing c-style strings.

  \remarks This class is used internally to prevent cycles in recursive operations on collections of classes or objects; it is
  probably not well-suited for any other use.

  The name of this class is passed as the \e Comparator template parameter in declarations of std::set, specialized for
  collections of c-style strings.
  
  \par See also
  cCycleCheck
  \n
  cCycleCheck::cCycleCheckPrv
  
  For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting

  \ingroup recursiveTesting
  */
  struct cLessThanStrCmpFunctor {
    /*! Called by std::set<const char *, cLessThanStrCmpFunctor> to compare two strings. */
    bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) < 0;
  } };
  /*! A collection of names of classes encountered during a recursive unit test. */
  std::set<const char *, cLessThanStrCmpFunctor> m_str_set;
  /*! A collection of IDs of objects encountered during a recursive object-verification. */
  std::set<const cID *> m_id_set;
};

cCycleCheck::cCycleCheck(bool recurse): m_cyck_prv(new cCycleCheckPrv), m_recurse(recurse) {}
cCycleCheck::~cCycleCheck(){ delete m_cyck_prv; }
bool cCycleCheck::ckStr(const char *str){ return m_cyck_prv->m_str_set.insert(str).second; }
bool cCycleCheck::ckID(const cID &id){ return m_cyck_prv->m_id_set.insert(&id).second; }

