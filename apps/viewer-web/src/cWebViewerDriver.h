#ifndef Avida_cWebViewerDriver_h
#define Avida_cWebViewerDriver_h

#include "cStats.h"
#include "cWorld.h"
#include "cPopulation.h"
#include "cHardwareBase.h"
#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"
#include <iostream>

#include "UI/UI.h"

#include "web_colors.h"

namespace UI = emp::UI;

using namespace Avida;

class cWebViewerDriver : public WorldDriver
{
   private:  //These do nothing
      void Pause()  {}
      void Finish() {}
      void Abort(AbortCondition cnd) {}
      void RegisterCallback(DriverCallback callback) {}

   protected:
      static bool m_pause;
      static bool m_done;
      static cUserFeedback m_feedback;
      static cWorld* m_world;
      static bool m_first_update;
      static cAvidaContext* m_ctx;
      static int m_update;
      static int m_popsize;
      static double m_ave_fitness;

      bool m_ready;

      static void DisplayErrors();
      void Setup(cWorld*, cUserFeedback);
      static void UpdateData();

   public:
      cWebViewerDriver(cWorld* world, cUserFeedback feedback)
                  { cWebViewerDriver::Setup(world, feedback); }
      ~cWebViewerDriver() {GlobalObjectManager::Unregister(this);}
      cWebViewerDriver() = delete;
      cWebViewerDriver(const cWebViewerDriver&) = delete;
     
      bool Ready() {return (!m_done && m_world!=nullptr && m_ctx!=nullptr);};
      void Layout();
      Avida::Feedback& Feedback() {return m_feedback;}
      
      static void Refresh(); 
      static void StepUpdate();
      static void PlayPause();
      static void Stop();

};

bool cWebViewerDriver::m_pause = false;
bool cWebViewerDriver::m_done = false;
cUserFeedback cWebViewerDriver::m_feedback;
cWorld* cWebViewerDriver::m_world = nullptr;
cAvidaContext* cWebViewerDriver::m_ctx = nullptr;
int cWebViewerDriver::m_update = 0;
int cWebViewerDriver::m_popsize = 0;
double cWebViewerDriver::m_ave_fitness = 0;


void cWebViewerDriver::PlayPause(){
   std::cerr << "PlayPause" << std::endl;
   if (m_pause){
      cWebViewerDriver::m_pause = false;
   } else {
      cWebViewerDriver::m_pause = true;
   }
   Refresh();
}

void cWebViewerDriver::Stop(){
   cWebViewerDriver::m_done = true;
   Refresh();
}

void cWebViewerDriver::Refresh()
{
  DisplayErrors();
  UI::document.Update();
}

void cWebViewerDriver::DisplayErrors(){
   const Color error_clr = Colors::red;
   const Color warn_clr = Colors::green;
   const Color notify_clr = Colors::black;
   for (int k=0; k<m_feedback.GetNumMessages(); ++k){
      cUserFeedback::eFeedbackType msg_type = m_feedback.GetMessageType(k);
      const cString& msg = m_feedback.GetMessage(k);
      UI::document << "<p style=\"color:";
      switch(msg_type){
         case cUserFeedback::eFeedbackType::UF_ERROR:
            UI::document << error_clr << "\">Error: ";
            break;
         case cUserFeedback::eFeedbackType::UF_WARNING:
            UI::document << warn_clr << "\">Warning: ";
            break;
         case cUserFeedback::eFeedbackType::UF_NOTIFICATION:
            UI::document << notify_clr <<"\">Note: ";
            break;
         default:
            UI::document << "#000000\">";
            break;
      }
      UI::document << msg.GetData() << "</p>";
   }
   m_feedback.Clear();
}

void cWebViewerDriver::Setup(cWorld* a_world, cUserFeedback feedback)
{
   GlobalObjectManager::Register(this);
   m_feedback = feedback;
   UI::Initialize();
   if (!a_world){
      m_feedback.Error("cWebViewerDriver is unable to find the world.");
   } else {
      // Setup our static members 
      m_pause = false;
      m_done = false;
      m_world = a_world;
      m_update = 0;
      m_popsize = 0;
      m_ave_fitness = 0;
      m_ctx = new cAvidaContext(this, m_world->GetRandom());
      m_world->SetDriver(this);
      Layout();
  }     
  Refresh();
}

void cWebViewerDriver::Layout(){
   UI::document << "<H1>Avida</H1>";
   UI::document << "<p>Update:  " 
         << UI::Live(m_update);
   UI::document << "<p>Pop Size: " 
         << UI::Live(m_popsize);
   UI::document << "<p>Avg Fitness: " 
         << UI::Live(m_ave_fitness);
   UI::document << "<br>";
   UI::document << UI::Button(cWebViewerDriver::PlayPause, "Play/Pause") 
      << UI::Button(cWebViewerDriver::Stop, "Stop");
}

void cWebViewerDriver::UpdateData(){
   m_update = m_world->GetStats().GetUpdate();
   m_popsize = m_world->GetStats().GetNumCreatures();
   m_ave_fitness = m_world->GetStats().GetAveFitness();
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
   Refresh();
   
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

   cWebViewerDriver::UpdateData();

   // Exit conditons...
   if (population.GetNumOrganisms() == 0) m_done = true;
}


#endif
