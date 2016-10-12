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
#define DEBUG_STREAM std::cerr

//Should we send debug information through the worker messaging interface?
#define D_SEND_JSON 1

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


//Some variables to measure performance
#define D_MEASURE_PERFORMANCE 0
#define D_RUN_TIL_UPDATE 2500

////What should we be outputting for debug purposes?
//#define DEBUG_MODE \
//  (D_FLOW | D_MSG_IN | D_MSG_OUT | D_STATUS | D_EVENTS | D_ACTIONS | D_ERROR)

#define DEBUG_MODE D_STATUS


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
