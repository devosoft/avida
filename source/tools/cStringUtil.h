/*
 *  cStringUtil.h
 *  Avida
 *
 *  Called "string_util.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cStringUtil_h
#define cStringUtil_h

#ifndef cString_h
#include "cString.h"
#endif

class cStringUtil
{
private:
  cStringUtil(); // @not_implemented

public:
  static cString Stringf(const char * fmt, ...);
  static cString ToRomanNumeral(const int in_value);
  static int StrLength(const char * _in);

  /**
   * Calculate the Hamming distance between two strings.
   *
   * @return The Hamming distance.
   * @param string1 the first string to compare.
   * @param string2 the second string to compare.
   * @param offset This parameter determines how many characters the second
   * string should be shifted wrt. the first before the comparison.
   **/
  static int Distance(const cString& string1, const cString& string2, int offset = 0);
  
  /**
   * Calculate the edit distance between two strings.
   *
   * @return The Edit (Levenstein) distance.
   * @param string1 the first string to compare.
   * @param string2 the second string to compare.
   * @param description The string to write out the differences
   **/
  static int EditDistance(const cString& string1, const cString& string2);
  static int EditDistance(const cString& string1, const cString& string2, cString& info, const char gap = ' '); 

  /**
   * Various, overloaded conversion functions for use in templates.  Note
   * that in all cases, the second argument is simply to set the return type.
   **/
  static const cString & Convert(const cString& in_string, const cString& out_string);
  static bool   Convert(const cString& in_string, bool   type_bool);
  static int    Convert(const cString& in_string, int    type_int);
  static double Convert(const cString& in_string, double type_double);
  static cString Convert(const cString& in_string);
  static cString Convert(bool in_bool);
  static cString Convert(int in_int);
  static cString Convert(double in_double);
};


#ifdef ENABLE_UNIT_TESTS
namespace nStringUtil {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
