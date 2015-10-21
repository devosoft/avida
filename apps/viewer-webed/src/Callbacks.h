//
//  Callbacks.h
//  viewer-webed
//
//  Created by Matthew Rupp on 9/3/15.
//  Copyright (c) 2015 MSU. All rights reserved.


#ifndef viewer_webed_Callbacks_h
#define viewer_webed_Callbacks_h

#include "Driver.h"
#include <cstdlib>


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
      D_(D_FLOW | D_STATUS, "Entering runtime loop");
      PostMessage(MSG_READY);
      while(!driver->IsFinished()){
        D_(D_STATUS, "Driver is active.");
        bool first_pass = true;
        while(driver->IsPaused()){
          if (first_pass){
            D_(D_STATUS, "Driver is paused.");
            first_pass = false;
          }
          emscripten_sleep(100);
          CheckMessages();
        }
        first_pass = true;
        while(!(driver->IsFinished() && driver->IsPaused())){
          if (first_pass){
            D_(D_STATUS, "Driver is running.");
            first_pass = false;
          }
          driver->StepUpdate();
          emscripten_sleep(100);
          CheckMessages();
        }
      }
    }
  
    void AvidaExit()
    {
      D_(D_FLOW | D_STATUS, "The Avida runtime is termiating.");
      WebViewerMsg msg_exit = FeedbackMessage(Feedback::FATAL);
      msg_exit["description"] = "Avida is exiting";
      PostMessage(msg_exit);
    }

  }
}

#endif
