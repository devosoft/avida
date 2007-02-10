/*
 *  cDefaultMessageDisplay.cc
 *  Avida
 *
 *  Called "default_message_display.cc" prior to 12/7/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

