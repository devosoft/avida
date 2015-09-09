#ifndef Avida_WebViewer_Driver_h
#define Avida_WebViewer_Driver_h

#include "cStats.h"
#include "cWorld.h"
#include "cPopulation.h"
#include "cHardwareBase.h"
#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"
#include "avida/data/Manager.h"
#include "avida/data/Package.h"
#include <iostream>

#include <emscripten.h>
#include "Messaging.h"
#include "json.hpp"


using namespace Avida;
using json = nlohmann::json;

namespace Avida{
  namespace WebViewer{
    class Driver : public WorldDriver
    {
    private:  //These do nothing
      void Pause()  {}
      void Finish() {}
      void Abort(AbortCondition cnd) {}
      void RegisterCallback(DriverCallback callback) {}
      
    protected:
      bool m_paused;
      bool m_finished;
      cUserFeedback m_feedback;
      cWorld* m_world;
      bool m_first_update;
      cAvidaContext* m_ctx;
      
      bool m_ready;
      
      void DisplayErrors();
      void Setup(cWorld*, cUserFeedback);
      
    public:
      Driver(cWorld* world, cUserFeedback feedback)
      { Driver::Setup(world, feedback); }
      ~Driver() {GlobalObjectManager::Unregister(this);}
      Driver() = delete;
      Driver(const Driver&) = delete;
      
      bool Ready() const {return (!m_finished && m_world!=nullptr && m_ctx!=nullptr);};
      bool IsFinished() const { return m_finished; }
      bool IsPaused() const { return m_paused; }
      Avida::Feedback& Feedback()  {return m_feedback;}
      
      void ProcessMessage(const WebViewerMsg& msg);
      bool StepUpdate();
      void Run();
      void RunPause();
      void Stop();
      json GetPopulationData();
      
    };
    
    
    void Driver::RunPause()
    {
      cerr << "RunPause" << endl;
      cerr << "\t m_paused=" << m_paused << " m_finished=" << m_finished << endl;
      if (m_paused){
        m_paused = false;
      } else {
        m_paused = true;
      }
    }
    
    void Driver::Stop()
    {
      std::cerr << "Stop." << std::endl;
      m_finished = true;
    }
    
    
    json Driver::GetPopulationData()
    {
      const cStats& stats = m_world->GetStats();
      const int update = stats.GetUpdate();;
      const double ave_fitness = stats.GetAveFitness();;
      const double ave_gestation_time = stats.GetAveGestation();
      const double ave_metabolic_rate = stats.GetAveMerit();
      const int org_count = stats.GetNumCreatures();
      const double ave_age = stats.GetAveCreatureAge();
      
      json pop_data = {
        {"core.update", update},
        {"core.world.ave_fitness", ave_fitness},
        {"core.world.ave_gestation_time", ave_gestation_time},
        {"core.world.ave_metabolic_rate", ave_metabolic_rate},
        {"core.world.organisms", org_count},
        {"core.world.ave_age", ave_age}
      };
      
      return pop_data;
    }
    
    
    void Driver::DisplayErrors(){
      for (int k=0; k<m_feedback.GetNumMessages(); ++k){
        cUserFeedback::eFeedbackType msg_type = m_feedback.GetMessageType(k);
        const cString& msg = m_feedback.GetMessage(k);
        switch(msg_type){
          case cUserFeedback::eFeedbackType::UF_ERROR:
            std::cerr << "Error: ";
            break;
          case cUserFeedback::eFeedbackType::UF_WARNING:
            std::cerr << "Warning: ";
            break;
          case cUserFeedback::eFeedbackType::UF_NOTIFICATION:
            std::cerr << "Note: ";
            break;
          default:
            break;
        }
        cerr << msg.GetData() << std::endl;
      }
      m_feedback.Clear();
    }
    
    
    void Driver::Setup(cWorld* a_world, cUserFeedback feedback)
    {
      GlobalObjectManager::Register(this);
      m_feedback = feedback;
      if (!a_world){
        m_feedback.Error("Driver is unable to find the world.");
      } else {
        // Setup our members 
        m_paused = true;
        m_finished = false;
        m_world = a_world;
        m_ctx = new cAvidaContext(this, m_world->GetRandom());
        m_world->SetDriver(this);
      }     
    }
    
    
    void Driver::ProcessMessage(const WebViewerMsg& msg)
    {
      cerr << "\tProcessing message" << msg.dump() << endl;
      bool success = false;
      
      if (msg.find("Key") == msg.end()) {  //This message is missing it's Key; can't process.
        cerr << "\t\tMessage is invlaid" << endl;
        WebViewerMsg error_msg;
        error_msg["Key"] = "AvidaError";
        error_msg["Fatal"] = false;
        error_msg["Type"] = "Messaging";
        error_msg["Description"] = "Message is missing key.";
        error_msg["Received"] = msg;
        //PostMessage(error_msg);
      } else {
        cerr << "\t\tMessage is valid." << endl;
        //WebViewerMsg msg = ReturnMessage(msg);
        //msg["Success"] = false;
        
        string key = msg["Key"];
      
        if (key == "RunPause"){
          cerr << "\t\t\tMessage is RunPause" << endl;
          //msg["Success"] = true;
          RunPause();
          //PostMessage(msg);
        } else if (key == "Finish") {
          //msg["Success"] = true;
          m_finished = true;
        } else {
          //msg["Description"] = "Unknown key";
        }
        //PostMessage(msg);
      }
    }
    
    
    extern "C"
    void Driver::Run()
    {
            //This method may not be needed.
    }
    
    
    bool Driver::StepUpdate()
    {
      if (m_paused || m_finished)
        return false;
      cPopulation& population = m_world->GetPopulation();
      cStats& stats = m_world->GetStats();
      
      const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
      const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get() +
      m_world->GetConfig().POINT_INS_PROB.Get() +
      m_world->GetConfig().POINT_DEL_PROB.Get();
      
      //Perform a single update
      DisplayErrors(); 
      
      m_world->GetEvents(*m_ctx);
      if (m_finished == true) return false;
      
      // Increment the Update.
      stats.IncCurrentUpdate();
      
      std::cerr << "Update: " << stats.GetUpdate() << std::endl;
      
      population.ProcessPreUpdate();
      
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
      
      DisplayErrors();
      
      // Exit conditons...
      if (population.GetNumOrganisms() == 0) m_finished = true;
      return true;
    } //Driver.h
  } //WebViewer namespace
} //Avida namespace



#endif
