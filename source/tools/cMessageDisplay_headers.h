/*
 *  cMessageDisplay_headers.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
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
