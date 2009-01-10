/*
 *  cMessageDisplay.cc
 *  Avida
 *
 *  Called "message_display.cc" prior to 12/7/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#include "cMessageDisplay_headers.h"

#include <climits>
#include <cstdio>
#include <cstdarg>
#include "../third-party/trio/trio.h"
#include <iostream>

using namespace std;


/*
pointers to the cMessageDisplay instantiations actually in use.
each of the message classes gets a handle to one of the cMessageDisplay
instantiations, so that by using the manipulator functions
  void setStandardOutDisplay(cMessageDisplay &out)
and
  void setStandardErrDisplay(cMessageDisplay &err),
the output and error messages can be redirected from, say, the console
to a graphical window and back again (if you want).
*/

static cMessageDisplay* s_info_display;
static cMessageDisplay* s_debug_display;
static cMessageDisplay* s_error_display;
static cMessageDisplay* s_fatal_display;
static cMessageDisplay* s_noprefix_display;



void setInfoDisplay(cMessageDisplay &info){
  cout << "<setInfoDisplay>" << endl;
  s_info_display = &info;
  cout << "<setInfoDisplay> done." << endl;
}
void setDebugDisplay(cMessageDisplay &debug){
  cout << "<setDebugDisplay>" << endl;
  s_debug_display = &debug;
  cout << "<setDebugDisplay> done." << endl;
}
void setErrorDisplay(cMessageDisplay &error){
  cout << "<setErrorDisplay>" << endl;
  s_error_display = &error;
  cout << "<setErrorDisplay> done." << endl;
}
void setFatalDisplay(cMessageDisplay &fatal){
  cout << "<setFatalDisplay>" << endl;
  s_fatal_display = &fatal;
  cout << "<setFatalDisplay> done." << endl;
}
void setNoPrefixDisplay(cMessageDisplay &noprefix){
  cout << "<setNoPrefixDisplay>" << endl;
  s_noprefix_display = &noprefix;
  cout << "<setNoPrefixDisplay> done." << endl;
}


/*
These booleans determine whether to print the function, file, and line
number containing the message call.  The booleans are reset according to
the contents of the environment variable "DEBUG_MESSAGING", which should
be a comma-delimited list containing any of the three words
  "chatter,where,function".

Message types in the classes MCFatal and MCNoPrefix are always
displayed; the remaining messages will only be displayed if
DEBUG_MESSAGING contains "chatter".

The line number and file containing the message call are printed if
DEBUG_MESSAGING contains "where".

The function containing the message call is printed only if
DEBUG_MESSAGING contains "function".

Note that message types of class "MCNoPrefix" will not print the
function, file, or line number.
*/
static bool show_function = false;
static bool show_where = false;
static bool be_quiet = true;


/*
Definition of the five message classes.
*/
cMessageClass MCInfo("Info:", &s_info_display, false, false, false);
cMessageClass MCDebug("Debug:", &s_debug_display, false, false, false);
cMessageClass MCError("Error:", &s_error_display, false, true, false);
cMessageClass MCFatal("Fatal:", &s_fatal_display, true, true, false);
cMessageClass MCNoPrefix("NoPrefix:", &s_noprefix_display, false, true, true);

/*
cMessageClass constructor.
*/
cMessageClass::cMessageClass(
  const char *class_name,
  cMessageDisplay **msg_display,
  bool is_fatal,
  bool is_prefix,
  bool no_prefix
):m_class_name(class_name),
  m_msg_display(msg_display),
  m_is_fatal(is_fatal),
  m_is_prefix(is_prefix),
  m_no_prefix(no_prefix),
  _configured(false)
{
  *msg_display = 0;
}


/*
This configures the behavior of each cMessageClass and cMessageType
according the contents of the environment variable DEBUG_MESSAGING.
this is affected by setting the static booleans "show_function,
show_where, be_quiet".

These booleans determine whether to print the function, file, and line
number containing the message call.  the booleans are reset according to
the contents of the environment variable "DEBUG_MESSAGING", which should
be a comma-delimited list containing any of the three words
  "chatter,where,function".

Message types in the classes MCFatal and MCNoPrefix are always
displayed; the remaining messages will only be displayed if
DEBUG_MESSAGING contains "chatter".

The line number and file containing the message call are printed if
DEBUG_MESSAGING contains "where".

The function containing the message call is printed only if
DEBUG_MESSAGING contains "function".

Message types of class "MCNoPrefix" will not print the function, file,
or line number.
*/
void
cMessageClass::configure(cMessageType *message_type){
  if(!_configured){
    _configured = true;
    char *denv = getenv("DEBUG_MESSAGING");
    if(denv){
      denv = strdup(denv);
      char *tok = strtok(denv, ",");
      while(tok){
        if(strcmp(tok,"where") == 0){
          show_where = true;
        } else if (strcmp(tok,"function") == 0){
          show_function = true;
        } else if (strcmp(tok,"chatter") == 0){
          be_quiet = false;
        }
        tok = strtok(NULL,",");
      }
      free(denv);
    }
  }
  if( (this == &MCInfo)
    ||(this == &MCDebug)
    ||(this == &MCError)
  ) message_type->m_is_active = !be_quiet;
}


/*
cMessageType constructor.
*/
cMessageType::cMessageType(
  const char *type_name,
  cMessageClass &message_class
):m_type_name(type_name),
  m_message_class(message_class),
  m_is_active(true)
{ message_class.configure(this); }


