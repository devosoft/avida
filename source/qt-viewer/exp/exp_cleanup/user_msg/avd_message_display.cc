#ifndef _LIMITS_H___
#include <limits.h>
#endif
#ifndef _STDIO_H_
#include <stdio.h>
#endif
#ifndef _STDARG_H
#include <stdarg.h>
#endif

#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "avd_message_display_typetrack.hh"
#endif

#ifndef TRIO_TRIO_H
#include "third-party/trio/trio.h"
#endif

#ifndef AVD_MESSAGE_DISPLAY_HH
#include "avd_message_display.hh"
#endif

using namespace std;


avdDefaultMessageDisplay s_avd_info_msg_out(&cout);
avdDefaultMessageDisplay s_avd_debug_msg_out(&cout);
avdDefaultMessageDisplay s_avd_error_msg_out(&cerr);
avdDefaultMessageDisplay s_avd_fatal_msg_out(&cerr);
avdDefaultMessageDisplay s_avd_plain_msg_out(&cout);
avdMessageDisplay *s_avd_info_display(&s_avd_info_msg_out);
avdMessageDisplay *s_avd_debug_display(&s_avd_debug_msg_out);
avdMessageDisplay *s_avd_error_display(&s_avd_error_msg_out);
avdMessageDisplay *s_avd_fatal_display(&s_avd_fatal_msg_out);
avdMessageDisplay *s_avd_plain_display(&s_avd_plain_msg_out);
avdMessageClass   avdMCInfo("Info:",  &s_avd_info_display, false, false, false);
avdMessageClass avdMCDebug("Debug:", &s_avd_debug_display, false, false, false);
avdMessageClass avdMCError("Error:", &s_avd_error_display, false, true, false);
avdMessageClass avdMCFatal("Fatal:", &s_avd_fatal_display, true, true, false);
avdMessageClass avdMCPlain("Plain:", &s_avd_plain_display, false, true, true);
avdMessageType  Info_GEN_avdMsg("General", avdMCInfo);
avdMessageType Debug_GEN_avdMsg("General", avdMCDebug);
avdMessageType Error_GEN_avdMsg("General", avdMCError);
avdMessageType Fatal_GEN_avdMsg("General", avdMCFatal);
avdMessageType Plain_avdMsg("General", avdMCPlain);
void setAvdInfoDisplay(avdMessageDisplay &md) { s_avd_info_display = &md; }
void setAvdDebugDisplay(avdMessageDisplay &md) { s_avd_debug_display = &md; }
void setAvdErrorDisplay(avdMessageDisplay &md) { s_avd_error_display = &md; }
void setAvdFatalDisplay(avdMessageDisplay &md) { s_avd_fatal_display = &md; }
void setAvdPlainDisplay(avdMessageDisplay &md) { s_avd_plain_display = &md; }

/*
A private namespace containing load-time initializations; these register
the default error types with the avdMessageSetTracker singleton.  (The
avdMessageSetTracker singleton is used in the GUI by the log window to
keep track of how messages of a given type should be displayed).
*/
namespace {
  class DebugMsgTypeSetRegistration {
  protected: avdMessageTypeSet *m_msg_types;
  public:
    DebugMsgTypeSetRegistration():m_msg_types(0){
      if((m_msg_types = new avdMessageTypeSet)){
        m_msg_types->Register("Plain",   &Plain_avdMsg);
        m_msg_types->Register("Info",   &Info_GEN_avdMsg);
        m_msg_types->Register("Debug", &Debug_GEN_avdMsg);
        m_msg_types->Register("Error", &Error_GEN_avdMsg);
        m_msg_types->Register("Fatal", &Fatal_GEN_avdMsg);
        avdMessageSetTracker::Instance().Register("General", m_msg_types);
      } else {
        cerr << "* Error in message display system *" << endl;
        cerr << "* in class DebugMsgTypeSetRegistration constructor *" << endl;
        cerr << endl;
        cerr << "Couldn't allocate \"avdMessageTypeSet\" object for use in" << endl;
        cerr << "registering general message types \"Plain\", \"Info\"," << endl;
        cerr << "\"Debug\", \"Error\", and \"Fatal\" with the message-type" << endl;
        cerr << "tracking system." << endl;
        cerr << "Normally the message-type tracking system is used by a log" << endl;
        cerr << "window, available in the graphic user interface, to" << endl;
        cerr << "determine how messages of a given type should be" << endl;
        cerr << "displayed to the user." << endl;
        cerr << endl;
      }
    }
    ~DebugMsgTypeSetRegistration(){
      avdMessageSetTracker::Instance().Unregister("General");
      if(m_msg_types) delete m_msg_types;
  } };
  const DebugMsgTypeSetRegistration s_registration;
}

