/*
 *  cMessageDisplay.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cMessageDisplay_h
#define cMessageDisplay_h

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
  virtual ~cMessageDisplay() { ; }
  
  virtual void out(cString &final_msg){}
  virtual void abort(){ abort(); }
};

void setInfoDisplay(cMessageDisplay &info);
void setDebugDisplay(cMessageDisplay &debug);
void setErrorDisplay(cMessageDisplay &error);
void setFatalDisplay(cMessageDisplay &fatal);
void setNoPrefixDisplay(cMessageDisplay &noprefix);

#endif
