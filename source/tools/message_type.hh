//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_TYPE_HH
#define MESSAGE_TYPE_HH

class cMessageClass;

class cMessageType{
public:
  cMessageType(
    const char *type_name,
    cMessageClass &message_class
  );
public:
  const char *m_type_name;
  const cMessageClass &m_message_class;
  bool m_is_active;
};

#endif
