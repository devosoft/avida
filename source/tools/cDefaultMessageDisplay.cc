/*
 *  cDefaultMessageDisplay.cc
 *  Avida
 *
 *  Called "default_message_display.cc" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cDefaultMessageDisplay.h"

#include "cString.h"

using namespace std;


void cDefaultMessageDisplay::out(cString& final_msg) { *m_out << final_msg << endl; m_out->flush(); }

/*
default instantiations of cDefaultMessageDisplay...
*/
cDefaultMessageDisplay s_info_msg_cout(&cout);
cDefaultMessageDisplay s_debug_msg_cerr(&cerr);
cDefaultMessageDisplay s_error_msg_cerr(&cerr);
cDefaultMessageDisplay s_fatal_msg_cerr(&cerr);
cDefaultMessageDisplay s_noprefix_msg_cout(&cout);

