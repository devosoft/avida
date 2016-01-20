#ifndef Avida_WebViewer_Driver_h
#define Avida_WebViewer_Driver_h

#include "cStats.h"
#include "cWorld.h"
#include "cPopulation.h"
#include "cHardwareBase.h"
#include "cEventList.h"
#include "cUserFeedback.h"
#include "avida/core/WorldDriver.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include <iostream>
#include <sstream>
#include <string>

#include <emscripten.h>
#include "Messaging.h"
#include "json.hpp"
#include "WebDebug.h"
#include "DriverConfig.h"
#include "Types.h"


using namespace Avida;
using json = nlohmann::json;

namespace Avida{
  namespace WebViewer{
    class Driver : public WorldDriver
    {
    private:
      string DumpEventList();
      void Pause()  { D_(D_FLOW | D_STATUS, "Pause"); D_(D_EVENTS, DumpEventList()); m_paused = (m_paused) ? false : true; }
      void Finish() {m_finished = true;}
      void Abort(AbortCondition cnd) {}
      void RegisterCallback(DriverCallback callback) {}
      
      
      
    protected:
      bool m_paused;    //Driver is paused
      bool m_finished;  //Driver is finished
      bool m_error;     //The driver has a critical error
      bool m_reset;     //The driver wants to reset when possible
      
      std::string m_reset_path;  //The path that contains configuration files for the next driver after the reset
      
      cUserFeedback m_feedback;  //A buffer containing messages that need to be sent to the GUI
      cWorld* m_world;           //The world
      cAvidaContext* m_ctx;
      int active_cell_id;        //The GUI has the ability to select an active cell; the driver needs this information for some actions
      
      
      void ProcessFeedback();  //Send messages to the GUI
      void Setup(cWorld*, cUserFeedback);  
      void ProcessAddEvent(const WebViewerMsg& msg, WebViewerMsg& ret_msg);  
      bool ValidateEventMessage(const WebViewerMsg& msg);
      string JsonToEventFormat(const WebViewerMsg& msg);
      
      
    public:
      Driver(cWorld* world, cUserFeedback& feedback) { Driver::Setup(world, feedback); }
      ~Driver() {GlobalObjectManager::Unregister(this);}
      Driver() = delete;
      Driver(const Driver&) = delete;
      
      bool IsError() const return { m_error; }
      bool IsFinished() const { return m_finished; }
      bool IsPaused() const { return m_paused; }
      
      bool ShouldReset() const {return m_reset;}
      void DoReset(const std::string& path) {m_reset = true; m_reset_path = path);
      
      bool IsActive() const {return m_world!=nullptr && m_ctx!=nullptr && !m_finished && !m_reset);
      bool ShouldPause() const { return m_is_paused && IsActive());
      bool ShouldRun() const {return !m_is_paused && IsActive());
      
      Avida::Feedback& Feedback()  {return m_feedback;}
      cWorld* GetWorld() { return m_world; }
      DriverConfig* GetNextConfig() { return nullptr; }
      
      
      void ProcessMessage(const WebViewerMsg& msg);
      bool StepUpdate();
      void Stop();
      json GetPopulationData();
      
    };
    
    
    
