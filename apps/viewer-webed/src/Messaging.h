#ifndef _Avida_Webviewer_Messaging_h
#define _Avida_Webviewer_Messaging_h

#include "json.hpp"
#include <string>
#include <cstdint>
#include <string>
#include <emscripten.h>


namespace Avida{
  namespace WebViewer{
    
    typedef std::uint32_t EMStringPtr;
    typedef nlohmann::json WebViewerMsg;
    typedef char* ReceivedMessage;
    
    namespace Feedback{
      typedef enum ERROR_TYPE {FATAL, WARNING, NOTIFICATION, UNKNOWN} ERROR_TYPE;
    }
    
    const WebViewerMsg MSG_READY = {
      {"Key", "AvidaStatus"},
      {"Status", "Paused"}
    };
    
    
    
    extern "C"
    EMStringPtr GetMessages()
    {
      return EM_ASM_INT_V(return getMessages());
    }
    
    
    void PostMessage(const WebViewerMsg& msg)
    {
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
    }
    
    
    WebViewerMsg ErrorMessage(Feedback::ERROR_TYPE err)
    {
      WebViewerMsg ret = { "key","userFeedback" };
      switch(err){
        case Feedback::FATAL:
          ret["type"] = "fatal";
          break;
        case Feedback::WARNING:
          ret["type"] = "warning";
          break;
        case Feedback::NOTIFICATION:
          ret["type"] = "notification";
          break;
        case Feedback::UNKNOWN:
        default:
          ret["type"] = "unknown";
          break;
      }
      return ret;
    }
    
    WebViewerMsg ReturnMessage(const WebViewerMsg& received)
    {
      WebViewerMsg return_msg;
      return_msg["key"] = received["key"];
      return_msg["received"] = received;
      return return_msg;
    }
    
    
    WebViewerMsg DefaultAddEventMessage(const WebViewerMsg& received)
    {
      WebViewerMsg return_msg = received;
      return_msg["start"] = 
        (received.find("start") != received.end()) ? received["start"] : "now";
      return_msg["interval"] = 
        (received.find("interval") != received.end()) ? received["interval"] : "1.0";
      return_msg["end"] = 
        (received.find("end") != received.end()) ? received["end"] : "end";
      return return_msg;
    }
    
  }  //WebViewer
}  //Avida

#endif

