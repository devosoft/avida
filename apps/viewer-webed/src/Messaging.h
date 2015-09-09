#ifndef _Avida_Webviewer_Messaging_h
#define _Avida_Webviewer_Messaging_h

#include "json.hpp"
#include <string>
#include <cstdint>
#include <string>
#include <emscripten.h>


namespace Avida{
  namespace WebViewer{
    
    typedef std::int8_t EMStringPtr;
    typedef nlohmann::json WebViewerMsg;
    typedef char* ReceivedMessage;
    
    const WebViewerMsg MSG_READY = {
      {"Key", "AvidaStatus"},
      {"Status", "Paused"}
    };
    
    
    extern "C"
    void CheckMessages()
    {
      // Call getMessages on the JS side
      EM_ASM(getMessages());
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
    
    
    WebViewerMsg ReturnMessage(const WebViewerMsg& received)
    {
      WebViewerMsg return_msg;
      return_msg["Key"] = received["Key"];
      return_msg["Received"] = received;
      return return_msg;
    }
    
  }  //WebViewer
}  //Avida

#endif