    /*
      The Feedback object contains messages that we want to send
      to the GUI.
      
      This method is where the driver files through the feedback
      object and creates JSON messages to send to the GUI.
    */
    void Driver::ProcessFeedback()
    {
      D_(D_FLOW,"Processing Feedback");
      for (int k=0; k<m_feedback.GetNumMessages(); ++k){
        cUserFeedback::eFeedbackType msg_type = m_feedback.GetMessageType(k);
        const cString& msg = m_feedback.GetMessage(k);
        WebViewerMsg ret_msg;
        switch(msg_type){
          case cUserFeedback::eFeedbackType::UF_ERROR:
            D_(D_MSG_OUT, "Feedback message is type ERROR");
            ret_msg = FeedbackMessage(Feedback::FATAL);
            ret_msg["message"] = msg.GetData();
            break;
          case cUserFeedback::eFeedbackType::UF_WARNING:
            D_(D_MSG_OUT, "tFeedback message is type WARNING");
            ret_msg = FeedbackMessage(Feedback::WARNING);
            ret_msg["message"] = msg.GetData();
            break;
          case cUserFeedback::eFeedbackType::UF_NOTIFICATION:
            D_(D_MSG_OUT, "Feedback message is type NOTIFICATION");
            ret_msg = FeedbackMessage(Feedback::NOTIFICATION);
            ret_msg["message"] = msg.GetData();
            break;
          case cUserFeedback::eFeedbackType::UF_DATA:
             D_(D_MSG_OUT, "Feedback message is type DATA");
            //Data messages will get sent directly with no userFeedback wrapper
            ret_msg = json::parse(msg.GetData());
            break;
          default:
            D_(D_MSG_OUT, "Feedback message is type UNKNOWN");
            ret_msg = FeedbackMessage(Feedback::UNKNOWN);
            ret_msg["message"] = msg.GetData();
            break;
        }
        PostMessage(ret_msg);
      }
      m_feedback.Clear();
      D_(D_FLOW, "Feedback processing complete");
    }
    
    
    /*
      Setup our driver.
      
      If there was a problem, set m_error to true.
      
      Otherwise, get ready to run and place us in a paused state.
    */
    void Driver::Setup(cWorld* a_world, cUserFeedback& feedback)
    {
      m_feedback = feedback;
      m_paused = false;
      m_finished = false;
      m_reset = false;
      m_world = nullptr;
      m_ctx = nullptr;
      m_error = false;
      
      D_(D_FLOW, "Setting up driver.");
      GlobalObjectManager::Register(this);
      if (!a_world){
        m_error = true;
        D_(D_FLOW, "Unable tosetup driver; world missing");
        m_feedback.Error("Driver is unable to find the world.");
      } else {
        // Setup our members 
        m_paused = true;
        m_world = a_world;
        m_ctx = new cAvidaContext(this, m_world->GetRandom());
        m_world->SetDriver(this);
        active_cell_id = -1;
        D_(D_FLOW, "Driver setup successful.");
      }
      ProcessFeedback();
    }
    
    
    /*
      Handle a message that was sent to the driver.
    */
    void Driver::ProcessMessage(const WebViewerMsg& msg)
    {
      D_(D_FLOW | D_MSG_IN, "ProcessMessage");
      D_(D_MSG_IN, "Received Message: " << msg);
      //This message is missing it's type; can't process.
      if (msg.find("type") == msg.end()) {  
        WebViewerMsg error_msg = FeedbackMessage(Feedback::WARNING);
        error_msg["request"];
        PostMessage(error_msg);
      } else {
        WebViewerMsg ret_msg = ReturnMessage(msg);
        ret_msg["success"] = false;
        if (msg["type"] == "addEvent") {  //This message is requesting we add an Event
          D_(D_MSG_IN, "Message is addEvent type");
          ProcessAddEvent(msg, ret_msg);  //So try to add it.
          D_(D_MSG_IN, "Done processing message");
        }
        else {
          D_(D_MSG_IN, "Message is unknown type");
          ret_msg["message"] = "unknown type";  //We don't know what this message wants
        }
        PostMessage(ret_msg);
        ProcessFeedback();
      }
      D_(D_FLOW | D_MSG_IN, "Done processing message");
    }
    
    
    /*
      As the name sounds, dump our event list.
    */
    string Driver::DumpEventList()
    {
      cEventList* list = m_world->GetEventsList();
      if (list == nullptr)
        return "";
      ostringstream oss;
      list->PrintEventList(oss);
      return oss.str();
    }
    
    
    /*
      Try to add an event from a message requesting that we do so.
      
      We will first add in additional properties of the message
      with particular defaults.
      
      Next, we will try to add the message to the event queue by
      seralizing the message into something the event system can
      consume.
      
      After that, we will trigger any immediate events and move
      on.
    */
    void Driver::ProcessAddEvent(const WebViewerMsg& rcv_msg, WebViewerMsg& ret_msg)
    {
    
      D_(D_FLOW, "Attempting to addEvent");
      
      //Some properties aren't required; we'll add defaults if they are missing
      WebViewerMsg msg = DefaultAddEventMessage(rcv_msg);
      
      D_(D_MSG_IN, "Defaulted message to be processed is " << msg);
      
      //Validate that the properties in the addEvent message is correct.
      if (!ValidateEventMessage(msg)){
        D_(D_MSG_IN, "addEvent is invalid");
        ret_msg["success"] = false;
      }
      //TODO: actually make run pause action; for now just treat it as immediate
      //Right now any addEvent with runPause will happen immediately.
      else if (msg["name"] == "runPause"){
        D_(D_MSG_IN, "addEvent has name runPause");
        ret_msg["success"] = true;
        Pause();
      } else {
        D_(D_MSG_IN, "Event is other than runPause");
        string event_line = JsonToEventFormat(msg);  //This will contain a properly formatted event list line if successful
        D_(D_MSG_IN, "Trying to add event line: " + event_line);
        
        //Singleton events can only be in the event queue once
        if (msg["singleton"] == json(true)) {
          D_(D_MSG_IN, "Message is singleton; deleting other entries with event named: " << msg["name"]);
          int deleted = m_world->GetEventsList()->DeleteEventByName(cString(msg["name"].get<std::string>().c_str()));
          D_(D_MSG_IN, deleted << " events removed.");
        }
        
        //
        bool success = m_world->GetEventsList()->AddEventFileFormat(event_line.data(), m_feedback);
        ret_msg["success"] = success;
        
        if (success){  //Process any immediate events
          D_(D_MSG_IN, "Event successfully added");
          // If we're paused, and the event starts "now", 
          // process the event immediately but also queue it for the next update as well
          if (msg["start"] == json("now") && m_paused == true){
            WebViewerMsg immediate = msg;
            immediate["trigger"] = "immediate";
            event_line = JsonToEventFormat(immediate);
            success = m_world->GetEventsList()->AddEventFileFormat(event_line.data(), m_feedback);
            D_(D_MSG_IN, "Event triggers now, and we're paused.  Did it add for immediate execution? " << success);
          }
          m_world->GetEventsList()->ProcessImmediates(*m_ctx);
          D_(D_MSG_IN, "Immediate events processed.");
        } else {
          D_(D_MSG_IN, "Unable to add event");
        }
      } //Done with non runPause message processing
      D_(D_MSG_IN | D_FLOW, "Done processing add event.");
    }
    
    
    
