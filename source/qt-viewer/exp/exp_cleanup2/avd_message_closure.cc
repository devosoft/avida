#ifndef AVD_MESSAGE_CLOSURE_HH
#include "avd_message_closure.hh"
#endif

#ifndef AVD_MESSAGE_CLASS_BASE_HH
#include "avd_message_class_base.hh"
#endif 
#ifndef AVD_MESSAGE_CLOSURE_BASE_HH
#include "avd_message_closure_base.hh"
#endif
#ifndef AVD_MESSAGE_DISPLAY_BASE_HH
#include "avd_message_display_base.hh"
#endif
#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

#ifndef TRIO_TRIO_H
#include "trio.h"
#endif

#ifndef _CPP_IOSTREAM
#include <iostream>
#endif

using namespace std;

// stub.
avdMessageClosureBase &avdMessageClosure::operator<<(const QString &s)
{ prefix(); return *this; }

avdMessageClosure::avdMessageClosure(
  const avdMessageTypeBase &type,
  const char *function_name,
  const char *file_name,
  int line_number
):m_type(type)
, m_function(function_name)
, m_file(file_name)
, m_line(line_number)
, m_error(INT_MAX)
, m_op_count(0)
{}
avdMessageClosure::~avdMessageClosure(void){
  if(!m_type.messageDisplay()){
    cerr << "* Error in message display system *" << endl;
    cerr << "* in class avdMessageClosure destructor *" << endl;
    cerr << endl;
    cerr << "Trying to display a message to the user, but my pointer to" << endl;
    cerr << "my message-displayer doesn't point anywhere.  So I'm" << endl;
    cerr << "going to send the message to the \"standard error\" device." << endl;
    cerr << "Here's the message:" << endl;
    cerr << endl;
    cerr << m_msg << endl;
    cerr << endl;
    if(m_type.isFatal()) abort();
  } else {
    (*m_type.messageDisplay())->out(m_msg);
    if(m_type.isFatal()) (*m_type.messageDisplay())->abort();
  }
}
void avdMessageClosure::prefix(void){
  if(m_op_count++ == 0 && !m_type.noPrefix()){
    if(m_type.isPrefix()){
      m_msg += m_type.typeName();
      m_msg += '-';
    }
    m_msg += m_type.className();
    if(!m_type.isPrefix()
      ||m_type.showFunction()
      ||m_type.showWhere()
    ){
      bool colon = false;
      if(!m_type.isPrefix()){
        m_msg += m_type.typeName();
        m_msg += ": ";
      }
      if(m_type.showFunction() || m_type.showWhere()){
        m_msg += "<";
      }
      if(m_type.showFunction()){
        if(!m_function){
          cerr << "* Error in message display system in *" << endl;
          cerr << "* void avdMessageClosure::prefix() *" << endl;
          cerr << endl;
          cerr << "I'm trying to prepare a message for you, and I'm" << endl;
          cerr << "supposed to print the name of the function" << endl;
          cerr << "from which the message originates, but I" << endl; 
          cerr << "wasn't given the function name." << endl;
          cerr << "But I'll try to print the rest of the message." << endl;
        } else { m_msg += m_function; }
        colon = true;
      }
      if(m_type.showWhere()){
        if(colon){m_msg += ':';}
        if(!m_file){
          cerr << "* Error in message display system in *" << endl;
          cerr << "* void avdMessageClosure::prefix() *" << endl;
          cerr << endl;
          cerr << "I'm trying to prepare a message for you, and I'm" << endl;
          cerr << "supposed to print the name of the file" << endl;
          cerr << "from which the message originates, but I" << endl; 
          cerr << "wasn't given the file name." << endl;
          cerr << "But I'll try to print the rest of the message." << endl;
        } else { m_msg += m_file; }
        m_msg += ':';
        // FIXME:  consider move this varargs gunk to cString.  -- kgn.
        {
          int size = trio_snprintf((char *) 0, 0, "%i", m_line);
          char *buf = new char[size + 1];
          //assert(buf != 0);
          if(!buf){
            cerr << "* Error in message display system in *" << endl;
            cerr << "* void avdMessageClosure::prefix() *" << endl;
            cerr << endl;
            cerr << "I'm trying to prepare a message for you, and I'm" << endl;
            cerr << "supposed to print the line number of the source-code" << endl;
            cerr << "file from which the message originates, but I can't" << endl; 
            cerr << "allocate enough space to print the line number." << endl;
            cerr << "But I'll try to print the rest of the message." << endl;
          } else {
            trio_snprintf(buf, size+1, "%i", m_line);
            m_msg += buf;
            delete[] buf;
          }
        }
      }
      if(m_type.showFunction() || m_type.showWhere()){
        m_msg += ">";
      }
      m_msg += ' ';
    } else {
      m_msg += ':';
    }
  }
}
avdMessageClosureBase &avdMessageClosure::operator<<(char c)
{ prefix(); m_msg += c; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(unsigned char c)
{ prefix(); m_msg += c; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(const char *s)
{ prefix(); m_msg += s; return *this; }

//avdMessageClosureBase &avdMessageClosure::operator<<(int i)
//{ return va("%d", i); }
//avdMessageClosureBase &avdMessageClosure::operator<<(unsigned int i)
//{ return va("%u", i); }
//avdMessageClosureBase &avdMessageClosure::operator<<(long i)
//{ return va("%ld", i); }
//avdMessageClosureBase &avdMessageClosure::operator<<(unsigned long i)
//{ return va("%lu", i); }
//avdMessageClosureBase &avdMessageClosure::operator<<(float f)
//{ return va("%g", f); }
//avdMessageClosureBase &avdMessageClosure::operator<<(double f)
//{ return va("%g", f); }
//avdMessageClosureBase &avdMessageClosure::operator<<(const void *p)
//{ return va("%p", p); }

avdMessageClosureBase &avdMessageClosure::operator<<(int i)
{ char buf[32]; trio_snprintf(buf, 32, "%d", i); m_msg += buf; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(unsigned int i)
{ char buf[32]; trio_snprintf(buf, 32, "%u", i); m_msg += buf; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(long i)
{ char buf[32]; trio_snprintf(buf, 32, "%ld", i); m_msg += buf; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(unsigned long i)
{ char buf[32]; trio_snprintf(buf, 32, "%lu", i); m_msg += buf; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(float f)
{ char buf[32]; trio_snprintf(buf, 32, "%g", f); m_msg += buf; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(double f)
{ char buf[32]; trio_snprintf(buf, 32, "%g", f); m_msg += buf; return *this; }
avdMessageClosureBase &avdMessageClosure::operator<<(const void *p)
{ char buf[32]; trio_snprintf(buf, 32, "%p", p); m_msg += buf; return *this; }

avdMessageClosureBase &avdMessageClosure::va(const char *fmt, ...)
{
  if(!fmt){
    cerr << "* Error in message display system in *" << endl;
    cerr << "* avdMessageClosure &avdMessageClosure::va(const char *, ...) *" << endl;
    cerr << endl;
    cerr << "I'm trying to prepare part of a message for you, but I" << endl; 
    cerr << "nonsensical directions for printing this part of the message." << endl;
    cerr << "But I'll try to print the rest of the message." << endl;

    return *this;
  }
  va_list ap;

  va_start(ap, fmt);
  int size = trio_vsnprintf((char *) 0, 0, fmt, ap);
  va_end(ap);

  char *buf = new char[size + 1];
  //assert(buf != 0);

  if(!buf){
    cerr << "* Error in message display system in *" << endl;
    cerr << "* avdMessageClosure &avdMessageClosure::va(const char *, ...) *" << endl;
    cerr << endl;
    cerr << "I'm trying to prepare part of a message for you, but I can't" << endl; 
    cerr << "allocate enough space to print this part of the message." << endl;
    cerr << "But I'll try to print the rest of the message." << endl;
  } else {
    va_start(ap, fmt);
    trio_vsnprintf(buf, size+1, fmt, ap);
    va_end(ap);

    prefix();
    m_msg += buf;
    delete[] buf;
  }
  return *this;
}
