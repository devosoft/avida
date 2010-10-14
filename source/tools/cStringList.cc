/*
 *  cStringList.cc
 *  Avida
 *
 *  Called "string_list.cc" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cStringList.h"

#include "cStringIterator.h"

using namespace std;


cStringList::cStringList(const cString & _list, char seperator)
{
  Load(_list, seperator);
}

cStringList::cStringList(const cStringList & _list)
{
  tConstListIterator<cString> list_it(_list.string_list);
  while (list_it.Next() != NULL) {
    string_list.PushRear(new cString( *(list_it.Get()) ));
  }
}

cStringList::~cStringList()
{
  while (string_list.GetSize() > 0) delete string_list.Pop();
}


cStringList & cStringList::operator=(const cStringList & _list)
{
  // If this list already as anything in it, make sure to delete it.
  Clear();

  // Now add the new list into this one.
  tConstListIterator<cString> list_it(_list.string_list);
  while (list_it.Next() != NULL) {
    string_list.PushRear(new cString( *(list_it.Get()) ));
  }
  return *this;
}

bool cStringList::HasString(const cString & test_string) const
{
  tConstListIterator<cString> string_it(string_list);
  const cString * cur_string = NULL;
  while ((cur_string = string_it.Next()) != NULL) {
    if (test_string == *cur_string) {
      return true;
    }
  }
  return false;
}

cString cStringList::PopString(const cString & test_string)
{
  tListIterator<cString> string_it(string_list);
  const cString * cur_string = NULL;
  while ((cur_string = string_it.Next()) != NULL) {
    if (test_string == *cur_string) {
      return ReturnString(string_list.Remove(string_it));
    }
  }
  return "";
}

cString cStringList::PopLine(int line_num)
{
  tListIterator<cString> string_it(string_list);
  string_it.Next();
  for (int k = 0; k < line_num && string_it.Next(); k++) ;
  return ReturnString(string_list.Remove(string_it));
}

int cStringList::LocateString(const cString & test_string) const
{
  tConstListIterator<cString> string_it(string_list);
  int pos = -1;  // @MRR ugly
  const cString * cur_string = NULL;
  while ((cur_string = string_it.Next()) != NULL) {
    ++pos;
    if (test_string == *cur_string) 
      return pos;
  }
  return -1;
}
