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



/*
  It'd be nice to encapsulate this into a RunTime class.  Unfortunately, the actual
  RuntimeLoop function needs to be exposed to emscripten and cannot be a member method.
  So, we're going to do this the old fashioned way and play pass the driver while
  exposing some potentially dangerous retrieval functions
  on the driver object to delete it... through deleting its cWorld!
  
  The driver itself contains all information about a single experiment.  It is a little
  strange, though, since the driver moves the world but the driver also contains the world
  and the world, the driver.  It's best to think of Creating/Resetting/Deleting the Driver
  in this header as doing both at the same time to the world and the driver.  
  
  NEVER delete the driver object directly.  ALWAYS delete it through DeleteDriver().
  
  This is because the cWorld destructor actually deletes our driver.  They refer
  to one another, and this is the traditional behavior that I don't want to muck
  with.
*/

namespace Avida {
  namespace WebViewer {
  
  
        
    /*
      Periodically, the RuntimeLoop will check for messages sent from
      different Worker threads.  These messages are sent to the
      driver for processing.  
    */
    void CheckMessages(Driver* driver)
    {
      EMStringPtr msg_buf = GetMessages();
      json msgs = json::parse( (char*) msg_buf);
      std::free( (void*) msg_buf);  //Cleanup JS allocation
      for (auto msg : msgs){
        driver->ProcessMessage(msg);
      }
    }
    
    
    /*
      Setup the driver/world with a particular set of configuration options.
    */
    Driver* SetupDriver(DriverConfig* cfg, Apto::Map<Apto::String, Apto::String>* defs = nullptr)
    {
        //new_world, feedback, and the driver are all deleted when the cWorld object is deleted.
        World* new_world = new World;
        cUserFeedback feedback;
        cWorld* world = cWorld::Initialize(cfg->GetConfig(), cfg->GetWorkingDir(), new_world, &feedback, defs);
        D_(D_STATUS, "The world is located at " << world);
        Driver* driver = new Driver(world, feedback);  //The driver and world register each other
        D_(D_STATUS, "The driver is located at " << &driver);
        return driver;
    }
    
    
    
    /*
      Setup the driver using the default packaged settings.
      This will in turn create the world and by agreement set responsbility
      for deleting the driver to the cWorld object. 
    */
    Driver* CreateDefaultDriver(int argc, char* argv[])
    {
      cAvidaConfig* cfg = new cAvidaConfig();
      cUserFeedback* feedback = new cUserFeedback;
      
      Apto::Map<Apto::String, Apto::String> defs;
      Avida::Util::ProcessCmdLineArgs(argc, argv, cfg, defs);
      
      DriverConfig* d_cfg = new DriverConfig(cfg,"/");
      Driver* driver = SetupDriver(d_cfg, &defs);
      delete d_cfg;
      
      D_(D_STATUS, "Avida is now configured with default settings.");
      return driver;
    }
    
    
    
    /*
      We'll let the world clean up the driver.
      DON'T directly delete the driver.  You'll
      crash my runtime.
    */
    void DeleteDriver(Driver*& driver)
    {
      delete driver->GetWorld();
      driver = nullptr;
    }
    
    
    /*
      Return a new driver (and delete the old) with new settings.
      This should only be done when configuration settings like
      the size of the world or number of tasks (not number of rewarded
      tasks) changes.  This is because there are a lot of data structures
      that rely on these objects and it's just easier to reinitialize than
      try to modify the core to realize data should be resized or invalidated.
    */
    Driver* DriverReset(Driver* driver)
    {
      D_(D_FLOW | D_STATUS, "Resetting driver.");
      WebViewerMsg msg_reset = FeedbackMessage(Feedback::NOTIFICATION);
      msg_reset["description"] = "The Avida driver is resetting";
      DriverConfig* d_cfg = driver->GetNextConfig();
      DeleteDriver(driver);
      return SetupDriver(d_cfg);
    }
  
  
    /*
      Clean up after ourselves before we return back to main to exit
      the program.
    */
    void AvidaExit(Driver* driver)
    {
      D_(D_FLOW | D_STATUS, "The Avida runtime is termiating.");
      WebViewerMsg msg_exit = FeedbackMessage(Feedback::FATAL);
      msg_exit["description"] = "Avida is exiting";
      PostMessage(msg_exit);
      DeleteDriver(driver);
    }
    
