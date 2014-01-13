/*
 *  cString.h
 *  Avida
 *
 *  Called "cstringh" prior to 12/7/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cString_h
#define cString_h

#include "apto/core.h"

#include <cstdlib>
#include <iostream>
#include <cstdarg>
#include <string>
#include <cstring>
#include <cassert>

#define MAX_STRING_LENGTH 4096
#define CONTINUE_LINE_CHAR '\\'

/**
 * A multipurpose string class with many convenient methods of
 * manipulating and comparing strings.
 **/

class cString
{
protected:
  inline void CopyOnWrite();

  // -- Contained Classes --
private:
  // Declarations (only needed)
  class cStringData;

  // cCharProxy -- To detect rvalue vs lvalue ---------------------
  class cCharProxy
  {
  private:
    cString& string;
    int index;

  public:
    cCharProxy(cString& _string, int _index) : string(_string), index(_index) { ; }

    inline cCharProxy& operator=(char c);     // lvalue
    inline cCharProxy& operator+=(char c);    // lvalue
    inline cCharProxy& operator-=(char c);    // lvalue
    inline cCharProxy& operator++();          // lvalue (prefix)
    inline char        operator++(int dummy); // lvalue (postfix)
    inline cCharProxy& operator--();          // lvalue (prefix)
    inline char        operator--(int dummy); // lvalue (postfix)
    inline operator char () const;            // rvalue
  };
  friend class cCharProxy;  // Telling rvalue vs lvalue ....

  // cStringData -- Holds the actual data and is reference count --
  class cStringData : public Apto::RefCountObject<Apto::ThreadSafe>
  {
    // NOTE: Terminating NULL is always there (you can't assign!!)
  private:
    int m_size;   // size of data (NOT INCLUDING TRAILING NULL)
    char* m_data;
    
    cStringData(); // @not_implemented

  public:
    explicit cStringData(int in_size);
    cStringData(int in_size, const char* in);
    cStringData(const cStringData& in);

    ~cStringData()
    {
      delete [] m_data;
    }

    cStringData& operator=(const cStringData& in)
    {
      delete [] m_data;
      m_size = in.GetSize();
      m_data = new char [m_size + 1];
      assert(m_data != NULL);   // Memory Allocation Error: Out of Memory
      for(int i = 0; i < m_size; ++i)  m_data[i] = in[i];
      m_data[m_size] = '\0';
      return (*this);
    }

    int GetSize() const { return m_size; }
    const char* GetData() const { return m_data; }

    char operator[] (int index) const
    {
      assert(index >= 0);    // Lower Bounds Error
      assert(index <= m_size); // Upper Bounds Error
      return m_data[index];
    }

    char& operator[](int index)
    {
      assert(index >= 0);     // Lower Bounds Error
      assert(index <= m_size);  // Upper Bounds Error
      assert(index != m_size);  // Cannot Change Terminating NULL
      return m_data[index];
    }
  };

public:
  cString(const char* in_str = "")
  {
    if (in_str) {
      value = Apto::SmartPtr<cStringData, Apto::InternalRCObject>(new cStringData((int)strlen(in_str), in_str));
    } else {
      value = Apto::SmartPtr<cStringData, Apto::InternalRCObject>(new cStringData(0, ""));
    }
    assert( value );  // Memory Allocation Error: Out of Memory
  }
  cString(const char* in, int in_size) : value(new cStringData(in_size, in))
  {
    assert(in_size >= 0);
    assert( in != NULL );     // NULL input string
    assert( value );  // Memory Allocation Error: Out of Memory
  }
  explicit cString(const int size) : value(new cStringData(size))
  {
    assert( value );    // Memory Allocation Error: Out of Memory
  }
  cString(const cString& in_str) :value(in_str.value) { ; }

  ~cString() { ; }


  // Cast to const char *
  operator const char* () const { return value->GetData(); }

  // Assignment Operators
  cString& operator=(const cString& in_str) { value = in_str.value; return *this; }
  cString& operator=(const char* in)
  {
    assert( in != NULL ); // NULL input string
    value = Apto::SmartPtr<cStringData, Apto::InternalRCObject>(new cStringData(strlen(in),in));
    assert(value);  // Memory Allocation Error: Out of Memory
    return *this;
  }
  
  
  /**
   * Get the size of the string (not including the terminating '\0').
   **/
  int GetSize() const { return value->GetSize(); }

