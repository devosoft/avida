//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef STRING_UTIL_HH
#define STRING_UTIL_HH

#ifndef STRING_HH
#include "string.hh"
#endif

class cString; // aggregate

struct cStringUtil {
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
  static int Distance(const cString & string1, const cString & string2,
	       int offset=0);
  
  /**
   * Calculate the edit distance between two strings.
   *
   * @return The Edit (Levenstein) distance.
   * @param string1 the first string to compare.
   * @param string2 the second string to compare.
   * @param description The string to write out the differences
   **/
  static int EditDistance(const cString & string1, const cString & string2);
  static int EditDistance(const cString & string1, const cString & string2,
			  cString & info, const char gap=' '); 

  /**
   * Various, overloaded conversion functions for use in templates.  Note
   * that in all cases, the second argument is simply to set the return type.
   **/
  static const cString & Convert(const cString & in_string,
				 const cString & out_string);
  static bool   Convert(const cString & in_string, bool   type_bool);
  static int    Convert(const cString & in_string, int    type_int);
  static double Convert(const cString & in_string, double type_double);
};

#endif
