/*
 *  cMessageDisplay_headers.h
 *  Avida
 *
 *  Called "message_display_headers.hh" prior to 12/7/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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

#ifndef cMessageDisplay_headers_h
#define cMessageDisplay_headers_h

#include <iostream>

#ifndef cDefaultMessageDisplay_h
#include "cDefaultMessageDisplay.h"
#endif
#ifndef cMessageClass_h
#include "cMessageClass.h"
#endif
#ifndef cMessageClosure_h
#include "cMessageClosure.h"
#endif
#ifndef cMessageDisplay_h
#include "cMessageDisplay.h"
#endif
#ifndef cMessageType_h
#include "cMessageType.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

extern cMessageType NoPrefix_Msg;
extern cMessageType Info_GEN_Msg;
extern cMessageType Debug_GEN_Msg;
extern cMessageType Error_GEN_Msg;
extern cMessageType Fatal_GEN_Msg;
#define Message _PLAIN_MSG(NoPrefix)
#define GenInfo _INFO_MSG(GEN)
#define GenDebug _DEBUG_MSG(GEN)
#define GenError _ERROR_MSG(GEN)
#define GenFatal _FATAL_MSG(GEN)

#endif
