//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#include <qstring.h>


using namespace std;


cMessageClosure &
cMessageClosure::operator<<(const QString &s){
  prefix();
  if(QString::null == s){
    _msg += "(null)";
  } else {
    _msg += s.latin1();
  }
  return *this;
}