	/**
	 *  Get the string
	 **/
	const char* GetData() const { return value->GetData(); }


  // Comparisons
  int Compare(const char * in) const;  // strcmp like function
  bool operator== (const char * in)    const { return (Compare(in)==0); }
  bool operator== (const cString & in) const;  // A bit optimized
  bool operator!= (const char * in)    const { return !(*this==in); }
  bool operator<  (const char * in)    const { return (Compare(in)<0); }
  bool operator>  (const char * in)    const { return (Compare(in)>0); }
  bool operator<= (const char * in)    const { return (Compare(in)<=0); }
  bool operator>= (const char * in)    const { return (Compare(in)>=0); }

  // Concatenation
  cString & operator+= (const char in)  { return AppendStr(1,&in); }
  cString & operator+= (const char * in){ return AppendStr(strlen(in),in); }
  cString & operator+= (const cString & in){return AppendStr(in.GetSize(),in);}
  cString operator+ (const char in_char){ return (cString(*this) += in_char); }
  cString operator+ (const char * in)   { return (cString(*this) += in); }
  cString operator+ (const cString & in){ return (cString(*this) += in); }


  // Additional modifiers
  cString& Set(const char* fmt, ...);
  cString& Set(const char* fmt, va_list args);

  cString& Insert(const char in, int pos = 0, int excise = 0) { return InsertStr(1, &in, pos, excise); }
  cString& Insert(const char* in, int pos = 0, int excise = 0) { return InsertStr(strlen(in), in, pos, excise); }
  cString& Insert(const cString& in, int pos = 0, int excise = 0) { return InsertStr(in.GetSize(), in, pos, excise); }


  // Removes 'size' characters from 'pos' (default size = to end of string)
  cString& Clip(int pos, int size = -1 /*end of string*/ )
    { if( size < 0 ) size = GetSize() - pos; return InsertStr(0, NULL, pos, size); }
  cString& ClipFront(int size) { /* Clip off first 'clip_size' chars */ return InsertStr(0, NULL, 0, size); }
  cString& ClipEnd(int size) { /* Clip off last 'clip_size' chars */ return InsertStr(0, NULL, GetSize() - size, size); }
  
  /**
   * Find and replace a substring in the string with a different substring.
   * If the substring is not found, the string object is not changed.
   *
   * @return The position at which the substring was found, or -1 if it wasn't found.
   * @param old_string The substring that is going to be replaced.
   * @param new_string The replacement.
   * @param pos The position at which the search should start.
   **/
  int Replace(const cString& old_st, const cString& new_st, int pos = 0);

  cString Pop(const char delim);  // Remove and return up to delim char
  
  /**
   * Remove the first word.
   *
   * @return The removed word.
   **/
  cString PopWord();
  
  /**
   * Remove the first line.
   *
   * @return The removed line.
   **/
  cString PopLine() { return Pop('\n'); } 
  
  /**
   * Remove begining whitespace.
   *
   * @return The number of characters removed.
   **/
  int LeftJustify(); 
  
  /**
   * Remove ending whitespace.
   *
   * @return The number of characters removed.
   **/
  int RightJustify(); 

  /**
    * Remove beginning and ending whitespace.
   **/
  void Trim(); 
  
  /**
   * Reverse the order of the characters in the string.
   **/
  void Reverse(); 
  
  /**
   * Convert the string to lowercase.
   **/
  cString& ToLower(); 
  
  /** 
   * Convert the string to uppercase.
   **/
  cString& ToUpper();
  
  /**
   * Replace all blocks of whitespace with a single space (' ').
   *
   * @see cString::IsWhitespace()
   **/
  void CompressWhitespace();  
  
  /**
   * Get rid of all(!) whitespace. 
   *
   * @see cString::IsWhitespace()
   **/
  void RemoveWhitespace(); 

  /**
   * Get rid of all occurances of a specific character.
   *
   * @see cString::RemoveWhitespace()
   **/
  void RemoveChar(char out_char); 
  
  /**
   * Get rid of one character at a specific location
   **/
  void RemovePos(int pos);
  
  
  // Parse for and replace escape sequences within the string
  cString& ParseEscapeSequences();
  

  // Individal Char Access
  inline char operator[] (int index) const { return static_cast<char>((*value)[index]); }
  cCharProxy operator[] (int index) { return cCharProxy(*this,index); }


