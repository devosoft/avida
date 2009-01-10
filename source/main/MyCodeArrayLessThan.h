/*
 *  MyCodeArrayLessThan.h
 *  Avida
 *
 *  Called "my_code_array_less_than.hh" prior to 12/5/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#ifndef MyCodeArrayLessThan_h
#define MyCodeArrayLessThan_h

#ifndef cMxCodeArray_h
#include "cMxCodeArray.h"
#endif

class MyCodeArrayLessThan
{
public:
  bool operator()(const cMxCodeArray& x, const cMxCodeArray& y) const { return x < y; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nMyCodeArrayLessThan {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