    /*
      We should be posting a message and setting state information
      in the JS runtime when we're transitioning between runtime states
      or when we're resetting the driver.
    */
    void NotifyDriverPaused()
    {
      D_(D_STATUS, "Driver is paused.");
    }
    
    void NotifyDriverRunning()
    {
      D_(D_STATUS, "Driver is running.");
    }
    
    void NotifyDriverResetting()
    {
      D_(D_STATUS, "Driver is resetting.");
    }
    
    void NotifyExitingRuntime()
    {
    }
    
    
    
    /*
      I need to expose this function using C-style linkage
      so I can whitelist it for emterpretifying.  The RuntimeLoop
      handles the logic of transitioning between paused and running
      states; when to check for messages; and when to sleep to
      let our browser do its own processing.  It also handles
      setting up our world/driver, resetting our world/driver, and
      preparing to exit the program.
    */
    extern "C"
    void RuntimeLoop(int argc, char* argv[])
    {
      /*
        When Avida starts, we won't know what the first set of
        actual experimental settings are (e.g. grid size, tasks,
        mutation rate, etc.)... but we do need to make sure that
        the AvidaEd UI is able to handle things like tracing 
        organisms.  One way to do this is to create a driver
        that uses a default set of configurations.  Most configuration
        settings in AvidaED will not result in the driver being completely
        reset, anyhow.  Presently, only changing the grid size
        will require the driver to be reset.  Everything else can
        simply modify the configuration object or alter the environment
        rewards.  We will let the driver, though, decide when it
        wants to be reincarnated with completely new settings and events.
      */
      NotifyDriverResetting();
      Driver* driver = CreateDefaultDriver(argc, argv);
      
      D_(D_FLOW | D_STATUS, "Entering runtime loop");
      
      /* Any call to CheckMessage could end up forcing us
        to delete the cWorld object (and the driver, feedback, etc.)
        objects.  Consequently, we always have to check to see
        if our driver still exists before polling its state.  It
        should *never* be nullptr unless something went wrong with
        creating the driver.
      */
      while(driver && !driver->IsFinished()){
        
        //Begin with the driver in a paused state.
        //Messages can still be received periodically.
        bool first_pass = true;
        while(driver && 
              driver->IsPaused() && 
              !driver->DoReset() ){
          if (first_pass){
            NotifyDriverPaused();
            first_pass = false;
          }
          emscripten_sleep(100);
          CheckMessages(driver);
        } //End paused loop
        
        //Move from paused state to running state.
        //We will transition back to the paused state
        //when we get the appropriate message or exit.
        //Note that a reset will also shove us into a paused
        //state.
        first_pass = true;
        while( driver && 
               !( driver->IsFinished() && 
                  driver->IsPaused() &&
                  driver->DoReset()) ) {
          if (first_pass){
            NotifyDriverRunning();
            first_pass = false;
          }
          driver->StepUpdate();
          emscripten_sleep(100);
          CheckMessages(driver);
        } // End step update loop
        
        //Reset requests require us to destroy the current world and driver
        //and initialize a new one before we proceed.  This will clear
        //all persistant data like events and stats.
        if (driver && driver->DoReset()){
          NotifyDriverResetting();
          driver = DriverReset(driver);
        }
      } // End driver available and not finished loop
      
      //Time to exit our runtime; cleanup after ourselves.
      if (driver){
        NotifyExitingRuntime();
        AvidaExit(driver);
      } else {
        cerr << "We should never be here.  The driver was unavailable during runtime loop." << endl;
      }
    } // End RuntimeLoop
  } //End WebViewer namespace
} //End Avida namespace

#endif
