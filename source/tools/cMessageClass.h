//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_CLASS_HH
#define MESSAGE_CLASS_HH

class cMessageDisplay;
class cMessageType;

class cMessageClass{
public:
  cMessageClass(
    const char *class_name,
    cMessageDisplay **msg_display,
    bool is_fatal,
    bool is_prefix,
    bool no_prefix
  );
public:
  void configure(cMessageType *message_type);
public:
  const char *const m_class_name;
  cMessageDisplay **m_msg_display;
  bool const m_is_fatal;
  bool const m_is_prefix;
  bool const m_no_prefix;
private:
  bool _configured;
};

/*
Declaration of the five message classes.
*/
extern cMessageClass MCInfo;
extern cMessageClass MCDebug;
extern cMessageClass MCError;
extern cMessageClass MCFatal;
extern cMessageClass MCNoPrefix;

#endif
