//
//  WebDebug.h
//  viewer-webed
//
//  Created by Matthew Rupp on 2/29/16.
//  Copyright © 2016 MSU. All rights reserved.
//

#ifndef WebDebug_h
#define WebDebug_h

#include <emscripten.h>
#include <iostream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

//If we're sending it as a stream, which stream should we use?
#define DEBUG_STREAM std::cerr

//Should we send debug information through the worker messaging interface?
#define D_SEND_JSON 0

//How detailed should our debug output be
#define D_VERBOSITY 0

//Constants to define different types of debugging
#define D_FLOW 1
#define D_MSG_IN 2
#define D_MSG_OUT 4
#define D_STATUS 8
#define D_EVENTS 16
#define D_ACTIONS 32
#define D_ERROR 64

////What should we be outputting for debug purposes?


//#define DEBUG_MODE \
//  (D_FLOW | D_MSG_IN | D_MSG_OUT | D_STATUS | D_EVENTS | D_ACTIONS | D_ERROR)

//#define DEBUG_MODE ( D_MSG_IN | D_STATUS | D_ACTIONS | D_EVENTS )

#define DEBUG_MODE 0

#ifdef NDEBUG 

  #define D_(...) do{}while(0);

#else

  #define D_0(MODE, MSG) D_1(MODE, MSG, 0)

  #define D_1(MODE, MSG, VERBOSITY)\
  do{\
    if ( ( (MODE) & (DEBUG_MODE) ) && (VERBOSITY <= D_VERBOSITY) )\
    {\
      if ( !D_SEND_JSON ){\
        DEBUG_STREAM << std::endl << "[AVIDA] " << MSG << std::endl;\
      }\
    }\
  } while(0);
  
  #define D_SELECT(_0, _1, FUNC, ...) FUNC
  
  #define D_(MODE, MSG, ...) D_SELECT(_0, ##__VA_ARGS__, D_1, D_0)(MODE, MSG, ##__VA_ARGS__)


#endif  //NDEBUG

#endif /* WebDebug_h */
