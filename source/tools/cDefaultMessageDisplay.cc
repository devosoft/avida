/*
 *  cDefaultMessageDisplay.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cDefaultMessageDisplay.h"

#include "cString.h"

using namespace std;


void cDefaultMessageDisplay::out(cString& final_msg) { *_out << final_msg << endl; _out->flush(); }

/*
default instantiations of cDefaultMessageDisplay...
*/
cDefaultMessageDisplay s_info_msg_cout(&cout);
cDefaultMessageDisplay s_debug_msg_cerr(&cerr);
cDefaultMessageDisplay s_error_msg_cerr(&cerr);
cDefaultMessageDisplay s_fatal_msg_cerr(&cerr);
cDefaultMessageDisplay s_noprefix_msg_cout(&cout);

