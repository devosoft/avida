//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef DEFAULT_MESSAGE_DISPLAY_HH
#include "cDefaultMessageDisplay.h"
#endif

#ifndef STRING_HH
#include "cString.h"
#endif

using namespace std;

void cDefaultMessageDisplay::out(cString &final_msg)
{ *_out << final_msg << endl; _out->flush(); }

/*
default instantiations of cDefaultMessageDisplay...
*/
cDefaultMessageDisplay s_info_msg_cout(&cout);
cDefaultMessageDisplay s_debug_msg_cerr(&cerr);
cDefaultMessageDisplay s_error_msg_cerr(&cerr);
cDefaultMessageDisplay s_fatal_msg_cerr(&cerr);
cDefaultMessageDisplay s_noprefix_msg_cout(&cout);