  /**
   * Convert string to int.
   *
   * @return The integer value corresponding to the string.
   **/
  int AsInt() const { return static_cast<int>(strtol(*this, NULL, 0)); }

  /**
   * Convert string to double.
   *
   * @return The double value corresponding to the string.
   **/
  double AsDouble() const { return strtod(*this, NULL); }

  // Accessors & Information
  /**
   * Tests whether the string is empty.
   **/
  bool IsEmpty() const { return GetSize() == 0; } // Can just call GetSize
    
  /**
   * Test if the continuation character is at the end of the line
   * if it is strip off the charecter and return true else return 
   * false
   **/
  bool IsContinueLine();

  /**
   * Counts whitespace, beginning at the given position.
   *
   * @param start The index at which counting should begin.
   **/
  int CountWhitespace(int start=0) const; 
  
  /**
   * Counts non-whitespace, beginning at the given position.
   *
   * @param start The index at which counting should begin.
   **/
  int CountWordsize(int start=0) const;
    
  /**
   * Counts until the first occurrence of '\n', beginning at the 
   * given position.
   *
   * @param start The index at which counting should begin.
   **/
  int CountLinesize(int start=0) const; 
  
  /**
   * Counts the number of lines in a string.
   **/
  int CountNumLines() const;           
  
  /**
   * Counts the number of separate words in a string.
   **/
  int CountNumWords() const;         
    
  /**
   * Get a specific word from a string.
   *
   * @param word_id The number of the word, counted from the beginning of 
   * the string, starting with 0.
   **/
  cString GetWord(int word_id=0) const;
    
  /**
   * Get the next word after the specified position. Any leading whitespace 
   * is removed.
   *
   * @param start The position at which the function should start 
   * searching for a word.
   **/
  cString GetWordAt(int start=0) const; 

  /**
   * Test if a character is whitespace. Currently, as whitespace count
   * ' ', '\r', '\t', '\n'.
   *
   * @param pos The position of the character to test.
   **/
  bool IsWhitespace(int pos) const {
    return ( (*this)[pos] == ' '  || (*this)[pos] == '\t' ||
	     (*this)[pos] == '\r' || (*this)[pos] == '\n' );
  }
  
  /**
   * Test if a character is a capital letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsUpperLetter(int pos) const {
    return ((*this)[pos] >= 'A' && (*this)[pos] <= 'Z');
  }
  
  /**
   * Test if a character is not a capital letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsLowerLetter(int pos) const {
    return ((*this)[pos] >= 'a' && (*this)[pos] <= 'z');
  }
  
  /**
   * Test if a character is a letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsLetter(int pos) const {
    return IsUpperLetter(pos) || IsLowerLetter(pos);
  }
  
  /**
   * Test if a character is a number (this includes expressions
   * such as -3.4e5).
   *
   * @param pos The position of the character to test.
   **/
  bool IsNumber(int pos) const {
    return ( ( (*this)[pos] >= '0' && (*this)[pos] <= '9' ) ||
	     (*this)[pos] == '-' || (*this)[pos] == '+' ||
	     (*this)[pos] == '.' || (*this)[pos] == 'e' ||
	     (*this)[pos] == 'E' );
  }
  
  /**
   * Test if a character is a numeral (0, 1, ..., 9).
   *
   * @param pos The position of the character to test.
   **/
  bool IsNumeric(int pos) const {
    return ((*this)[pos] >= '0' && (*this)[pos] <= '9');
  }
  
  /**
   * Test if a character is either a numeral or a letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsAlphaNumeric(int pos) const {
    return IsLetter(pos) || IsNumber(pos);
  }
  
  /**
   * Test whether the complete string consits only of whitespace.
   **/
  bool IsWhitespace() const;
  
  /**
   * Test whether the complete string consits only of uppercase letters.
   **/
  bool IsUpperLetter() const;
  
  /**
   * Test whether the complete string consits only of lowercase letters.
   **/
  bool IsLowerLetter() const;
  
  /**
   * Test whether the complete string consits only of letters.
   **/
  bool IsLetter() const;
  
  /**
   * Test whether the complete string can be seen as a number.
   **/
  bool IsNumber() const;
  
  /**
   * Test whether the complete string consits only of numerals.
   **/
  bool IsNumeric() const;
  
  /**
   * Test whether the complete string consits only of letters or numerals.
   **/
  bool IsAlphaNumeric() const;
  
