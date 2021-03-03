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
#include <map>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;

//If we're sending it as a stream, which stream should we use?
// std::cerr shows up in javascript console
#define DEBUG_STREAM std::cerr

//Should we send debug information through the worker messaging interface?
// 0 send to DEBUG_STREAM shows up in json console in web browser.
// 1 send to messaging system with AEDBridge - will need to do more changes
#define D_SEND_JSON 0

//How detailed should our debug output be (sets level of detail)
// 0 minium amount of text
// 3 lots of text hard to wade through
#define D_VERBOSITY 0

//Constants to define different types of debugging
// D_FLOW: program internal flow (how did I get here?)
// D_MSG_IN: input messages
// D_MSG_OUT: output messages
// D_STATUS: avida-core system status (e.g. driver, world, etc.)
// D_EVENTS: relating to the *scheduling* of actions
// D_ACTIONS: relating to the actions themselves
// D_ERROR: generic error output (e.g. print something before a crash when debugging)
#define D_FLOW 1
#define D_MSG_IN 2
#define D_MSG_OUT 4
#define D_STATUS 8
#define D_EVENTS 16
#define D_ACTIONS 32
#define D_ERROR 64


//Some variables to measure performance; not used since handshaking started.
#define D_MEASURE_PERFORMANCE 0
#define D_RUN_TIL_UPDATE 2500

////What should we be outputting for debug purposes?
//#define DEBUG_MODE \
//  (D_FLOW | D_MSG_IN | D_MSG_OUT | D_STATUS | D_EVENTS | D_ACTIONS | D_ERROR)
//  (D_MSG_OUT | D_EVENTS | D_MSG_IN | D_STATUS | D_ACTIONS)

//#define DEBUG_MODE \
//  (D_FLOW | D_MSG_IN | D_MSG_OUT | D_STATUS | D_EVENTS | D_ACTIONS | D_ERROR)
// @WRE 2021-02-02 The following definition was used in debugging the
// globalResourceAmount dict in the populationStats message.
// #define DEBUG_MODE \
//  (D_MSG_IN | D_MSG_OUT | D_ERROR)

// this defines the level; when set to 0 no debug statements are sent
// edit the line above with the correct subset or-ed together to get what you want
// and comment out setting to zero.

// next line sets debug mode to 0 and eleminates our debug statements.
#define DEBUG_MODE 0

const std::map<unsigned, string> debug_modes = {
  {1,"FLOW"}, {2, "MSG_IN"}, {4, "MSG_OUT"}, {8, "STATUS"}, {16, "EVENTS"}, 
  {32, "ACTIONS"}, {64, "ERROR"}
};

vector<string> GetDebugModes(unsigned m)
{
  vector<string> rv;
  for (auto& kv : debug_modes){
    if (m & kv.first) rv.push_back(kv.second);
  }
  return rv;
}



#ifdef NDEBUG 

  #define D_(...) do{}while(0);

#else

  #define D_0(MODE, MSG) D_1(MODE, MSG, 0)

  #if D_SEND_JSON == 0
    #define D_1(MODE, MSG, VERBOSITY)\
    do{\
      if ( ( (MODE) & (DEBUG_MODE) ) && (VERBOSITY <= D_VERBOSITY) )\
      {\
        string fn(__FILE__);\
        size_t pos = fn.rfind("/");\
        fn = fn.substr(pos+1, string::npos);\
        DEBUG_STREAM << std::endl << "[AVIDA " << fn << "@" << __LINE__ << "]" << MSG << std::endl;\
      }\
    } while(0);

  #else
    #define D_1(MODE, MSG, VERBOSITY)\
    do{\
      if ( ( (MODE) & (DEBUG_MODE) ) && (VERBOSITY <= D_VERBOSITY) )\
      {\
        string fn(__FILE__);\
        size_t pos = fn.rfind("/");\
        fn = fn.substr(pos+1, string::npos);\
        std::ostringstream oss;\
        oss << MSG;\
        string s_msg = oss.str();\
        json j_msg = {\
          {"type","av_debug"},\
          {"msg",s_msg},\
          {"file",fn},\
          {"line_number",__LINE__},\
          {"function", __FUNCTION__},\
          {"verbosity", VERBOSITY},\
          {"mode", GetDebugModes(MODE)}\
        };\
        EM_ASM_ARGS({doPostMessage(Pointer_stringify($0));}, j_msg.dump().c_str());\
      }\
    \
    }while(0);

  #endif
  
  #define D_SELECT(_0, _1, FUNC, ...) FUNC
  
  #define D_(MODE, MSG, ...) D_SELECT(_0, ##__VA_ARGS__, D_1, D_0)(MODE, MSG, ##__VA_ARGS__)

#endif  //NDEBUG

#endif /* WebDebug_h */
