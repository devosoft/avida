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
    TYPE_STRING = 4
  };
  
private:
  ////////////////////////////////////////////////////////////
  // Base class for the internal type system....
  class cFlexVar_Base {
  public:
    cFlexVar_Base() { ; }
    virtual ~cFlexVar_Base() { ; }

    virtual int AsInt() const = 0;
    virtual char AsChar() const = 0;
    virtual double AsDouble() const = 0;
    virtual void SetString(cString & in_str) const = 0;
    cString AsString() const { cString out_str; SetString(out_str); return out_str; }
    
    virtual eFlexType GetType() const { return TYPE_NONE; }

#define ABSTRACT_FLEX_VAR_BASE_OP(OP, RETURN_TYPE)                         \
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
  class cFlexVar_Int : public cFlexVar_Base {
  private:
    int value;
  public:
    cFlexVar_Int(int in_val) : value(in_val) { ; }
    ~cFlexVar_Int() { ; }

    int AsInt() const { return value; }
    char AsChar() const { return (char) value; }
    double AsDouble() const { return (double) value; }
    void SetString(cString & in_str) const { in_str.Set("%d", value); }

    eFlexType GetType() const { return TYPE_INT; }

#define CREATE_FLEX_VAR_INT_MATH_OP(OP, RETURN_TYPE)                                                         \
    RETURN_TYPE operator OP (int in_var) const { return value OP in_var; }                                   \
    RETURN_TYPE operator OP (char in_var) const { return value OP (int) in_var; }                            \
    RETURN_TYPE operator OP (double in_var) const { return ((double) value) OP in_var; }                     \
    RETURN_TYPE operator OP (const cString & in_var) const { return ((double) value) OP in_var.AsDouble(); }

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
    char value;
  public:
    cFlexVar_Char(char in_val) : value(in_val) { ; }
    ~cFlexVar_Char() { ; }

    int AsInt() const { return (int) value; }
    char AsChar() const { return value; }
    double AsDouble() const { return (double) value; }
    void SetString(cString & in_str) const { in_str.Set("%c", value); }

    eFlexType GetType() const { return TYPE_CHAR; }

#define CREATE_FLEX_VAR_CHAR_MATH_OP(OP, RETURN_TYPE)                                             \
    RETURN_TYPE operator OP (int in_var) const { return ((int) value) OP in_var; }                \
    RETURN_TYPE operator OP (char in_var) const { return value OP in_var; }                       \
    RETURN_TYPE operator OP (double in_var) const { return ((double) value) OP in_var; }          \
    RETURN_TYPE operator OP (const cString & in_var) const { return cString(&value, 1) OP in_var; }

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
    double value;
  public:
    cFlexVar_Double(double in_val) : value(in_val) { ; }
    ~cFlexVar_Double() { ; }

    int AsInt() const { return (int) value; }
    char AsChar() const { return (char) value; }
    double AsDouble() const { return value; }
    void SetString(cString & in_str) const { in_str.Set("%f", value); }

    eFlexType GetType() const { return TYPE_DOUBLE; }

#define CREATE_FLEX_VAR_DOUBLE_MATH_OP(OP, RETURN_TYPE)                                           \
    RETURN_TYPE operator OP (int in_var) const { return value OP (double) in_var; }               \
    RETURN_TYPE operator OP (char in_var) const { return value OP (double) in_var; }              \
    RETURN_TYPE operator OP (double in_var) const { return value OP in_var; }                     \
    RETURN_TYPE operator OP (const cString & in_var) const { return value OP in_var.AsDouble(); }

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
    cString value;
  public:
    cFlexVar_String(const cString & in_val) : value(in_val) { ; }
    ~cFlexVar_String() { ; }

    int AsInt() const { return value.AsInt(); }
    char AsChar() const { return value[0]; }
    double AsDouble() const { return value.AsDouble(); }
    void SetString(cString & in_str) const { in_str = value; }

    eFlexType GetType() const { return TYPE_STRING; }

#define CREATE_FLEX_VAR_STRING_MATH_OP(OP, RETURN_TYPE)                                           \
    RETURN_TYPE operator OP (int in_var) const { return value.AsDouble() OP (double) in_var; }    \
    RETURN_TYPE operator OP (char in_var) const { return value OP cString(&in_var, 1); }              \
    RETURN_TYPE operator OP (double in_var) const { return value.AsDouble() OP in_var; }          \
    RETURN_TYPE operator OP (const cString & in_var) const { return value OP in_var; }

    CREATE_FLEX_VAR_STRING_MATH_OP(==, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(!=, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(<, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(>, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(<=, bool);
    CREATE_FLEX_VAR_STRING_MATH_OP(>=, bool);
  };

  cFlexVar_Base * var;

public:
  // Setup constructors to be able to build this variable from whatever input we need.
  cFlexVar(const cFlexVar & in_var) : var(NULL) {
    if (in_var.GetType() == TYPE_INT) var = new cFlexVar_Int( in_var.AsInt() );
    else if (in_var.GetType() == TYPE_CHAR) var = new cFlexVar_Char( in_var.AsChar() );
    else if (in_var.GetType() == TYPE_DOUBLE) var = new cFlexVar_Double( in_var.AsDouble() );
    else if (in_var.GetType() == TYPE_STRING) var = new cFlexVar_String( in_var.AsString() );
  }
  cFlexVar(int in_value = 0) : var(new cFlexVar_Int(in_value)) { ; }
  cFlexVar(char in_value) : var(new cFlexVar_Char(in_value)) { ; }
  cFlexVar(double in_value) : var(new cFlexVar_Double(in_value)) { ; }
  cFlexVar(const cString & in_value) : var(new cFlexVar_String(in_value)) { ; }
  ~cFlexVar() { delete var; }

  // Setup an accessor to determine the native type of this variable.
  eFlexType GetType() const { return var->GetType(); }

  // Setup accessors to get this variable as any type we might need.
  int AsInt() const { return var->AsInt(); }
  char AsChar() const { return var->AsChar(); }
  double AsDouble() const { return var->AsDouble(); }
  cString AsString() const { return var->AsString(); }
  void SetString(cString & in_str) const { var->SetString(in_str); }

  // Setup a way to convert the native types
  int MakeInt() { int val = AsInt(); delete var; var = new cFlexVar_Int(val); return val; }
  char MakeChar() { char val = AsChar(); delete var; var = new cFlexVar_Char(val); return val; }
  double MakeDouble() { double val = AsDouble(); delete var; var = new cFlexVar_Double(val); return val; }
  cString MakeString() { cString val = AsString(); delete var; var = new cFlexVar_String(val); return val; }
  
  // Setup assignment operators...
  cFlexVar & operator=(const cFlexVar & in_var) {
    delete var;
    if (in_var.GetType() == TYPE_INT) var = new cFlexVar_Int( in_var.AsInt() );
    else if (in_var.GetType() == TYPE_CHAR) var = new cFlexVar_Char( in_var.AsChar() );
    else if (in_var.GetType() == TYPE_DOUBLE) var = new cFlexVar_Double( in_var.AsDouble() );
    else if (in_var.GetType() == TYPE_STRING) var = new cFlexVar_String( in_var.AsString() );
    return *this;
  }
  cFlexVar & operator=(int in_value)             { delete var; var = new cFlexVar_Int(in_value);    return *this; }
  cFlexVar & operator=(char in_value)            { delete var; var = new cFlexVar_Char(in_value);   return *this; }
  cFlexVar & operator=(double in_value)          { delete var; var = new cFlexVar_Double(in_value); return *this; }
  cFlexVar & operator=(const cString & in_value) { delete var; var = new cFlexVar_String(in_value); return *this; }
  cFlexVar & operator=(char * in_value)          { delete var; var = new cFlexVar_String(in_value); return *this; }

  // The following macro will forward all of the commands with the associated operator to the internal class.
#define FORWARD_FLEX_VAR_OP(OP, RETURN_TYPE)                                             \
  RETURN_TYPE operator OP(int in_var) const { return (*var) OP in_var; }                 \
  RETURN_TYPE operator OP(char in_var) const { return (*var) OP in_var; }                \
  RETURN_TYPE operator OP(double in_var) const { return (*var) OP in_var; }              \
  RETURN_TYPE operator OP(const cString & in_var) const { return (*var) OP in_var; }     \
  RETURN_TYPE operator OP(const cFlexVar & in_var) const {                               \
    const eFlexType type = in_var.GetType();                                             \
    if (type == TYPE_INT) return (*var) OP in_var.AsInt();                               \
    else if (type == TYPE_CHAR) return (*var) OP in_var.AsChar();                        \
    else if (type == TYPE_DOUBLE) return (*var) OP in_var.AsDouble();                    \
    else /* if (type == TYPE_STRING) */ return (*var) OP in_var.AsString();              \
  }

  FORWARD_FLEX_VAR_OP(==, bool)
  FORWARD_FLEX_VAR_OP(!=, bool)
  FORWARD_FLEX_VAR_OP(<, bool)
  FORWARD_FLEX_VAR_OP(>, bool)
  FORWARD_FLEX_VAR_OP(<=, bool)
  FORWARD_FLEX_VAR_OP(>=, bool)
  
};


#endif
