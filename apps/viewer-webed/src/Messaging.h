#ifndef _Avida_Webviewer_Messaging_h
#define _Avida_Webviewer_Messaging_h

#include "json.hpp"
#include <string>
#include <cstdint>
#include <string>
#include <emscripten.h>
#include "WebDebug.h"

using json = nlohmann::json;

namespace Avida{
  namespace WebViewer{
    
    typedef std::uint32_t EMStringPtr;
    typedef nlohmann::json WebViewerMsg;
    typedef char* ReceivedMessage;
    
    namespace Feedback{
      typedef enum FEEDBACK_TYPE {FATAL, WARNING, NOTIFICATION, UNKNOWN} FEEDBACK_TYPE;
    }
    
    const WebViewerMsg MSG_READY = {
      {"type", "userFeedback"},
      {"level", "notification"},
      {"message", "ready"}
    };
    
    constexpr vector<string> EVENT_PROPERTIES = 
      {"type", "name", "triggerType", "start", "interval", "end", "singleton"};
    
    
    
    extern "C"
    EMStringPtr GetMessages()
    {
      return EM_ASM_INT_V(return doGetMessages());
    }
    
    
    void PostMessage(const WebViewerMsg& msg)
    {
      D_(D_FLOW | D_MSG_OUT, "About to post message" << msg);
      D_(D_MSG_OUT, "Message is: " << msg, 1);
      // We're enforcing a rule that all messages
      // sent or received must be JSON objects.
      // In reality, emscripten requires these
      // messages must be strings or numbers
      // We're defaulting to strings only.
      EM_ASM_ARGS(
                  {
                    doPostMessage(Pointer_stringify($0));
                  }, 
                    msg.dump().c_str()
                  );
      D_(D_FLOW | D_MSG_OUT, "Done posting message.");
    }
    
    
    WebViewerMsg FeedbackMessage(Feedback::FEEDBACK_TYPE err)
    {
      WebViewerMsg ret = {{ "type","userFeedback" }};
      switch(err){
        case Feedback::FATAL:
          ret["level"] = "fatal";
          break;
        case Feedback::WARNING:
          ret["level"] = "warning";
          break;
        case Feedback::NOTIFICATION:
          ret["level"] = "notification";
          break;
        case Feedback::UNKNOWN:
        default:
          ret["level"] = "unknown";
          break;
      }
      return ret;
    }
    
    WebViewerMsg ReturnMessage(const WebViewerMsg& received)
    {
      WebViewerMsg return_msg;
      return_msg["type"] = "response";
      return_msg["request"] = received;
      return return_msg;
    }
    
    
    WebViewerMsg DefaultAddEventMessage(const WebViewerMsg& received)
    {
      D_(D_FLOW | D_MSG_IN, "Defaulting message " << received);
      WebViewerMsg return_msg(received);
      
      return_msg["triggerType"] =
        (received.count("triggerType")) ? received["triggerType"] : "update";
      
      if (return_msg["triggerType"] == "i" || return_msg["triggerType"] == "immediate")
        return return_msg;
      
      return_msg["start"] = 
        (received.count("start")) ? received["start"] : "now";
      return_msg["interval"] = 
        (received.count("interval")) ? received["interval"] : "always";
      return_msg["end"] = 
        (received.count("end")) ? received["end"] : "";
      return_msg["singleton"] =
        (received.count("singleton")) ? received["singleton"] : json(false);
      
      D_(D_FLOW | D_MSG_IN, "Done defaulting message.");
      return return_msg;
    }
    
    
    string DeQuote(const string& str_in)
    {
      ostringstream oss;
      for (char c : str_in)
        if (c != '"' && c != '\'')
          oss << c;
      return oss.str();
    }
    
    
    json StripProperties(const json& j, const vector<string>& excl_props)
    {
      json n;
      for (auto it = j.begin(); it != j.end(); ++it){
        if (excl_props.find(j.key() != excl_props.end())
          continue;
        n[j.key()] = j.value();
      }
      reuturn n;
    }
    
    bool contains(const json& j, const string& p)
    {
      return (j.find(p) != j.end());
    }
    
  }  //WebViewer
}  //Avida

#endif

