//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_DISPLAY_HH
#define MESSAGE_DISPLAY_HH

/*
cMessageDisplay displays the message "final_msg" to the user/programmer.
cMessageDisplay is meant to be subclassed, where
  out(string final_msg)
is reimplemented to display "final_msg" in the desired manner. 

Instances of the subclass may then be passed to
  setStandardOutDisplay(cMessageDisplay &)
and
  setStandardErrDisplay(cMessageDisplay &)
with expected results.
*/

class cString;

class cMessageDisplay{
public:
  virtual void out(cString &final_msg){}
  virtual void abort(){ abort(); }
};

void setInfoDisplay(cMessageDisplay &info);
void setDebugDisplay(cMessageDisplay &debug);
void setErrorDisplay(cMessageDisplay &error);
void setFatalDisplay(cMessageDisplay &fatal);
void setNoPrefixDisplay(cMessageDisplay &noprefix);

#endif
