//
//  WebDriverActions.h
//  viewer-webed
//
//  Created by Matthew Rupp on 9/23/15.
//  Copyright (c) 2015 MSU. All rights reserved.
//

#ifndef viewer_webed_WebDriverActions_h
#define viewer_webed_WebDriverActions_h

class cActionLibrary;

#include "avida/core/Feedback.h"
#include "cActionLibrary.h"

#include "Driver.h"
#include "WebDriverActions.h"

using namespace Avida::WebViewer;

class cWebAction : public cAction
{
  protected:
    Avida::Feedback& m_feedback;
  public:
    cWebAction(cWorld* world, const cString& args, Avida::Feedback& fb) : cAction(world,args) , m_feedback(fb) {}
};


class cWebActionPopulationStats : public cWebAction {
public:
  cWebActionPopulationStats(cWorld* world, const cString& args, Avida::Feedback& fb) : cWebAction(world, args, fb)
  {
  }
  static const cString GetDescription() { return "no arguments"; }
  void Process(cAvidaContext& ctx){
    const cStats& stats = m_world->GetStats();
    const int update = stats.GetUpdate();;
    const double ave_fitness = stats.GetAveFitness();;
    const double ave_gestation_time = stats.GetAveGestation();
    const double ave_metabolic_rate = stats.GetAveMerit();
    const int org_count = stats.GetNumCreatures();
    const double ave_age = stats.GetAveCreatureAge();
    
    json pop_data = {
      {"type", "data"},
      {"name", "webPopulationStats"},
      {"update", update},
      {"ave_fitness", ave_fitness},
      {"ave_gestation_time", ave_gestation_time},
      {"ave_metabolic_rate", ave_metabolic_rate},
      {"organisms", org_count},
      {"ave_age", ave_age},
      {"not", stats.GetTaskLastCount(0)},    //TODO: THIS WILL ONLY WORK WITH LOGIC 9  @MRR
      {"nand", stats.GetTaskLastCount(1)},
      {"and", stats.GetTaskLastCount(2)},
      {"orn", stats.GetTaskLastCount(3)},
      {"or", stats.GetTaskLastCount(4)},
      {"andn", stats.GetTaskLastCount(5)},
      {"nor", stats.GetTaskLastCount(6)},
      {"xor", stats.GetTaskLastCount(7)},
      {"equ", stats.GetTaskLastCount(8)}
    };
    m_feedback.Data(pop_data.dump().c_str());
  }
};


void RegisterWebDriverActions(cActionLibrary* action_lib)
{
    action_lib->Register<cWebActionPopulationStats>("webPopulationStats");
}

#endif
