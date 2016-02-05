//
//  WebDebug.h
//  viewer-webed
//
//  Created by Matthew Rupp on 9/30/15.
//  Copyright (c) 2015 MSU. All rights reserved.
//

#ifndef viewer_webed_WebDebug_h
#define viewer_webed_WebDebug_h

#include <cassert>
#include <iostream>

//How detailed should our debug output be
#define D_VERBOSITY 1

//Constants to define different types of debugging
#define D_FLOW 1
#define D_MSG_IN 2
#define D_MSG_OUT 4
#define D_STATUS 8
#define D_EVENTS 16
#define D_ACTIONS 32
#define D_ERROR 64

////What should we be outputting for debug purposes?


//#define DEBUG_LEVEL \
//  (D_FLOW | D_MSG_IN | D_MSG_OUT | D_STATUS | D_EVENTS | D_ACTIONS | D_ERROR)

#define DEBUG_LEVEL (D_STATUS | D_MSG_IN)


#define DEBUG_STREAM std::cerr



#ifdef NDEBUG 

  #define D_(...) do{}while(0);

#else

  #define D_0(LEVEL, MSG) D_1(LEVEL, MSG, 0)

  #define D_1(LEVEL, MSG, VERBOSITY)\
  do{\
    if ( ( (LEVEL) & (DEBUG_LEVEL) ) && (VERBOSITY <= D_VERBOSITY) )\
    {\
      DEBUG_STREAM << "AVIDA: " << MSG << std::endl;\
    }\
  } while(0)
  
  #define D_SELECT(_0, _1, FUNC, ...) FUNC
  
  #define D_(LEVEL, MSG, ...) D_SELECT(_0, ##__VA_ARGS__, D_1, D_0)(LEVEL, MSG, ##__VA_ARGS__)


#endif  //NDEBUG

#endif  //GAURD
