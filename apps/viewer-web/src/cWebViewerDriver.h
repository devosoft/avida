#ifndef Avida_cWebViewerDriver_h
#define Avida_cWebViewerDriver_h

#include "cStats.h"
#include "cWorld.h"
#include "cPopulation.h"
#include "cHardwareBase.h"
#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"

#include "UI/UI.h"

#include "web_colors.h"

namespace UI = emp::UI;

using namespace Avida;

class cWebViewerDriver : public WorldDriver
{
   protected:
      cWorld* m_world;
      bool m_first_update;
      bool m_pause;
      bool m_done;
      /*
      class StdIOFeedback : public Avida::Feedback
      {
         void Error(const char* fmt, ...);
         void Warning(const char* fmt, ...);
         void Notify(const char* fmt, ...);
      } m_feedback;
      */
      cUserFeedback m_feedback;

      void Setup();
      
      static int m_update;
      static int m_num_creatures;
      static int m_ave_fitness;

      void DisplayErrors();

   public:
      cWebViewerDriver(cWorld* world, cUserFeedback feedback) : 
         m_world(world),m_first_update(true),m_pause(false),m_done(true),m_feedback(feedback) { cWebViewerDriver::Setup(); }
      ~cWebViewerDriver() {GlobalObjectManager::Unregister(this);}
      cWebViewerDriver() = delete;
      cWebViewerDriver(const cWebViewerDriver&) = delete;
     
      void Refresh(); 
      void Run(); 
      void Pause() {}
      void Finish() {}
      Avida::Feedback& Feedback() {return m_feedback;}
      void Abort(AbortCondition cnd) {}
      void RegisterCallback(DriverCallback callback) {}

};

int cWebViewerDriver::m_update = -1;
int cWebViewerDriver::m_num_creatures = -1;
int cWebViewerDriver::m_ave_fitness = -1;

void cWebViewerDriver::Refresh()
{
   DisplayErrors();
   //m_update = this->m_world->GetStats().GetUpdate();
   //m_num_creatures = this->m_world->GetStats().GetNumCreatures();
   //m_ave_fitness = this->m_world->GetStats().GetAveFitness();
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
      }
      UI::document << msg.GetData() << "</p>";
   }
}

void cWebViewerDriver::Setup()
{
   GlobalObjectManager::Register(this);
   UI::Initialize();
   if (m_world=nullptr){
      m_feedback.Error("cWebViewerDriver is unable to find the world.");
   } else {
      m_feedback.Notify("Setting driver.");
      m_world->SetDriver(this);
      UI::document << "<p>Update:  ";  //<< UI::Var(cWebViewerDriver::m_update);
      UI::document << "<p>Pop Size: "; //<< UI::Var(cWebViewerDriver::m_num_creatures);
      UI::document << "<p>Avg Fitness: ";// << UI::Var(cWebViewerDriver::m_ave_fitness);
   }
   Refresh();
}

void cWebViewerDriver::Run()
{
  return;
  cPopulation& population = m_world->GetPopulation();
  cStats& stats = m_world->GetStats();
  
  const int ave_time_slice = m_world->GetConfig().AVE_TIME_SLICE.Get();
  const double point_mut_prob = m_world->GetConfig().POINT_MUT_PROB.Get() +
                                m_world->GetConfig().POINT_INS_PROB.Get() +
                                m_world->GetConfig().POINT_DEL_PROB.Get();
  
  cAvidaContext ctx(this, m_world->GetRandom());
  ctx.EnableOrgFaultReporting();
  
  while (!m_done) {
    Refresh();
    
    m_world->GetEvents(ctx);
    if (m_done == true) break;
    
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
       
   for (int i = 0; i < UD_size; i++) population.ProcessStep(ctx, step_size, population.ScheduleOrganism());

    // end of update stats...
    population.ProcessPostUpdate(ctx);
    
    
    // Do Point Mutations
    if (point_mut_prob > 0 ) {
      for (int i = 0; i < population.GetSize(); i++) {
        if (population.GetCell(i).IsOccupied()) {
          population.GetCell(i).GetOrganism()->GetHardware().PointMutate(ctx);
        }
      }
    }
    
    // Exit conditons...
    if (population.GetNumOrganisms() == 0) m_done = true;
  }
  Refresh();

}
#endif
