//
//  Callbacks.h
//  viewer-webed
//
//  Created by Matthew Rupp on 9/3/15.
//  Copyright (c) 2015 MSU. All rights reserved.


#ifndef viewer_webed_Callbacks_h
#define viewer_webed_Callbacks_h

#include "Driver.h"
#include <fstream>
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
    
    void SyncFS(bool from_remote)
    {
      EM_ASM(
             FS.syncfs(false, function(err) {
        if (err) {
          console.log("Unable to sync FS");
        }
        else {
          console.log("Sync'd FS");
        }
      }
                       ));
    }
    
    void WriteTemp()
    {
      ofstream fot("/cwd/tmp.txt");
      fot << "The quick brown fox jumped over the lazy dog" << endl;
      SyncFS(false);
      fot.close();
    }
    
    void ReadTemp()
    {
      return;
      SyncFS(true);
      string path = "/ws/g4/entryname.txt";
      ifstream fin(path.c_str());
      string from_file;
      fin >> from_file;
      cerr << "Received: " << from_file << " from " << path << endl;
    }
    
    
    /*
     Setup the IDBFS (IndexedDB File System).
     This is meant as a temporary measure until we can get
     a full-browser pouchDB implementation working.
     */
    
    void SetupIDBFS()
    {
      EM_ASM(
             FS.mkdir('/cwd');
             FS.mount(IDBFS, {}, '/cwd');
             );
    }
    
    
    /*
     Periodically, the RuntimeLoop will check for messages sent from
     different Worker threads.  These messages are sent to the
     driver for processing.  
     */
    void CheckMessages(Driver* driver)
    {
      while(true){
        EMStringPtr msg_buf = GetMessage();
        if (msg_buf == (EMStringPtr) nullptr)  //If we are at the end of our queue (nullptr)
          return; 
        json msg = json::parse( (char*) msg_buf);  //Parse the message
        std::free( (void*) msg_buf);  //Cleanup JS allocation
        if (driver->ProcessMessage(msg))  //If true is returned, break and keep the queue intact
          return;
      } 
    }
    
    
    
    /*
     Setup the driver from a particular path containing configuration files.
     This will in turn create the world and by agreement set responsbility
     for deleting the driver to the cWorld object. 
     */
    Driver* SetupDriver(int argc, char* argv[], const string& path)
    {
      char cpath[256];
      string cfg_path = path + "/avida.cfg";
      ifstream fin(cfg_path.c_str());
      char line[256];
      while(fin.good() && !fin.eof()){
        fin.getline(line,256);
        cerr << "avida.cfg: " << line << endl;
      }
      fin.close();
      cAvidaConfig* cfg = new cAvidaConfig();
      
      if (Apto::FileSystem::ChDir(Apto::String(path.c_str()))){
        cerr << "Switched to directory: " << path << endl;
      } else {
        cerr << "Unable to switch to directory: " << path << endl;
        if (!Apto::FileSystem::ChDir(Apto::String("/"))){
          cerr << "FATAL ERROR: Cannot access /" << endl;
        }
      }
      Apto::Map<Apto::String, Apto::String> defs;
      Avida::Util::ProcessCmdLineArgs(argc, argv, cfg, defs);
      World* new_world = new World;
      cUserFeedback feedback;  //Temporary feedback object; messages from init will be copied into the driver
      
      cWorld* world = cWorld::Initialize(cfg, "/", new_world, &feedback, &defs);
      
      
      D_(D_STATUS, "The world is located at " << world);
      
      Driver* driver = new Driver(world, feedback);  //The driver and world register each other
      
      D_(D_STATUS, "The driver is located at " << driver);
      
      D_(D_STATUS, "Avida driver with configuration from " + path);
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
      if (driver){
               cerr << "About to delete driver:"  << driver 
               << " with world " << driver->GetWorld() 
               << " with grid "
               << driver->GetWorld()->GetConfig().WORLD_X.Get() 
               << " " 
               << driver->GetWorld()->GetConfig().WORLD_Y.Get()
               << std::endl;
      }
      D_(D_FLOW | D_STATUS, "Resetting driver.");
      WebViewerMsg msg_reset = FeedbackMessage(Feedback::NOTIFICATION);
      msg_reset["description"] = "The Avida driver is resetting";
      string path = driver->GetNewDriverPath();
      if (path == "")
        path = "/";
      DeleteDriver(driver);
     
      driver = SetupDriver(0, nullptr, path);
      if (driver){
               cerr << "About to run driver:"  << driver 
               << " with world " << driver->GetWorld() 
               << driver->GetWorld()->GetConfig().WORLD_X.Get() 
               << " " 
               << driver->GetWorld()->GetConfig().WORLD_Y.Get()
               << std::endl;
      }
      return driver;
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
      D_(D_STATUS, "Exiting runtime.  This is not recoverable via the web worker.");
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
      
      //D_(D_FLOW | D_STATUS, "Creating file system");
      //SetupIDBFS();
      //WriteTemp();
      //ReadTemp();
      
      NotifyDriverResetting();
      
      
      Driver* driver = SetupDriver(argc, argv, "/");
      
      D_(D_FLOW | D_STATUS, "Entering runtime loop");
      
      /* Any call to CheckMessage could end up forcing us
       to delete the cWorld object (and the driver, feedback, etc.)
       objects.  Consequently, we always have to check to see
       if our driver still exists before polling its state.  It
       should *never* be nullptr unless something went wrong with
       creating the driver.
       */
      while(driver){
        while(driver->IsActive()){
          //Begin with the driver in a paused state.
          //Messages can still be received periodically.
          
          cerr << "ZZ: The driver is located at " << driver << endl;
          cerr << driver->GetWorld()->GetConfig().WORLD_X.Get() 
               << " " 
               << driver->GetWorld()->GetConfig().WORLD_Y.Get()
               << std::endl;
          
          bool first_pass = true;
          while(driver && driver->ShouldPause()){
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
          while( driver && driver->ShouldRun() ) {
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
          if ( driver && driver->ShouldReset() ){
            NotifyDriverResetting();
            driver = DriverReset(driver);
            
          }
        } // The driver is no longer active
        cerr << driver->IsFinished() << driver->IsError() << driver->ShouldReset() << driver->IsActive() << endl;
        driver = SetupDriver(argc, argv, "/");  //Reset to default driver
      }
      
      cerr << "At the moment, we should never get here." << endl;
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
