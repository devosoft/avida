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
            bool m_pause;
            bool m_done;
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

            bool Ready() {return (!m_done && m_world!=nullptr && m_ctx!=nullptr);};
            Avida::Feedback& Feedback() {return m_feedback;}

            bool StepUpdate();
            void Run();
            void PlayPause();
            void Stop();
            json GetPopulationData();

      };

      void Driver::PlayPause()
      {
         std::cerr << "PlayPause" << std::endl;
         std::cerr << "\t m_pause=" << m_pause << " m_done=" << m_done << std::endl;
         if (m_pause){
            m_pause = false;
            if (!m_done) 
               Run();
         } else {
            m_pause = true;
         }
      }

      void Driver::Stop()
      {
         std::cerr << "Stop." << std::endl;
         m_done = true;
      }

      
      json Driver::GetPopulationData()
      {
         #define MGR_TO_JSON(KEY, TYPE)\
            { #KEY, mgr->GetCurrentValue(#KEY)->TYPE##Value() }

         const Avida::Data::ManagerPtr mgr = m_world->GetDataManager();
         json pop_data = {
            MGR_TO_JSON(core.update, Int),
            MGR_TO_JSON(core.world.organisms, Int),
            MGR_TO_JSON(core.world.ave_metabolic_rate, Double),
            MGR_TO_JSON(core.world.ave_gestation_time, Double),
            MGR_TO_JSON(core.world.ave_fitness, Double),
            MGR_TO_JSON(core.world.ave_age, Double)
         };

         #undef MGR_TO_JSON

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
            m_pause = false;
            m_done = false;
            m_world = a_world;
            m_ctx = new cAvidaContext(this, m_world->GetRandom());
            m_world->SetDriver(this);
         }     
      }



      void Driver::Run()
      {
         while(StepUpdate());
      }


      bool Driver::StepUpdate()
      {
         if (m_pause || m_done)
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
         if (m_done == true) return false;

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
         if (population.GetNumOrganisms() == 0) m_done = true;
         return true;
      } //Driver.h
   } //WebViewer namespace
} //Avida namespace

#endif
