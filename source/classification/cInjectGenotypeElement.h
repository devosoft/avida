/*
 *  cInjectGenotypeElement.h
 *  Avida
 *
 *  Called "inject_genotype_element.hh" prior to 11/30/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cInjectGenotypeElement_h
#define cInjectGenotypeElement_h

#ifndef defs_h
#include "defs.h"
#endif

class cInjectGenotype;

class cInjectGenotypeElement {
private:
  cInjectGenotype* inject_genotype;
  cInjectGenotypeElement* next;
  cInjectGenotypeElement* prev;
  
  cInjectGenotypeElement(const cInjectGenotypeElement&); // @not_implemented
  cInjectGenotypeElement& operator=(const cInjectGenotypeElement&); // @not_implemented
public:
  cInjectGenotypeElement(cInjectGenotype* in_gen = NULL)
    : inject_genotype(in_gen), next(NULL), prev(NULL) { ; }
  ~cInjectGenotypeElement() { ; }

  cInjectGenotype* GetInjectGenotype() const { return inject_genotype; }
  cInjectGenotypeElement* GetNext() const { return next; }
  cInjectGenotypeElement* GetPrev() const { return prev; }

  void SetNext(cInjectGenotypeElement* in_next) { next = in_next; }
  void SetPrev(cInjectGenotypeElement* in_prev) { prev = in_prev; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nInjectGenotypeElement {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif
