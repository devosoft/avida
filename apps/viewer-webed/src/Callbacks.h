//
//  Callbacks.h
//  viewer-webed
//
//  Created by Matthew Rupp on 9/3/15.
//  Copyright (c) 2015 MSU. All rights reserved.


#include "Messaging.h"
#include "Driver.h"
#include <cstdlib>

#ifndef viewer_webed_Callbacks_h
#define viewer_webed_Callbacks_h

extern Avida::WebViewer::Driver* driver;

namespace Avida {
  namespace WebViewer {
    
    void CheckMessages()
    {
      EMStringPtr msg_buf = GetMessages();
      json msgs = nlohmann::json::parse( (char*) msg_buf);
      std::free( (void*) msg_buf);
      for (auto msg : msgs){
        driver->ProcessMessage(msg);
      }
    }
    
    /*
      I need to expose this function using C-style linkage
      so I can whitelist it for emterpretifying.
    */
    extern "C"
    void RunDriver()
    {
      PostMessage(MSG_READY);
      while(!driver->IsFinished()){
        while(driver->IsPaused()){
          emscripten_sleep(100);
          CheckMessages();
        }
        while(!(driver->IsFinished() && driver->IsPaused())){
          driver->StepUpdate();
          emscripten_sleep(100);
          CheckMessages();
        }
      }
    }
  
  
    void AvidaExit()
    {
      WebViewerMsg msg_exit = ErrorMessage(Feedback::FATAL);
      msg_exit["description"] = "Avida is exiting";
      PostMessage(msg_exit);
    }

  }
}

#endif