/*
cMessageClosure constructor and destructor.
*/
cMessageClosure::cMessageClosure(
  cMessageType &type,
  const char *function_name,
  const char *file_name,
  int line_number
):_type(type),
  _function(function_name),
  _file(file_name),
  _line(line_number),
  _error(INT_MAX),
  _op_count(0)
{}

cMessageClosure::~cMessageClosure(void){
  if(!(*_type.m_message_class.m_msg_display)){
    cerr << "<~cMessageClosure> Warning:" << endl;
    cerr << "<~cMessageClosure> message display classes not fully initialized;" << endl;
    cerr << "<~cMessageClosure> directing messages of class \""
      << cString(_type.m_type_name)
      << "\" to its default output device..." << endl;
    if(&_type.m_message_class == &MCInfo) {
      (*_type.m_message_class.m_msg_display) = &s_info_msg_cout;
    } else if(&_type.m_message_class == &MCDebug) {
      (*_type.m_message_class.m_msg_display) = &s_debug_msg_cerr;
    } else if(&_type.m_message_class == &MCError) {
      (*_type.m_message_class.m_msg_display) = &s_error_msg_cerr;
    } else if(&_type.m_message_class == &MCFatal) {
      (*_type.m_message_class.m_msg_display) = &s_fatal_msg_cerr;
    } else if(&_type.m_message_class == &MCNoPrefix) {
      (*_type.m_message_class.m_msg_display) = &s_noprefix_msg_cout;
    } else {
      (*_type.m_message_class.m_msg_display) = &s_noprefix_msg_cout;
    }
  }
  (*_type.m_message_class.m_msg_display)->out(_msg);
  if(_type.m_message_class.m_is_fatal) (*_type.m_message_class.m_msg_display)->abort();
}


/*
Pretty-printing for cMessageClosure.
*/
void
cMessageClosure::prefix(void){
  if(_op_count++ == 0 && !_type.m_message_class.m_no_prefix){
    if(_type.m_message_class.m_is_prefix){
      _msg += _type.m_type_name;
      _msg += '-';
    }
    _msg += _type.m_message_class.m_class_name;
    if(!_type.m_message_class.m_is_prefix || show_function || show_where){
      bool colon = false;
      if(!_type.m_message_class.m_is_prefix){
        _msg += _type.m_type_name;
        _msg += ": ";
      }
      if(show_function || show_where){
        _msg += "<";
      }
      if(show_function){
        _msg += _function;
        colon = true;
      }
      if(show_where){
        if(colon){_msg += ':';}
        _msg += _file;
        _msg += ':';
        // FIXME:  consider move this varargs gunk to cString.  -- kgn.
        {
          int size = trio_snprintf((char *) 0, 0, "%i", _line);
          char *buf = new char[size + 1];
          assert(buf != 0);
          trio_snprintf(buf, size+1, "%i", _line);
          _msg += buf;
          delete[] buf;
        }
      }
      if(show_function || show_where){
        _msg += ">";
      }
      _msg += ' ';
    } else {
      _msg += ':';
    }
  }
}


/*
cMessageClosure::va():

Support for cString-based operator<< and operator() definitions in
cMessageClosure.  Variable argument lists are formatted and then printed
into a temporary buffer; the contents of this buffer are then appended
to the cString _msg.
*/

// FIXME:  consider move this varargs gunk to cString.  -- kgn.
cMessageClosure &
cMessageClosure::va(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  int size = trio_vsnprintf((char *) 0, 0, fmt, ap);
  va_end(ap);

  char *buf = new char[size + 1];
  assert(buf != 0);

  va_start(ap, fmt);
  trio_vsnprintf(buf, size+1, fmt, ap);
  va_end(ap);

  prefix();
  _msg += buf;
  delete[] buf;

  return *this;
}

/*
operator<< and operator() definitions for cMessageClosure.

Any kind of input can be placed behind the operator<<, really, but try
to not (further) overcrowd the class...

The operator is meant to convert its input into a string for display.
If for example you implement
  cMessageClosure &operator<<(const myClass &instance);
then you can write a message thus:
  DemoDebug << "this is a myClass instance: " << my_instance;
with the result:
  "Debug [Demo] this is a myClass instance: stuff in my_instance."
*/
cMessageClosure &cMessageClosure::operator<<(char c){ prefix(); _msg += c; return *this; }
cMessageClosure &cMessageClosure::operator<<(unsigned char c){ prefix(); _msg += c; return *this; }
cMessageClosure &cMessageClosure::operator<<(int i){ return va("%d", i); }
cMessageClosure &cMessageClosure::operator<<(unsigned int i){ return va("%u", i); }
cMessageClosure &cMessageClosure::operator<<(long i){ return va("%ld", i); }
cMessageClosure &cMessageClosure::operator<<(unsigned long i){ return va("%lu", i); }
cMessageClosure &cMessageClosure::operator<<(float f){ return va("%g", f); }
cMessageClosure &cMessageClosure::operator<<(double f){ return va("%g", f); }
cMessageClosure &cMessageClosure::operator<<(const void *p){ return va("%p", p); }
cMessageClosure &cMessageClosure::operator<<(const char *s){ prefix(); _msg += s; return *this; }

/*
Definition of the message type "NoPrefix_Msg".
*/
cMessageType NoPrefix_Msg("NoPrefix", MCNoPrefix);
cMessageType Info_GEN_Msg("General", MCInfo);
cMessageType Debug_GEN_Msg("General", MCDebug);
cMessageType Error_GEN_Msg("General", MCError);
cMessageType Fatal_GEN_Msg("General", MCFatal);