    /*
      Events need to have a particular format in order for them to be
      processed correctly.  This method attempts to validate an
      event message.
    */
    bool Driver::ValidateEventMessage(const json& msg)
    {
      D_(D_FLOW | D_MSG_IN, "Validating event message.");
      bool success = true;
      
      //Action name is missing
      D_(D_MSG_IN, "Checking for name.");
      if (msg.find("name") == msg.end()){
        success=false;
        m_feedback.Warning("addEvent is missing name property");
      }
      // Can't find event trigger type
      D_(D_MSG_IN, "Checking for type");
      if (msg.find("triggerType") == msg.end()){
        success=false;
        m_feedback.Warning("addEvent is missing triggerType property");
      }
      
      if (msg["triggerType"] == "i" || msg["triggerType"] == "immediate"){
        D_(D_MSG_IN, "Message is trigerType immediate");
        if (msg.count("start") != 0 || msg.count("interval") != 0 || msg.count("end") != 0){
          D_(D_MSG_IN, "Message has a start, interval, or end condition specified.");
          m_feedback.Warning("addEvent is triggerType immediate but has timing start, interval, or end specified");
          success = false;
        }
      } else {
        //Missing start condition
        D_(D_MSG_IN, "Checking for start");
        if (msg.find("start") == msg.end()){
          success = false;
          m_feedback.Warning("addEvent is missing start property");
        }
        // Can't find the event interval
        D_(D_MSG_IN, "Checking for interval");
        if (msg.find("interval") == msg.end()){
          success=false;
          m_feedback.Warning("addEvent is missing interval property");
        }
        // Can't find the event end 
        D_(D_MSG_IN, "Checking for end");
        if (msg.find("end") == msg.end() || msg["end"] != ""){
          success=false;
          m_feedback.Warning("addEvent is missing end property");
        }
      }
      D_(D_FLOW | D_MSG_IN, "Done validating event.");
      return success;
    }
    
    
    /*
      Avida's event system expects that we'll be sending it a single string to configure the
      timing and action that is to be triggered.  Therefore, we must convert our JSON object
      into a string that it can consume.
      
      A special note: WebActions can receive not just an ordered, space delimited string of
      arguments.  They can also receive a serialized JSON object.  If args is undefined
      and there are additional properties of the message are considered parts of the JSON
      object to send the action.  WebViewer actions determine if the string object pased as an
      argument is a serialized JSON object if the first character of the argument string is a
      UNIT_SEP character.
    */
    string Driver::JsonToEventFormat(const WebViewerMsg& msg)
    {
      D_(D_MSG_IN | D_FLOW, "JsonToEventFormat");
      ostringstream line_in;
      
      //Convert action triggering information
      line_in << DeQuote(msg["triggerType"]) << " ";
      if (msg["triggerType"] != "i" && msg["triggerType"] != "immediate")
      {
        line_in << ((msg["start"].is_string()) ? DeQuote(msg["start"]) : to_string((double)(msg["start"])));
        line_in << ":";
        line_in << ((msg["interval"].is_string()) ? DeQuote(msg["interval"]) : to_string((double)(msg["interval"])));
        if (msg["end"] != "")
           line_in << ((msg["end"].is_string()) ? DeQuote(msg["end"]) : to_string((double)(msg["end"])));
      }
      
      //Convert action name
      line_in << " " << DeQuote(msg["name"]);
      
      //Convert action arguments
      if (contains(msg,"args")){  //Arguments are specified through the args property (ordered, unnamed)
        for (auto arg : msg["args"]){ 
          line_in << " " << (msg["args"]).is_string()) ? DeQuote(msg[args]) : msg["args"]);
      } else {  // Arguments in json format
        json jargs = StripProperties(jargs, EVENT_PROPERTIES);  //Remove known properties for events
        if (!jargs.empty()){  //If there are still properties defined, pass them
          string str_jargs = string(UNIT_SEP) + string(jargs.dump());  //UNIT_SEP at 0 indicates to WebActions this is a json object
          line_in << " " << str_jargs;
        }
      }
      
