#ifndef AVD_MESSAGE_DISPLAY_HH
#include "avd_message_display.hh"
#endif

#ifndef AVD_MESSAGE_DISPLAY_BASE_HH
#include "avd_message_display_base.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif

#ifndef _CPP_IOSTREAM
#include <iostream>
#endif

using namespace std;

avdMessageDisplayBase *s_avd_info_display(&s_avd_info_msg_out);
avdMessageDisplayBase *s_avd_debug_display(&s_avd_debug_msg_out);
avdMessageDisplayBase *s_avd_error_display(&s_avd_error_msg_out);
avdMessageDisplayBase *s_avd_fatal_display(&s_avd_fatal_msg_out);
avdMessageDisplayBase *s_avd_plain_display(&s_avd_plain_msg_out);
void setAvdInfoDisplay(avdMessageDisplayBase &md) { s_avd_info_display = &md; }
void setAvdDebugDisplay(avdMessageDisplayBase &md) { s_avd_debug_display = &md; }
void setAvdErrorDisplay(avdMessageDisplayBase &md) { s_avd_error_display = &md; }
void setAvdFatalDisplay(avdMessageDisplayBase &md) { s_avd_fatal_display = &md; }
void setAvdPlainDisplay(avdMessageDisplayBase &md) { s_avd_plain_display = &md; }

void avdMessageDisplay::out(const cString &final_msg) const {
  if(m_out){
    *m_out << final_msg ; m_out->flush();
  } else {
    cerr << "* Error in message display system in method *" << endl;
    cerr << "* \"void avdMessageDisplay::out(const cString &)\" *" << endl;
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
avdMessageDisplay s_avd_info_msg_out(&cout);
avdMessageDisplay s_avd_debug_msg_out(&cout);
avdMessageDisplay s_avd_error_msg_out(&cerr);
avdMessageDisplay s_avd_fatal_msg_out(&cerr);
avdMessageDisplay s_avd_plain_msg_out(&cout);
