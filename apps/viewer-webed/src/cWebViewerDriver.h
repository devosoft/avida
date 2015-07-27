#ifndef Avida_cWebViewerDriver_h
#define Avida_cWebViewerDriver_h

#include "cStats.h"
#include "cWorld.h"
#include "cPopulation.h"
#include "cHardwareBase.h"
#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"
#include <iostream>


using namespace Avida;

namespace Avida{
   namespace WebViewer{
      class cWebViewerDriver : public WorldDriver
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
            cWebViewerDriver(cWorld* world, cUserFeedback feedback)
            { cWebViewerDriver::Setup(world, feedback); }
            ~cWebViewerDriver() {GlobalObjectManager::Unregister(this);}
            cWebViewerDriver() = delete;
            cWebViewerDriver(const cWebViewerDriver&) = delete;

            bool Ready() {return (!m_done && m_world!=nullptr && m_ctx!=nullptr);};
            Avida::Feedback& Feedback() {return m_feedback;}

            void StepUpdate();
            void PlayPause();
            void Stop();

      };

      void cWebViewerDriver::PlayPause(){
         std::cerr << "PlayPause" << std::endl;
         if (m_pause){
            cWebViewerDriver::m_pause = false;
         } else {
            cWebViewerDriver::m_pause = true;
         }
      }

      void cWebViewerDriver::Stop(){
         std::cerr << "Stop." << std::endl;
         cWebViewerDriver::m_done = true;
      }


      void cWebViewerDriver::DisplayErrors(){
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


      void cWebViewerDriver::Setup(cWorld* a_world, cUserFeedback feedback)
      {
         GlobalObjectManager::Register(this);
         m_feedback = feedback;
         if (!a_world){
            m_feedback.Error("cWebViewerDriver is unable to find the world.");
         } else {
            // Setup our members 
            m_pause = false;
            m_done = false;
            m_world = a_world;
            m_ctx = new cAvidaContext(this, m_world->GetRandom());
            m_world->SetDriver(this);
         }     
      }


      void cWebViewerDriver::StepUpdate(){

         if (cWebViewerDriver::m_pause || cWebViewerDriver::m_done)
            return;
         cPopulation& population = m_world->GetPopulation();
         cStats& stats = m_world->GetStats();

         const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
         const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get() +
            m_world->GetConfig().POINT_INS_PROB.Get() +
            m_world->GetConfig().POINT_DEL_PROB.Get();

         //Perform a single update
         DisplayErrors(); 

         m_world->GetEvents(*m_ctx);
         if (m_done == true) return;

         // Increment the Update.
         stats.IncCurrentUpdate();

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
      } //cWebViewerDriver.h
   } //WebViewer namespace
} //Avida namespace

#endif
