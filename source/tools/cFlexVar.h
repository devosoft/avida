/*
 *  cFlexVar.h
 *  Avida
 *
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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

#ifndef cFlexVar_h
#define cFlexVar_h

#include "cString.h"

// This class is designed to allow generic core variables to be passed around, and for the receiver to be able
// to handle them however they like.  Each instance of the class represents a single variable whose type can be
// requested and whose value can easily be converted as needed.

class cFlexVar {
public:
  enum eFlexType {
    TYPE_NONE   = 0,
    TYPE_INT    = 1,
    TYPE_CHAR   = 2,
    TYPE_DOUBLE = 3,
    TYPE_STRING = 4,
    TYPE_BOOL   = 5
  };
  
private:
  ////////////////////////////////////////////////////////////
  // Base class for the internal type system....
  class cFlexVar_Base {
  public:
    cFlexVar_Base() { ; }
    virtual ~cFlexVar_Base() { ; }

    virtual bool AsBool() const = 0;
    virtual int AsInt() const = 0;
    virtual char AsChar() const = 0;
    virtual double AsDouble() const = 0;
    virtual void SetString(cString & in_str) const = 0;
    cString AsString() const { cString out_str; SetString(out_str); return out_str; }
    
    virtual eFlexType GetType() const { return TYPE_NONE; }
    virtual void Print(std::ostream& out) const = 0;

#define ABSTRACT_FLEX_VAR_BASE_OP(OP, RETURN_TYPE)                         \
    virtual RETURN_TYPE operator OP (bool in_var) const = 0;                \
    virtual RETURN_TYPE operator OP (int in_var) const = 0;                \
    virtual RETURN_TYPE operator OP (char in_var) const = 0;               \
    virtual RETURN_TYPE operator OP (double in_var) const = 0;             \
    virtual RETURN_TYPE operator OP (const cString & in_var) const = 0;

    ABSTRACT_FLEX_VAR_BASE_OP(==, bool)
    ABSTRACT_FLEX_VAR_BASE_OP(!=, bool)
    ABSTRACT_FLEX_VAR_BASE_OP(<, bool)
    ABSTRACT_FLEX_VAR_BASE_OP(>, bool)
    ABSTRACT_FLEX_VAR_BASE_OP(<=, bool)
    ABSTRACT_FLEX_VAR_BASE_OP(>=, bool)
  };
  
  ////////////////////////////////////////////////////////////
  // Internal class for managing int type......
  class cFlexVar_Bool : public cFlexVar_Base {
  private:
    bool m_value;
  public:
    cFlexVar_Bool(bool in_val) : m_value(in_val) { ; }
    ~cFlexVar_Bool() { ; }
    
    bool AsBool() const { return m_value; }
    int AsInt() const { return (int) m_value; }
    char AsChar() const { return (char) m_value; }
    double AsDouble() const { return (double) m_value; }
    void SetString(cString & in_str) const { if (m_value) in_str = "true"; else in_str = "false"; }
    
    eFlexType GetType() const { return TYPE_BOOL; }
    void Print(std::ostream& out) const { out << m_value; }
    
#define CREATE_FLEX_VAR_BOOL_MATH_OP(OP, RETURN_TYPE)                                                           \
RETURN_TYPE operator OP (bool in_var) const { return (int)m_value OP in_var; }                                   \
RETURN_TYPE operator OP (int in_var) const { return (int)m_value OP in_var; }                                   \
RETURN_TYPE operator OP (char in_var) const { return (int)m_value OP (int) in_var; }                            \
RETURN_TYPE operator OP (double in_var) const { return ((double) m_value) OP in_var; }                     \
RETURN_TYPE operator OP (const cString & in_var) const { return ((double) m_value) OP in_var.AsDouble(); }
    
    CREATE_FLEX_VAR_BOOL_MATH_OP(==, bool);
    CREATE_FLEX_VAR_BOOL_MATH_OP(!=, bool);
    CREATE_FLEX_VAR_BOOL_MATH_OP(<, bool);
    CREATE_FLEX_VAR_BOOL_MATH_OP(>, bool);
    CREATE_FLEX_VAR_BOOL_MATH_OP(<=, bool);
    CREATE_FLEX_VAR_BOOL_MATH_OP(>=, bool);
  };

  
  class cFlexVar_Int : public cFlexVar_Base {
  private:
    int m_value;
  public:
    cFlexVar_Int(int in_val) : m_value(in_val) { ; }
    ~cFlexVar_Int() { ; }

    bool AsBool() const { return (m_value); }
    int AsInt() const { return m_value; }
    char AsChar() const { return (char) m_value; }
    double AsDouble() const { return (double) m_value; }
    void SetString(cString & in_str) const { in_str.Set("%d", m_value); }

    eFlexType GetType() const { return TYPE_INT; }
    void Print(std::ostream& out) const { out << m_value; }

#define CREATE_FLEX_VAR_INT_MATH_OP(OP, RETURN_TYPE)                                                           \
    RETURN_TYPE operator OP (bool in_var) const { return m_value OP (int)in_var; }                             \
    RETURN_TYPE operator OP (int in_var) const { return m_value OP in_var; }                                   \
    RETURN_TYPE operator OP (char in_var) const { return m_value OP (int) in_var; }                            \
    RETURN_TYPE operator OP (double in_var) const { return ((double) m_value) OP in_var; }                     \
    RETURN_TYPE operator OP (const cString & in_var) const { return ((double) m_value) OP in_var.AsDouble(); }

    CREATE_FLEX_VAR_INT_MATH_OP(==, bool);
    CREATE_FLEX_VAR_INT_MATH_OP(!=, bool);
    CREATE_FLEX_VAR_INT_MATH_OP(<, bool);
    CREATE_FLEX_VAR_INT_MATH_OP(>, bool);
    CREATE_FLEX_VAR_INT_MATH_OP(<=, bool);
    CREATE_FLEX_VAR_INT_MATH_OP(>=, bool);
  };


  ////////////////////////////////////////////////////////////
  // Internal class for managing char type......

  class cFlexVar_Char : public cFlexVar_Base {
  private:
    char m_value;
  public:
    cFlexVar_Char(char in_val) : m_value(in_val) { ; }
    ~cFlexVar_Char() { ; }

    bool AsBool() const { return ((int)m_value); }
    int AsInt() const { return (int) m_value; }
    char AsChar() const { return m_value; }
    double AsDouble() const { return (double) m_value; }
    void SetString(cString & in_str) const { in_str.Set("%c", m_value); }

    eFlexType GetType() const { return TYPE_CHAR; }
    void Print(std::ostream& out) const { out << m_value; }

#define CREATE_FLEX_VAR_CHAR_MATH_OP(OP, RETURN_TYPE)                                               \
    RETURN_TYPE operator OP (bool in_var) const { return (int)m_value OP (int)in_var; }                             \
    RETURN_TYPE operator OP (int in_var) const { return ((int) m_value) OP in_var; }                \
    RETURN_TYPE operator OP (char in_var) const { return m_value OP in_var; }                       \
    RETURN_TYPE operator OP (double in_var) const { return ((double) m_value) OP in_var; }          \
    RETURN_TYPE operator OP (const cString & in_var) const { return cString(&m_value, 1) OP in_var; }

    CREATE_FLEX_VAR_CHAR_MATH_OP(==, bool);
    CREATE_FLEX_VAR_CHAR_MATH_OP(!=, bool);
    CREATE_FLEX_VAR_CHAR_MATH_OP(<, bool);
    CREATE_FLEX_VAR_CHAR_MATH_OP(>, bool);
    CREATE_FLEX_VAR_CHAR_MATH_OP(<=, bool);
    CREATE_FLEX_VAR_CHAR_MATH_OP(>=, bool);
  };


  ////////////////////////////////////////////////////////////
  // Internal class for managing double type......

  class cFlexVar_Double : public cFlexVar_Base {
  private:
    double m_value;
  public:
    cFlexVar_Double(double in_val) : m_value(in_val) { ; }
    ~cFlexVar_Double() { ; }

    bool AsBool() const { return ((int)m_value); }
    int AsInt() const { return (int) m_value; }
    char AsChar() const { return (char) m_value; }
    double AsDouble() const { return m_value; }
    void SetString(cString & in_str) const { in_str.Set("%f", m_value); }

    eFlexType GetType() const { return TYPE_DOUBLE; }
    void Print(std::ostream& out) const { out << m_value; }

#define CREATE_FLEX_VAR_DOUBLE_MATH_OP(OP, RETURN_TYPE)                                             \
    RETURN_TYPE operator OP (bool in_var) const { return m_value OP (double)in_var; }                             \
    RETURN_TYPE operator OP (int in_var) const { return m_value OP (double) in_var; }               \
    RETURN_TYPE operator OP (char in_var) const { return m_value OP (double) in_var; }              \
    RETURN_TYPE operator OP (double in_var) const { return m_value OP in_var; }                     \
    RETURN_TYPE operator OP (const cString & in_var) const { return m_value OP in_var.AsDouble(); }

    CREATE_FLEX_VAR_DOUBLE_MATH_OP(==, bool);
    CREATE_FLEX_VAR_DOUBLE_MATH_OP(!=, bool);
    CREATE_FLEX_VAR_DOUBLE_MATH_OP(<, bool);
    CREATE_FLEX_VAR_DOUBLE_MATH_OP(>, bool);
    CREATE_FLEX_VAR_DOUBLE_MATH_OP(<=, bool);
    CREATE_FLEX_VAR_DOUBLE_MATH_OP(>=, bool);
  };


  ////////////////////////////////////////////////////////////
  // Internal class for managing cString type......

  class cFlexVar_String : public cFlexVar_Base {
  private:
    cString m_value;
  public:
    cFlexVar_String(const cString & in_val) : m_value(in_val) { ; }
    ~cFlexVar_String() { ; }

    bool AsBool() const
    {
      cString val(m_value);
      val.ToUpper();
      
      if (val == "TRUE" || val == "T") return true;
      if (val == "FALSE" || val == "F") return false;
      
      return (val.AsInt());
    }
    
    int AsInt() const { return m_value.AsInt(); }
    char AsChar() const { return m_value[0]; }
    double AsDouble() const { return m_value.AsDouble(); }
    void SetString(cString & in_str) const { in_str = m_value; }

    eFlexType GetType() const { return TYPE_STRING; }
    void Print(std::ostream& out) const { out << m_value; }

#define CREATE_FLEX_VAR_STRING_MATH_OP(OP, RETURN_TYPE)                                           \
    RETURN_TYPE operator OP (bool in_var) const { return AsBool() OP in_var; }  \
    RETURN_TYPE operator OP (int in_var) const { return m_value.AsDouble() OP (double) in_var; }  \
    RETURN_TYPE operator OP (char in_var) const { return m_value OP cString(&in_var, 1); }        \
    RETURN_TYPE operator OP (double in_var) const { return m_value.AsDouble() OP in_var; }        \
    RETURN_TYPE operator OP (const cString & in_var) const { return m_value OP in_var; }

    CREATE_FLEX_VAR_STRING_MATH_OP(==, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(!=, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(<, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(>, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(<=, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(>=, bool);
  };

  cFlexVar_Base* m_var;

public:
  // Setup constructors to be able to build this variable from whatever input we need.
  cFlexVar(const cFlexVar & in_var) : m_var(NULL) {
    if (in_var.GetType() == TYPE_INT) m_var = new cFlexVar_Int( in_var.AsInt() );
    else if (in_var.GetType() == TYPE_CHAR) m_var = new cFlexVar_Char( in_var.AsChar() );
    else if (in_var.GetType() == TYPE_DOUBLE) m_var = new cFlexVar_Double( in_var.AsDouble() );
    else if (in_var.GetType() == TYPE_STRING) m_var = new cFlexVar_String( in_var.AsString() );
  }
  cFlexVar(int in_value = 0) : m_var(new cFlexVar_Int(in_value)) { ; }
  cFlexVar(char in_value) : m_var(new cFlexVar_Char(in_value)) { ; }
  cFlexVar(double in_value) : m_var(new cFlexVar_Double(in_value)) { ; }
  cFlexVar(const cString & in_value) : m_var(new cFlexVar_String(in_value)) { ; }
  ~cFlexVar() { delete m_var; }

  // Setup an accessor to determine the native type of this variable.
  eFlexType GetType() const { return m_var->GetType(); }
  void Print(std::ostream& out) const { m_var->Print(out); }


  // Setup accessors to get this variable as any type we might need.
  bool AsBool() const { return m_var->AsBool(); }
  int AsInt() const { return m_var->AsInt(); }
  char AsChar() const { return m_var->AsChar(); }
  double AsDouble() const { return m_var->AsDouble(); }
  cString AsString() const { return m_var->AsString(); }
  void SetString(cString & in_str) const { m_var->SetString(in_str); }
  
  template<class Type> Type As() const;

  // Setup a way to convert the native types
  int MakeInt() { int val = AsInt(); delete m_var; m_var = new cFlexVar_Int(val); return val; }
  char MakeChar() { char val = AsChar(); delete m_var; m_var = new cFlexVar_Char(val); return val; }
  double MakeDouble() { double val = AsDouble(); delete m_var; m_var = new cFlexVar_Double(val); return val; }
  cString MakeString() { cString val = AsString(); delete m_var; m_var = new cFlexVar_String(val); return val; }
  
  // Setup assignment operators...
  cFlexVar & operator=(const cFlexVar & in_var) {
    delete m_var;
    if (in_var.GetType() == TYPE_INT) m_var = new cFlexVar_Int( in_var.AsInt() );
    else if (in_var.GetType() == TYPE_CHAR) m_var = new cFlexVar_Char( in_var.AsChar() );
    else if (in_var.GetType() == TYPE_DOUBLE) m_var = new cFlexVar_Double( in_var.AsDouble() );
    else if (in_var.GetType() == TYPE_STRING) m_var = new cFlexVar_String( in_var.AsString() );
    return *this;
  }
  cFlexVar& operator=(int in_value)             { delete m_var; m_var = new cFlexVar_Int(in_value);    return *this; }
  cFlexVar& operator=(char in_value)            { delete m_var; m_var = new cFlexVar_Char(in_value);   return *this; }
  cFlexVar& operator=(double in_value)          { delete m_var; m_var = new cFlexVar_Double(in_value); return *this; }
  cFlexVar& operator=(const cString & in_value) { delete m_var; m_var = new cFlexVar_String(in_value); return *this; }
  cFlexVar& operator=(const char * in_value)          { delete m_var; m_var = new cFlexVar_String(in_value); return *this; }

  // The following macro will forward all of the commands with the associated operator to the internal class.
#define FORWARD_FLEX_VAR_OP(OP, RETURN_TYPE)                                               \
  RETURN_TYPE operator OP(bool in_var) const { return (*m_var) OP in_var; }                 \
  RETURN_TYPE operator OP(int in_var) const { return (*m_var) OP in_var; }                 \
  RETURN_TYPE operator OP(char in_var) const { return (*m_var) OP in_var; }                \
  RETURN_TYPE operator OP(double in_var) const { return (*m_var) OP in_var; }              \
  RETURN_TYPE operator OP(const cString & in_var) const { return (*m_var) OP in_var; }     \
  RETURN_TYPE operator OP(const cFlexVar & in_var) const {                                 \
    const eFlexType type = in_var.GetType();                                               \
    if (type == TYPE_INT) return (*m_var) OP in_var.AsInt();                               \
    else if (type == TYPE_CHAR) return (*m_var) OP in_var.AsChar();                        \
    else if (type == TYPE_DOUBLE) return (*m_var) OP in_var.AsDouble();                    \
    else /* if (type == TYPE_STRING) */ return (*m_var) OP in_var.AsString();              \
  }

  FORWARD_FLEX_VAR_OP(==, bool)
  FORWARD_FLEX_VAR_OP(!=, bool)
  FORWARD_FLEX_VAR_OP(<, bool)
  FORWARD_FLEX_VAR_OP(>, bool)
  FORWARD_FLEX_VAR_OP(<=, bool)
  FORWARD_FLEX_VAR_OP(>=, bool)
  
};

template<> inline bool cFlexVar::As<bool>() const { return m_var->AsBool(); }
template<> inline int cFlexVar::As<int>() const { return m_var->AsInt(); }
template<> inline char cFlexVar::As<char>() const { return m_var->AsChar(); }
template<> inline double cFlexVar::As<double>() const { return m_var->AsDouble(); }
template<> inline cString cFlexVar::As<cString>() const { return m_var->AsString(); }


inline std::ostream& operator << (std::ostream& out, const cFlexVar & entry)
{
  entry.Print(out);
  return out;
}


#endif
