#ifndef QSTRING_H
#include <qstring.h>
#endif

#ifndef AVD_MESSAGE_DISPLAY_HH
#include "avd_message_display.hh"
#endif

avdMessageClosure &avdMessageClosure::operator<<(const QString &s){
  prefix();
  m_msg += (QString::null == s)?("(null)"):(s.latin1());
  return *this;
}

// arch-tag: implementation file for debug-message display of QStrings
