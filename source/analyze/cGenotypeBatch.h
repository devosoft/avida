/*
 *  cGenotypeBatch.h
 *  Avida
 *
 *  Called "genotype_batch.hh" prior to 12/2/05.
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

#ifndef cGenotypeBatch_h
#define cGenotypeBatch_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

// cGenotypeBatch      : Collection of cAnalyzeGenotypes

class cAnalyzeGenotype;

class cGenotypeBatch {
private:
  tListPlus<cAnalyzeGenotype> genotype_list;
  cString name;
  bool is_lineage;
  bool is_aligned;
private:
  // disabled copy constructor.
  cGenotypeBatch(const cGenotypeBatch &);
public:
  cGenotypeBatch() : name(""), is_lineage(false), is_aligned(false) { ; }
  ~cGenotypeBatch() { ; }

  tListPlus<cAnalyzeGenotype> & List() { return genotype_list; }
  cString & Name() { return name; }
  bool IsLineage() { return is_lineage; }
  bool IsAligned() { return is_aligned; }

  void SetLineage(bool _val=true) { is_lineage = _val; }
  void SetAligned(bool _val=true) { is_aligned = _val; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenotypeBatch {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