  /**
   * Search for a single character.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int Find(char in_char, int pos=0) const;
  
  /**
   * Search for a substring.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int Find(const char * in, int pos=0) const{
    return FindStr(in, strlen(in), pos); }
  
  /**
   * Search for a substring.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int Find(const cString & in, int pos=0) const{
    return FindStr(in, in.GetSize(), pos); }
  
  /**
   * Search for a word.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int FindWord(const cString & in, int pos=0) const;
  
  
  /**
   * Cut out a substring.
   *
   * @return The substring.
   * @param start The beginning of the substring in the string.
   * @param size The number of characters in the substring.
   **/
  cString Substring(int start, int size) const ;
  
  /**
   * Determine if in_string is a substring of this string.
   *
   * @return Is this a substring?
   * @param in_string the string to test.
   * @param start The beginning of the substring in the string.
   **/
  bool IsSubstring(const cString & in_string, int start) const;
 
  /**
   * Clip a portion of the string and output it.
   *
   * @return Removed substring.
   * @param pos the position to start the ejection.
   * @param excise number of sites to eject.
   **/
  cString EjectStr(int pos, int excise);


  // {{{ -- INTERNALS -------------------------------------------------------
protected:
  // -- Internal Functions --

  // Methods that take input string size (unsafe to call from outside)
  cString& AppendStr(const int in_size, const char* in);  // Optimized
  cString& InsertStr(const int in_size, const char* in, int pos, int excise=0);
  int FindStr(const char* in_string, const int in_size, int pos) const;

  // -- Internal Data --
protected:
  Apto::SmartPtr<cStringData, Apto::InternalRCObject> value;

// }}} End Internals
};


// {{{ ** External cString Functions **

// iostream input
std::istream& operator >> (std::istream& in, cString& string);
std::ostream& operator << (std::ostream& out, const cString& string);

// }}}

// -- INLINE INCLUDES --

void cString::CopyOnWrite()
{
  if (value->RefCount() != 1) {  // if it is shared
    value = Apto::SmartPtr<cStringData, Apto::InternalRCObject>(new cStringData(*value));  // make own copy of value
  }
}

cString::cCharProxy & cString::cCharProxy::operator= (char c){  // lvalue
  string.CopyOnWrite();
  (*(string.value))[index] = c;
  return *this;
}

cString::cCharProxy & cString::cCharProxy::operator+= (char c){  // lvalue
  string.CopyOnWrite();
  (*(string.value))[index] += c;
  return *this;
}

cString::cCharProxy & cString::cCharProxy::operator-= (char c){  // lvalue
  string.CopyOnWrite();
  (*(string.value))[index] -= c;
  return *this;
}

cString::cCharProxy & cString::cCharProxy::operator++ (){  // lvalue (prefix)
  string.CopyOnWrite();
  ++(*(string.value))[index];
  return *this;
}

char cString::cCharProxy::operator++ (int dummy){  // lvalue (postfix)
  (void)dummy;
  char rv = (*(string.value))[index];
  string.CopyOnWrite();
  ++(*(string.value))[index];
  return rv;
}

cString::cCharProxy & cString::cCharProxy::operator-- (){  // lvalue (prefix)
  string.CopyOnWrite();
  --(*(string.value))[index];
  return *this;
}

char cString::cCharProxy::operator-- (int dummy){  // lvalue (postfix)
  (void)dummy;
  char rv = (*(string.value))[index];
  string.CopyOnWrite();
  --(*(string.value))[index];
  return rv;
}

cString::cCharProxy::operator char () const {  // rvalue
  return static_cast<char>((*(string.value))[index]);
}


// cString Hashing Support
// --------------------------------------------------------------------------------------------------------------

// HASH_TYPE = cString
// We hash a string simply by adding up the individual character values in
// that string and modding by the hash size.  For most applications this
// will work fine (and reasonably fast!) but some patterns will cause all
// strings to go into the same cell.  For example, "ABC"=="CBA"=="BBB".
namespace Apto {
  template <class T, int HashFactor> class HashKey;
  template <int HashFactor> class HashKey<cString, HashFactor>
  {
  public:
    static int Hash(const cString& key)
    {
      unsigned int out_hash = 0;
      for (int i = 0; i < key.GetSize(); i++)
        out_hash += (unsigned int) key[i];
      return out_hash % HashFactor;
    }
  };
};


#endif
