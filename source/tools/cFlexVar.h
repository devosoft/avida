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
  class cFlexVar_Base {
  public:
    cFlexVar_Base() { ; }
    virtual ~cFlexVar_Base() { ; }

    virtual int AsInt() = 0;
    virtual char AsChar() = 0;
    virtual double AsDouble() = 0;
    virtual void SetString(cString & in_str) = 0;
    cString AsString() { cString out_str; SetString(out_str); return out_str; }
    
    virtual eFlexType GetType() { return TYPE_NONE; }
  };
  
  class cFlexVar_Int : public cFlexVar_Base {
  private:
    int value;
  public:
    cFlexVar_Int(int in_val) : value(in_val) { ; }
    ~cFlexVar_Int() { ; }

    int AsInt() { return value; }
    char AsChar() { return (char) value; }
    double AsDouble() { return (double) value; }
    void SetString(cString & in_str) { in_str.Set("%d", value); }

    eFlexType GetType() { return TYPE_INT; }
  };

  class cFlexVar_Char : public cFlexVar_Base {
  private:
    char value;
  public:
    cFlexVar_Char(char in_val) : value(in_val) { ; }
    ~cFlexVar_Char() { ; }

    int AsInt() { return (int) value; }
    char AsChar() { return value; }
    double AsDouble() { return (double) value; }
    void SetString(cString & in_str) { in_str.Set("%c", value); }

    eFlexType GetType() { return TYPE_CHAR; }
  };

  class cFlexVar_Double : public cFlexVar_Base {
  private:
    double value;
  public:
    cFlexVar_Double(double in_val) : value(in_val) { ; }
    ~cFlexVar_Double() { ; }

    int AsInt() { return (int) value; }
    char AsChar() { return (char) value; }
    double AsDouble() { return value; }
    void SetString(cString & in_str) { in_str.Set("%f", value); }

    eFlexType GetType() { return TYPE_DOUBLE; }
  };

  class cFlexVar_String : public cFlexVar_Base {
  private:
    cString value;
  public:
    cFlexVar_String(const cString & in_val) : value(in_val) { ; }
    ~cFlexVar_String() { ; }

    int AsInt() { return value.AsInt(); }
    char AsChar() { return value[0]; }
    double AsDouble() { return value.AsDouble(); }
    void SetString(cString & in_str) { in_str = value; }

    eFlexType GetType() { return TYPE_STRING; }
  };

  cFlexVar_Base * var;

public:
  cFlexVar(int in_value) : var(new cFlexVar_Int(in_value)) { ; }
  cFlexVar(char in_value) : var(new cFlexVar_Char(in_value)) { ; }
  cFlexVar(double in_value) : var(new cFlexVar_Double(in_value)) { ; }
  cFlexVar(const cString & in_value) : var(new cFlexVar_String(in_value)) { ; }
  
  int AsInt() { return var->AsInt(); }
  char AsChar() { return var->AsChar(); }
  double AsDouble() { return var->AsDouble(); }
  cString AsString() { return var->AsString(); }
  void SetString(cString & in_str) { var->SetString(in_str); }
  
  eFlexType GetType() { return var->GetType(); }
};

#endif
