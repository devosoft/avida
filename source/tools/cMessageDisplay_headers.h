#ifndef MESSAGE_DISPLAY_HDRS_HH
#define MESSAGE_DISPLAY_HDRS_HH

#ifndef _CPP_IOSTREAM
#include <iostream>
#endif
#ifndef DEFAULT_MESSAGE_DISPLAY_HH
#include "cDefaultMessageDisplay.h"
#endif
#ifndef MESSAGE_CLASS_HH
#include "cMessageClass.h"
#endif
#ifndef MESSAGE_CLOSURE_HH
#include "cMessageClosure.h"
#endif
#ifndef MESSAGE_DISPLAY_HH
#include "cMessageDisplay.h"
#endif
#ifndef MESSAGE_TYPE_HH
#include "cMessageType.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif

#ifndef MESSAGE_DISPLAY_HH
#include "cMessageDisplay.h"
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
