//
//  DriverConfig.h
//  viewer-webed
//
//  Created by Matthew Rupp on 11/2/15.
//  Copyright (c) 2015 MSU. All rights reserved.
//

#ifndef viewer_webed_DriverConfig_h
#define viewer_webed_DriverConfig_h

#include <string>

namespace Avida {
  namespace WebViewer {

    /*
      Just a quick class to store and retrieve settings for a world.
      The driver and CreateDefaultDriver method use this.  The cAvidaConfig
      pointer will be passed to a world object which will then delete it
      once the world has run its course.
    */
    class DriverConfig  
    {
      protected:
        cAvidaConfig* cfg;
        string working_dir;
      
      public:
        DriverConfig(cAvidaConfig* acfg, string dir) : cfg(acfg), working_dir(dir) {}
        cAvidaConfig* GetConfig() { return cfg; }
        const char* GetWorkingDir() { return working_dir.c_str(); };
    };
    


  };
};

#endif
