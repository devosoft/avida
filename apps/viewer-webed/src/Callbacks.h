//
//  Callbacks.h
//  viewer-webed
//
//  Created by Matthew Rupp on 9/3/15.
//  Copyright (c) 2015 MSU. All rights reserved.


#include "Messaging.h"
#include "Driver.h"

#ifndef viewer_webed_Callbacks_h
#define viewer_webed_Callbacks_h

extern Avida::WebViewer::Driver* driver;

namespace Avida {
  namespace WebViewer {
    /*
      This is a means of allowing JS to pass received
      messages into Avida.
    */
    extern "C"
    void ReceiveMessage(const ReceivedMessage msg)
    {
      driver->ProcessMessage(nlohmann::json::parse(msg));
    }
    
    
    /*
      This is a backup means of running the driver using
      a callback from JS.  The Emterpreterified (!)
      code does not require this callback to be exposed
      as the callstack is preserved to allow for message
      reception.
    */
    extern "C"
    void RunDriver()
    {
      driver->Run();
    }
  }
}

#endif