void avdDefaultMessageDisplay::out(const cString &final_msg){
  if(m_out){
    *m_out << final_msg << endl; m_out->flush();
  } else {
    cerr << "* Error in message display system in method *" << endl;
    cerr << "* \"void avdDefaultMessageDisplay::out(const cString &)\" *" << endl;
    cerr << endl;
    cerr << "Trying to display a message to the user, but my pointer to" << endl;
    cerr << "the default output device doesn't point anywhere.  So I'm" << endl;
    cerr << "going to send the message to the \"standard error\" device." << endl;
    cerr << "Here's the message:" << endl;
    cerr << endl;
    cerr << final_msg << endl;
    cerr << endl;
  }
}
void avdMessageClass::configure(avdMessageType *message_type){
  if(!message_type){
    cerr << "* Error in message display system in method *" << endl;
    cerr << "* void avdMessageClass::configure(avdMessageType *) *" << endl;
    cerr << endl;
    cerr << "Trying to configure a message type, but the pointer" << endl;
    cerr << "I was given to the message type doesn't point anywhere." << endl;
  } else {
    message_type->m_is_active = true;
    message_type->m_show_where = true;
    message_type->m_show_function = true;
  }
}
avdMessageType::avdMessageType(
  const char *type_name,
  avdMessageClass &message_class
):m_type_name(type_name),
  m_message_class(message_class)
{ message_class.configure(this); }
avdMessageClosure::avdMessageClosure(
  avdMessageType &type,
  const char *function_name,
  const char *file_name,
  int line_number
):m_type(type),
  m_function(function_name),
  m_file(file_name),
  m_line(line_number),
  m_error(INT_MAX),
  m_op_count(0)
{}
avdMessageClosure::~avdMessageClosure(void){
  if(!m_type.m_message_class.m_msg_display){
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
    if(m_type.m_message_class.m_is_fatal) abort();
  } else {
    (*m_type.m_message_class.m_msg_display)->out(m_msg);
    if(m_type.m_message_class.m_is_fatal) (*m_type.m_message_class.m_msg_display)->abort();
  }
}
void avdMessageClosure::prefix(void){
  if(m_op_count++ == 0 && !m_type.m_message_class.m_no_prefix){
    if(m_type.m_message_class.m_is_prefix){
      m_msg += m_type.m_type_name;
      m_msg += '-';
    }
    m_msg += m_type.m_message_class.m_class_name;
    if(!m_type.m_message_class.m_is_prefix
      ||m_type.m_show_function
      ||m_type.m_show_where
    ){
      bool colon = false;
      if(!m_type.m_message_class.m_is_prefix){
        m_msg += m_type.m_type_name;
        m_msg += ": ";
      }
      if(m_type.m_show_function || m_type.m_show_where){
        m_msg += "<";
      }
      if(m_type.m_show_function){
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
      if(m_type.m_show_where){
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
      if(m_type.m_show_function || m_type.m_show_where){
        m_msg += ">";
      }
      m_msg += ' ';
    } else {
      m_msg += ':';
    }
  }
}
avdMessageClosure &avdMessageClosure::operator<<(char c)
{ prefix(); m_msg += c; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(unsigned char c)
{ prefix(); m_msg += c; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(const char *s)
{ prefix(); m_msg += s; return *this; }

//avdMessageClosure &avdMessageClosure::operator<<(int i)
//{ return va("%d", i); }
//avdMessageClosure &avdMessageClosure::operator<<(unsigned int i)
//{ return va("%u", i); }
//avdMessageClosure &avdMessageClosure::operator<<(long i)
//{ return va("%ld", i); }
//avdMessageClosure &avdMessageClosure::operator<<(unsigned long i)
//{ return va("%lu", i); }
//avdMessageClosure &avdMessageClosure::operator<<(float f)
//{ return va("%g", f); }
//avdMessageClosure &avdMessageClosure::operator<<(double f)
//{ return va("%g", f); }
//avdMessageClosure &avdMessageClosure::operator<<(const void *p)
//{ return va("%p", p); }

avdMessageClosure &avdMessageClosure::operator<<(int i)
{ char buf[32]; trio_snprintf(buf, 32, "%d", i); m_msg += buf; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(unsigned int i)
{ char buf[32]; trio_snprintf(buf, 32, "%u", i); m_msg += buf; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(long i)
{ char buf[32]; trio_snprintf(buf, 32, "%ld", i); m_msg += buf; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(unsigned long i)
{ char buf[32]; trio_snprintf(buf, 32, "%lu", i); m_msg += buf; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(float f)
{ char buf[32]; trio_snprintf(buf, 32, "%g", f); m_msg += buf; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(double f)
{ char buf[32]; trio_snprintf(buf, 32, "%g", f); m_msg += buf; return *this; }
avdMessageClosure &avdMessageClosure::operator<<(const void *p)
{ char buf[32]; trio_snprintf(buf, 32, "%p", p); m_msg += buf; return *this; }

avdMessageClosure &avdMessageClosure::va(const char *fmt, ...)
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

// arch-tag: implementation file for debug-message display handling