      D_(D_MSG_IN | D_FLOW, "Done JsonToEventFormat");
      return line_in.str();  //Return our input from the stream
    }
    
    
    
    /*
      StepUpdate executes a single update of the Avida Experiment.
    */
    bool Driver::StepUpdate()
    {
      
      //If we're paused or we're done, return that we're no longer running
      if (m_paused || m_finished)
        return false;
        
      //Otherwise, let's get ready to do an update
      cPopulation& population = m_world->GetPopulation();
      cStats& stats = m_world->GetStats();
      
      const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
      const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get() +
      m_world->GetConfig().POINT_INS_PROB.Get() +
      m_world->GetConfig().POINT_DEL_PROB.Get();
      
      //Perform a single update
      ProcessFeedback(); 
      
      m_world->GetEvents(*m_ctx);
      if (m_finished == true) return false;
      
      // Increment the Update.
      stats.IncCurrentUpdate();
      
      
      population.ProcessPreUpdate();
      D_(D_FLOW, "Update: " << stats.GetUpdate(), 1);
     
      
      // Handle all data collection for previous update.
      if (stats.GetUpdate() > 0) {
        // Tell the stats object to do update calculations and printing.
        stats.ProcessUpdate();
      }
      
      
      // Process the update.
      const int UD_size = ave_time_slice * population.GetNumOrganisms();
      const double step_size = 1.0 / (double) UD_size;
      
      for (int i = 0; i < UD_size; i++) 
        population.ProcessStep(*m_ctx, step_size, population.ScheduleOrganism());
      
      // end of update stats...
      population.ProcessPostUpdate(*m_ctx);
      
      
      // Do Point Mutations
      if (point_mut_prob > 0 ) {
        for (int i = 0; i < population.GetSize(); i++) {
          if (population.GetCell(i).IsOccupied()) {
            population.GetCell(i).GetOrganism()->GetHardware().PointMutate(*m_ctx);
          }
        }
      }
      
      ProcessFeedback();
      
      // Exit conditons...
      if (population.GetNumOrganisms() == 0) m_finished = true;
      return true;
    } //Driver.h
  } //WebViewer namespace
} //Avida namespace



#endif
