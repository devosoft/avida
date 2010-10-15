/*
 *  cStringUtil.cc
 *  Avida
 *
 *  Called "string_util.cc" prior to 12/7/05.
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

#include "cStringUtil.h"

#include "tMatrix.h"

#include "AvidaTools.h"

#include <cstdarg>
#include <cstdio>

using namespace std;
using namespace AvidaTools;


cString cStringUtil::Stringf(const char * fmt, ...) {
  va_list argp;
  char buf[MAX_STRING_LENGTH];

  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  va_end(argp);

  return cString(buf);
}


cString cStringUtil::ToRomanNumeral(const int in_value) {
  cString ret_string;
  if (in_value < 0) {
    ret_string = "-";
    ret_string += ToRomanNumeral(-in_value);
  } else if (in_value > 3999) {
    // Out of bounds; return a blank;
  } else if (in_value >= 1000) {
    ret_string = "M";
    ret_string += ToRomanNumeral(in_value - 1000);
  } else if (in_value >= 900) {
    ret_string = "CM";
    ret_string += ToRomanNumeral(in_value - 900);
  } else if (in_value >= 500) {
    ret_string = "D";
    ret_string += ToRomanNumeral(in_value - 500);
  } else if (in_value >= 400) {
    ret_string = "CD";
    ret_string += ToRomanNumeral(in_value - 400);
  } else if (in_value >= 100) {
    ret_string = "C";
    ret_string += ToRomanNumeral(in_value - 100);
  } else if (in_value >= 90) {
    ret_string = "XC";
    ret_string += ToRomanNumeral(in_value - 90);
  } else if (in_value >= 50) {
    ret_string = "L";
    ret_string += ToRomanNumeral(in_value - 50);
  } else if (in_value >= 40) {
    ret_string = "XL";
    ret_string += ToRomanNumeral(in_value - 40);
  } else if (in_value >= 10) {
    ret_string = "X";
    ret_string += ToRomanNumeral(in_value - 10);
  } else if (in_value == 9) {
    ret_string = "IX";
  } else if (in_value >= 5) {
    ret_string = "V";
    ret_string += ToRomanNumeral(in_value - 5);
  } else if (in_value == 4) {
    ret_string = "IV";
  } else if (in_value > 0) {
    ret_string = "I";
    ret_string += ToRomanNumeral(in_value - 1);
  }
  // else we already have it exactly and don't need to return anything.
  return ret_string;
}


int cStringUtil::StrLength(const char * in){
  // if 'in'==NULL return 0;
  int size = 0;
  if( in != NULL ){
    while( in[size]!='\0' )  ++size; // Count chars in _in
  }
  return size;
}


int cStringUtil::Distance(const cString & string1, const cString & string2,
			  int offset) 
{
  if (offset < 0) return cStringUtil::Distance(string2, string1, -offset);

  const int size1 = string1.GetSize();
  const int size2 = string2.GetSize();

  // Calculate by how much the strings overlap.
  int overlap = Min( size1 - offset,  size2 );

  // Initialize the distance to that part of the strings which do not
  // overlap.
  int num_diffs = size1 + size2 - 2 * overlap;

  // Step through the overlapped section and add on any additional differences.
  for (int i = 0; i < overlap; i++) {
    if (string1[i + offset] != string2[i]) num_diffs++;
  }

  return num_diffs;
}


int cStringUtil::EditDistance(const cString & string1, const cString & string2)
{
  const int size1 = string1.GetSize();
  const int size2 = string2.GetSize();

  if (size1 == 0) return size2;
  if (size2 == 0) return size1;

  int * cur_row  = new int[size1];  // The row we are calculating
  int * prev_row = new int[size1];  // The last row we calculated

  // Initialize the previous row to record the differece from nothing.
  for (int i = 0; i < size1; i++) prev_row[i] = i + 1;

  // Loop through all of the other rows
  for (int i = 0; i < size2; i++) {
    // Initialize the first entry in the current row.
    if (string1[0] == string2[i]) cur_row[0] = i;
    else cur_row[0] = (i < prev_row[0]) ? (i+1) : (prev_row[0] + 1);

    // Move down the cur_row and fill it in.
    for (int j = 1; j < size1; j++) {
      // If the values are equal, keep the value in the upper left.
      if (string1[j] == string2[i]) {
        cur_row[j] = prev_row[j-1];
      }

      // Otherwise, set the current position the the minimal of the three
      // numbers to the upper right in the chart plus one.
      else {
        cur_row[j] = (prev_row[j] < prev_row[j-1]) ? prev_row[j] : prev_row[j-1];
        if (cur_row[j-1] < cur_row[j]) cur_row[j] = cur_row[j-1];
        cur_row[j]++;
      }
    }

    // Swap cur_row and prev_row. (we only really need to move the cur row
    // over to prev, but this saves us from having to keep re-allocating
    // new rows.  We recycle!
    int * temp_row = cur_row;
    cur_row = prev_row;
    prev_row = temp_row;
  }

  // Now that we are done, return the bottom-right corner of the chart.

  const int value = prev_row[size1 - 1];

  delete [] cur_row;
  delete [] prev_row;

  return value;
}

int cStringUtil::EditDistance(const cString & string1, const cString & string2,
			      cString & info, const char gap)
{
  const int size1 = string1.GetSize();
  const int size2 = string2.GetSize();
 
  if (!size1) return size2;
  if (!size2) return size1;

  tMatrix<double> dist_matrix(size2+1, size1+1);

  // Keep track of changes in a mut_matrix.
  //  N=None, M=Mutations, I=Insertion, D=Deletion
  tMatrix<char> mut_matrix(size2+1, size1+1);

  // Initialize the first row and col to record the differece from nothing.
  for (int i = 0; i < size1+1; i++) {
    dist_matrix(0,i) = (double) i;
    mut_matrix(0,i) = 'I';
  }
  for (int i = 0; i < size2+1; i++) {
    dist_matrix(i,0) = (double) i;
    mut_matrix(i,0) = 'D';
  }
  mut_matrix(0,0) = 'N';

  for (int i = 0; i < size2; i++) {
    // Move down the cur_row and fill it out.
    for (int j = 0; j < size1; j++) {
      // If the values are equal, keep the value in the upper left.
      if (string1[j] == string2[i]) {
        dist_matrix(i+1,j+1) = dist_matrix(i,j);
        mut_matrix(i+1,j+1) = 'N';
        continue; // Move on to next entry...
      }

      // Otherwise, set the current position to the minimal of the three
      // numbers above (insertion), to the left (deletion), or upper left
      // (mutation) in the chart, plus one.
      double mut_dist = dist_matrix(i,j) + 1;
      if ((string1[j] == gap) || (string2[i] == gap)) mut_dist -= 0.0001;
      const double ins_dist = dist_matrix(i+1,j) + (string1[j] != gap);
      const double del_dist = dist_matrix(i,j+1) + (string2[i] != gap);

      if (mut_dist < ins_dist && mut_dist < del_dist) {  // Mutation!
        dist_matrix(i+1,j+1) = mut_dist;
        mut_matrix(i+1,j+1) = 'M';
      } else if (ins_dist < del_dist) {                  // Insertion!
        dist_matrix(i+1,j+1) = ins_dist;
        mut_matrix(i+1,j+1) = 'I';
      } else {                                           // Deletion!
        dist_matrix(i+1,j+1) = del_dist;
        mut_matrix(i+1,j+1) = 'D';
      }
    }
  }

  // Construct the list of changes
  int pos1 = size1;
  int pos2 = size2;
  info = "";

  cString mut_string;
  while (pos1 > 0 || pos2 > 0) {
    if (mut_matrix(pos2, pos1) == 'N') {     
      pos1--; pos2--;
      continue;
    }

    // There is a mutation here; determine the type...
    const char old_char = (pos2 > 0) ? string2[pos2-1] : '\0';
    const char new_char = (pos1 > 0) ? string1[pos1-1] : '\0';

    if (mut_matrix(pos2, pos1) == 'M') {
      mut_string.Set("M%d%c%c", pos2-1, old_char, new_char);
      pos1--; pos2--;
    }
    else if (mut_matrix(pos2, pos1) == 'D') {
      mut_string.Set("D%d%c", pos2-1, old_char);
      pos2--;
    }
    else { // if (mut_matrix(pos2, pos1) == 'I') {
      mut_string.Set("I%d%c", pos1-1, new_char);
      pos1--;
    }

    if (info.GetSize() > 0) mut_string += ",";
    info.Insert(mut_string);
  } 

  // Now that we are done, return the bottom-right corner of the chart.
  return (int) dist_matrix(size2, size1);
}



const cString & cStringUtil::Convert(const cString& in_string,
				     const cString& out_string)
{
  return in_string;
}

bool cStringUtil::Convert(const cString& in_string, bool type_bool)
{
  return (in_string.AsInt() != 0);
}

int cStringUtil::Convert(const cString& in_string, int type_int)
{
  return in_string.AsInt();
}

double cStringUtil::Convert(const cString& in_string, double type_double)
{
  return in_string.AsDouble();
}

cString cStringUtil::Convert(const cString& in_string)
{
  return in_string;
}

cString cStringUtil::Convert(bool in_bool)
{
  if (in_bool == false) return "0";
  return "1";
}

cString cStringUtil::Convert(int in_int)
{
  return Stringf("%d", in_int);
}

cString cStringUtil::Convert(double in_double)
{
  return Stringf("%f", in_double);
}

/* Return an array of integers from a string with format x,y..z,a */

tArray<int> cStringUtil::ReturnArray(cString& in_string)
{
  tArray<int> out_list;
  while (in_string.GetSize() != 0) {
    cString chunk = in_string.Pop(',');

    /* if the string has a .. in it find the two numbers on either side of it */

    if (chunk.Find(".") != -1) {
      cString start_str = chunk.Pop('.');
      chunk.RemoveChar('.');
      cString stop_str = chunk.PopWord();
      int start_int = start_str.AsInt();
      int stop_int = stop_str.AsInt();
      for (int i = start_int; i <= stop_int; i++) out_list.Push(i);
    } else {
      out_list.Push(chunk.AsInt());
    }
  }
  return(out_list);
}

