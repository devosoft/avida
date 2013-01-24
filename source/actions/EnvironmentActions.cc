/*
 *  EnvironmentActions.cc
 *  Avida
 *
 *  Created by David Bryson on 8/9/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "math.h"
#include "EnvironmentActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cArgSchema.h"
#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cResourceDef.h"
#include "cStats.h"
#include "cUserFeedback.h"
#include "cWorld.h"

class cActionInjectResource : public cAction
{
private:
  cString m_res_name;
  double m_res_count;
  
public:
  cActionInjectResource(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_res_count(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
  }

  static const cString GetDescription() { return "Arguments: <string res_name> <double res_count>"; }

  void Process(cAvidaContext& ctx)
  {
    cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
    if (res != NULL) m_world->GetPopulation().GetResources().UpdateResource(ctx, res->GetID(), m_res_count);
  }
};


/*
 Inject (add) a specified amount of a specified resource, scaled by
 the current average merit divided by the average time slice.
*/
class cActionInjectScaledResource : public cAction
{
private:
  cString m_res_name;
  double m_res_count;
  
public:
  cActionInjectScaledResource(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_res_count(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string res_name> <double res_count>"; }
  
  void Process(cAvidaContext& ctx)
  {
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if (ave_merit <= 0.0) ave_merit = 1.0; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();

    cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
    if (res != NULL) m_world->GetPopulation().GetResources().UpdateResource(ctx, res->GetID(), (m_res_count / ave_merit));
  }
};


/*
 Removes a specified percentage of a specified resource, scaled by
 the current average merit divided by the average time slice.
 */
class cActionOutflowScaledResource : public cAction
{
private:
  cString m_res_name;
  double m_res_percent;
  
public:
  cActionOutflowScaledResource(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_res_percent(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_percent = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string res_name> <double res_percent>"; }
  
  void Process(cAvidaContext& ctx)
  {
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if (ave_merit <= 0.0) ave_merit = 1.0; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();

    cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
    
    double res_level = m_world->GetPopulation().GetResources().GetResource(ctx, res->GetID());
    double scaled_perc = 1 / (1 + ave_merit * (1 - m_res_percent) / m_res_percent);
    res_level -= res_level * scaled_perc;
    
    if (res != NULL) m_world->GetPopulation().GetResources().UpdateResource(ctx, res->GetID(), res_level);
  }
};

/* Change the amount of a particular global resource */

class cActionSetResource : public cAction
{
private:
  cString m_res_name;
  double m_res_count;
  
public:
  cActionSetResource(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_res_count(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string res_name> <double res_count>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().GetResources().SetResource(ctx, m_res_name, m_res_count);
  }
};


/* Zero out all resources */

class cZeroResources : public cAction
{
private:
  
public:
  cZeroResources(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
  }
  
  static const cString GetDescription() { return "Arguments: none"; }
  
  void Process(cAvidaContext& ctx)
  {
    cResourceDefLib & res_lib = m_world->GetEnvironment().GetResDefLib();
    for (int i=0; i < res_lib.GetSize(); i++)  {
      cResourceDef* res = res_lib.GetResDef(i);
      m_world->GetPopulation().GetResources().SetResource(ctx, res->GetID(), 0.0);
    }
  }
};

/* Change the amount of a particular resource in a particular cells */

class cActionSetCellResource : public cAction
{
private:
  Apto::Array<int> m_cell_list;
  cString m_res_name;
  double m_res_count;
  int m_res_id;
  
public:
  cActionSetCellResource(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_cell_list(0), m_res_name(""), m_res_count(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) 
    {
      cString s = largs.PopWord();
      m_cell_list = cStringUtil::ReturnArray(s);    
    }
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
    
    cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
    assert(res);
    m_res_id = res->GetID(); // Save the id so we don't have to do many string conversions
  }

  static const cString GetDescription() { return "Arguments: <int cell_id> <string res_name> <double res_count>"; }

  void Process(cAvidaContext& ctx)
  {
    cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_id);
    for(int i=0; i<m_cell_list.GetSize(); i++)
    {
      int m_cell_id = m_cell_list[i];
      Apto::Array<double> counts = m_world->GetPopulation().GetResources().GetCellResources(m_cell_id, ctx);
      if ((res != NULL) && (res->GetID() < counts.GetSize()))
      {
        counts[res->GetID()] = m_res_count;
        m_world->GetPopulation().GetResources().SetCellResources(m_cell_id, counts);
      }
    }
  }
};


/*Change the settings of a Dynamic Resource*/
class cActionSetDynamicResource : public cAction
{
private:
  cString env_string;
  cString m_res_name;
  
public:
  cActionSetDynamicResource(cWorld* world, const cString& args, Feedback&) : cAction(world, args), env_string("")
  {
    cString largs(args);
    if (largs.GetSize()) env_string = largs;
    if (largs.GetSize()) largs.PopWord();
    if (largs.GetSize()) m_res_name = largs.PopWord().Pop(':');
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string env_string>"; }
  
  void Process(cAvidaContext& ctx)
  {
    cUserFeedback feedback;
    m_world->GetEnvironment().LoadLine(env_string, feedback);
    
    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
        case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }
        
    m_world->GetPopulation().GetResources().UpdateDynamicRes(ctx, m_world, m_res_name);
  } 
};

class cActionSetDynamicResPlateauInflow : public cAction
{
private:
  cString m_res_name;
  double m_inflow;
  
public:
  cActionSetDynamicResPlateauInflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_inflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_inflow = largs.PopWord().AsDouble();
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double inflow>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name)->SetPlateauInflow(m_inflow);
  } 
};

class cActionSetDynamicResPlateauOutflow : public cAction
{
private:
  cString m_res_name;
  double m_outflow;
  
public:
  cActionSetDynamicResPlateauOutflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_outflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_outflow = largs.PopWord().AsDouble();
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double outflow>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name)->SetPlateauOutflow(m_outflow);
  } 
};

class cActionSetDynamicResConeInflow : public cAction
{
private:
  cString m_res_name;
  double m_inflow;
  
public:
  cActionSetDynamicResConeInflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_inflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_inflow = largs.PopWord().AsDouble();
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double inflow>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name)->SetConeInflow(m_inflow);
  } 
};

class cActionSetDynamicResConeOutflow : public cAction
{
private:
  cString m_res_name;
  double m_outflow;
  
public:
  cActionSetDynamicResConeOutflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_outflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_outflow = largs.PopWord().AsDouble();
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double outflow>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name)->SetConeOutflow(m_outflow);
  } 
};

class cActionSetDynamicResInflow : public cAction
{
private:
  cString m_res_name;
  double m_inflow;
  
public:
  cActionSetDynamicResInflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_inflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_inflow = largs.PopWord().AsDouble();
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double inflow>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name)->SetDynamicResInflow(m_inflow);
  } 
};

class cActionSetDynamicResPlatVarInflow : public cAction
{
private:
  cString m_res_name;
  double m_mean;
  double m_variance;
  int m_type;
  
public:
  cActionSetDynamicResPlatVarInflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_mean(0.0), m_variance(0.0), m_type(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_mean = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_variance = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_type = largs.PopWord().AsInt();
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double mean> <double variance>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().GetResources().SetDynamicResPlatVarInflow(m_res_name, m_mean, m_variance, m_type);
  } 
};

class cActionSetPredatoryResource : public cAction
{
private:
  cString m_res_name;
  double m_odds;
  int m_juvs_per;
  double m_detection_prob;
  
public:
  cActionSetPredatoryResource(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_res_name(""), m_odds(0.0), m_juvs_per(0), m_detection_prob(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_odds = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_juvs_per = largs.PopWord().AsInt();
    if (largs.GetSize()) m_detection_prob = largs.PopWord().AsDouble();
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double kill_odds> <int guarded_juvs_per_adult> <double detection_prob>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().GetResources().SetPredatoryResource(m_res_name, m_odds, m_juvs_per, m_detection_prob);        
  } 
};

class cActionSetProbabilisticResource : public cAction
{
private:
  cString m_res_name;
  double m_initial;
  double m_inflow;
  double m_outflow;
  double m_lambda;
  double m_theta;
  int m_x;
  int m_y;
  int m_count;
  
public:
  cActionSetProbabilisticResource(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args), m_res_name(""), 
                                  m_initial(0.0), m_inflow(0.0), m_outflow(0.0), m_lambda(1.0), m_theta(0.0), m_x(-1), m_y(-1), m_count(-1)
  {
    cArgSchema schema(':','=');
    schema.AddEntry("res_name", 0, (const char*)m_world->GetEnvironment().GetResDefLib().GetResDef(0)->GetName());
    
    schema.AddEntry("initial", 0, 0.0);
    schema.AddEntry("inflow", 1, 0.0);
    schema.AddEntry("outflow", 2, 0.0);
    schema.AddEntry("lambda", 3, 1.0);
    schema.AddEntry("theta", 4, 0.0); 
    
    schema.AddEntry("x", 0, -1);
    schema.AddEntry("y", 1, -1);
    schema.AddEntry("num", 2, -1);
    
    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (args) {
      m_res_name = argc->GetString(0);
      
      m_initial = argc->GetDouble(0);
      m_inflow = argc->GetDouble(1);
      m_outflow = argc->GetDouble(2);
      m_lambda = argc->GetDouble(3);
      m_theta = argc->GetDouble(4);
      
      m_x = argc->GetInt(0);
      m_y = argc->GetInt(1);
      m_count = argc->GetInt(2);
    }
    
    assert(m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name));
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double initial> <double inflow> <double outflow> <double lambda> <double theta> <int x> <int y> <int num>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name)->SetProbabilisticResource(ctx, m_initial, m_inflow, m_outflow, m_lambda, m_theta, m_x, m_y, m_count);
  } 
};

/* Change Environment settings */
class cActionChangeEnvironment : public cAction
{
private:
  cString env_string;
  
public:
  cActionChangeEnvironment(cWorld* world, const cString& args, Feedback&) : cAction(world, args), env_string("")
  {
    cString largs(args);
    if (largs.GetSize()) env_string = largs;
  }
  
  static const cString GetDescription() { return "Arguments: <string env_string>"; }
  
  void Process(cAvidaContext&)
  {
    cUserFeedback feedback;
    m_world->GetEnvironment().LoadLine(env_string, feedback);

    for (int i = 0; i < feedback.GetNumMessages(); i++) {
      switch (feedback.GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
        case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
        default: break;
      };
      cerr << feedback.GetMessage(i) << endl;
    }

    m_world->GetPopulation().GetResources().UpdateResource(m_world->GetVerbosity(), m_world);          
  }
};

// Set the values associated with a specified reaction.  If the name of the
// reaction used is "ALL" then all reactions will be changed.  If the name is
// "RANDOM:3" then random reactions will be set to the specified value.  The
// number after the colon indicated the number of reactions to set.

class cActionSetReactionValue : public cAction
{
private:
  cString m_name;
  double m_value;
  
public:
  cActionSetReactionValue(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_value(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_value = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <double value>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().SetReactionValue(ctx, m_name, m_value);
  }
};


class cActionSetReactionValueMult : public cAction
{
private:
  cString m_name;
  double m_value;
  
public:
  cActionSetReactionValueMult(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_value(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_value = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <double value>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().SetReactionValueMult(m_name, m_value);
  }
};


class cActionSetReactionInst : public cAction
{
private:
  cString m_name;
  cString m_inst;
  
public:
  cActionSetReactionInst(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_inst("")
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_inst = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <string inst>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().SetReactionInst(m_name, m_inst);
  }
};

class cActionSetReactionMinTaskCount : public cAction
{
private:
  cString m_name;
  int m_min_count;
  
public:
  cActionSetReactionMinTaskCount(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_min_count(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_min_count = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <int min_count>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().SetReactionMinTaskCount(m_name, m_min_count);
  }
};

class cActionSetReactionMaxTaskCount : public cAction
{
private:
  cString m_name;
  int m_max_count;
  
public:
  cActionSetReactionMaxTaskCount(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_max_count(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_max_count = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <int max_count>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().SetReactionMaxTaskCount(m_name, m_max_count);
  }
};

class cActionSetReactionMinCount : public cAction
{
private:
  cString m_name;
  int m_reaction_min_count;
  
public:
  cActionSetReactionMinCount(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_reaction_min_count(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_reaction_min_count = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <int reaction_min_count>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().SetReactionMinCount(m_name, m_reaction_min_count);
  }
};

class cActionSetReactionMaxCount : public cAction
{
private:
  cString m_name;
  int m_reaction_max_count;
  
public:
  cActionSetReactionMaxCount(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_reaction_max_count(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_reaction_max_count = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <int reaction_max_count>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetEnvironment().SetReactionMaxCount(m_name, m_reaction_max_count);
  }
};

class cActionSetReactionTask : public cAction
{
private:
  cString m_name;
  cString m_task;
  
public:
  cActionSetReactionTask(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_task("")
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_task = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <string task_name>"; }
  
  void Process(cAvidaContext&)
  {
    bool success = m_world->GetEnvironment().SetReactionTask(m_name, m_task);
    if (!success) {
      m_world->GetDriver().Feedback().Error("SetReactionTask action failed");
      m_world->GetDriver().Abort(Avida::INTERNAL_ERROR);
    }
  }
};

class cActionSetResourceInflow : public cAction
{
private:
  cString m_name;
  double m_inflow;
  
public:
  cActionSetResourceInflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_inflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_inflow = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double inflow>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().GetResources().SetResourceInflow(m_name, m_inflow);
  }
};




class cActionSetResourceOutflow : public cAction
{
private:
  cString m_name;
  double m_outflow;
  
public:
  cActionSetResourceOutflow(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_name(""), m_outflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_outflow = largs.PopWord().AsDouble();
    assert(m_outflow <= 1.0);
    assert(m_outflow >= 0.0);
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <double outflow>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetPopulation().GetResources().SetResourceOutflow(m_name, m_outflow);
  }
};



class cActionSetEnvironmentInputs : public cAction
{
private:
  Apto::Array<int> m_inputs;
  
public:
  cActionSetEnvironmentInputs(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_inputs()
  {
    cString largs(args);
    if (largs.GetSize()) m_inputs.Push(largs.PopWord().AsInt());
    if (largs.GetSize()) m_inputs.Push(largs.PopWord().AsInt());
    if (largs.GetSize()) m_inputs.Push(largs.PopWord().AsInt());
        
    if ( m_inputs.GetSize() != 3 )
    {
      cerr << "Must have exactly 3 inputs for SetEnvironmentInputs action." << endl;
      exit(1);
    }
    
    if ( (m_inputs[0] >> 24 != 15) || (m_inputs[1] >> 24 != 51) || (m_inputs[2] >> 24 != 85) )
    {
      cerr << "Inputs must begin 0F, 33, 55 for SetEnvironmentInputs" << endl;
      cerr << "They are: " << m_inputs[0] << " " << m_inputs[1] << " " << m_inputs[2] << endl;
      exit(1);    
    }
    
  }
  
  static const cString GetDescription() { return "Arguments: <int input_1> <int input_2> <int input_3> "; }
  
  void Process(cAvidaContext& ctx)
  {
    //First change the environmental inputs
    cEnvironment& env = m_world->GetEnvironment();
    env.SetSpecificInputs(m_inputs);
    
    //Now immediately change the inputs in each cell and
    //clear the input array of each organism so changes take effect
    
    cPopulation& pop = m_world->GetPopulation();
    pop.ResetInputs(ctx);
  }
};



class cActionSetEnvironmentRandomMask : public cAction
{
private:
  unsigned int m_mask;
  
public:
  cActionSetEnvironmentRandomMask(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_mask(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_mask = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int mask>"; }
  
  void Process(cAvidaContext& ctx)
  {
    // First change the environmental input mask
    m_world->GetEnvironment().SetSpecificRandomMask(m_mask);
    
    // Now immediately change the inputs in each cell and clear the input array of each organism so changes take effect
    m_world->GetPopulation().ResetInputs(ctx);
  }
};




class cActionSetTaskArgInt : public cAction
{
private:
  int m_task;
  int m_arg;
  int m_value;
  
public:
  cActionSetTaskArgInt(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_task(0), m_arg(0), m_value(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_task = largs.PopWord().AsInt();
    if (largs.GetSize()) m_arg = largs.PopWord().AsInt();
    if (largs.GetSize()) m_value = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int task> <int arg> <int value>"; }
  
  void Process(cAvidaContext&)
  {
    cEnvironment& env = m_world->GetEnvironment();
    if (m_task >= 0 && m_task < env.GetNumTasks()) {
      env.GetTask(m_task).GetArguments().SetInt(m_arg, m_value);
    } else {
      m_world->GetDriver().Feedback().Error("Task specified in SetTaskArgInt action does not exist");
      m_world->GetDriver().Abort(Avida::INTERNAL_ERROR);
    }
  }
};


/**
 Sets resource availiblity to seasonal
 */
class cActionSetSeasonalResource : public cAction {
private:
	cString m_res_name;

public:
	cActionSetSeasonalResource(cWorld* world, const cString& args, Feedback&): cAction(world, args), m_res_name("")
	{
		cString largs(args);
		if (largs.GetSize()) m_res_name = largs.PopWord();
	}
		
	static const cString GetDescription() { return "Arguments: <string reaction_name>"; }
		
	void Process(cAvidaContext& ctx)
	{
		int time = m_world->GetStats().GetUpdate();
		double m_res_count = -1*(0.4*tanh(((double)time-182500.0)/50000.0)+0.5)*(0.5*sin((double)time/58.091)+0.5)+1;
		cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
		if (res != NULL)
			m_world->GetPopulation().GetResources().SetResource(ctx, res->GetID(), m_res_count);
	}
};

/**
 Sets resource availiblity to seasonal 1 to -1 for 1K years of 365 updates
 */
class cActionSetSeasonalResource1Kyears_1To_1 : public cAction {
private:
	cString m_res_name;
	double m_scale;
		
public:
	cActionSetSeasonalResource1Kyears_1To_1(cWorld* world, const cString& args, Feedback&): cAction(world, args), m_res_name(""), m_scale(1.0)
	{
		cString largs(args);
		if (largs.GetSize()) m_res_name = largs.PopWord();
		if (largs.GetSize()) m_scale = largs.PopWord().AsDouble();
	}
		
	static const cString GetDescription() { return "Arguments: <string reaction_name> <double scale>"; }
	
	void Process(cAvidaContext& ctx)
	{
		int time = m_world->GetStats().GetUpdate();
		double m_res_count = -m_scale*(tanh(((double)time-182500.0)/50000.0)+1.0)*(0.5*sin((double)time/58.091)+0.5)+1.0;
		if(m_res_count < 0.0)
			m_res_count = 0.0;
		cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
		if (res != NULL)
			m_world->GetPopulation().GetResources().SetResource(ctx, res->GetID(), m_res_count);			
	}
};


/**
 Sets resource availiblity to seasonal 1 to -1 for 10K years of 365 updates
 */
class cActionSetSeasonalResource10Kyears_1To_1 : public cAction {
private:
	cString m_res_name;
	double m_scale;
	
public:
	cActionSetSeasonalResource10Kyears_1To_1(cWorld* world, const cString& args, Feedback&): cAction(world, args), m_res_name(""), m_scale(1.0)
	{
		cString largs(args);
		if (largs.GetSize()) m_res_name = largs.PopWord();
		if (largs.GetSize()) m_scale = largs.PopWord().AsDouble();

	}
	
	static const cString GetDescription() { return "Arguments: <string reaction_name> <double scale>"; }
	
	void Process(cAvidaContext& ctx)
	{
		int time = m_world->GetStats().GetUpdate();
		double m_res_count = -m_scale*(tanh(((double)time-1825000.0)/500000.0)+1.0)*(0.5*sin((double)time/58.091)+0.5)+1.0;
		if(m_res_count < 0.0)
			m_res_count = 0.0;
		cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
		if (res != NULL)
			m_world->GetPopulation().GetResources().SetResource(ctx, res->GetID(), m_res_count);			
	}
};


/**
Sets resource availiblity to periodic
 */
class cActionSetPeriodicResource : public cAction
{
private:
  cString m_res_name;
  double m_res_count;
  double amplitude;
  double frequency;
  double phaseShift;
  double initY;

public:
  cActionSetPeriodicResource(cWorld* world, const cString& args, Feedback&): cAction(world, args), m_res_name(""), amplitude(1.0),
                                                                  frequency(1.0), phaseShift(0.0), initY(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) amplitude = largs.PopWord().AsDouble();
    if (largs.GetSize()) frequency = largs.PopWord().AsDouble();
    if (largs.GetSize()) phaseShift = largs.PopWord().AsDouble();
    if (largs.GetSize()) initY = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <string amplitude> <string pi/frequence> <phaseShift*pi> <string initial_Y>"; }
  
  void Process(cAvidaContext& ctx)
  {
    int time = m_world->GetStats().GetUpdate();
    m_res_count = (amplitude * sin(3.14159/frequency * time - phaseShift * 3.14159) + initY) / 2;
    cResourceDef* res = m_world->GetEnvironment().GetResDefLib().GetResDef(m_res_name);
    if (res != NULL) m_world->GetPopulation().GetResources().SetResource(ctx, res->GetID(), m_res_count);

  }
};



class cActionSetTaskArgDouble : public cAction
{
private:
  int m_task;
  int m_arg;
  double m_value;
  
public:
  cActionSetTaskArgDouble(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_task(0), m_arg(0), m_value(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_task = largs.PopWord().AsInt();
    if (largs.GetSize()) m_arg = largs.PopWord().AsInt();
    if (largs.GetSize()) m_value = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <int task> <int arg> <double value>"; }
  
  void Process(cAvidaContext&)
  {
    cEnvironment& env = m_world->GetEnvironment();
    if (m_task >= 0 && m_task < env.GetNumTasks()) {
      env.GetTask(m_task).GetArguments().SetDouble(m_arg, m_value);
    } else {
      m_world->GetDriver().Feedback().Error("Task specified in SetTaskArgDouble action does not exist");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }
};


class cActionSetTaskArgString : public cAction
{
private:
  int m_task;
  int m_arg;
  cString m_value;
  
public:
  cActionSetTaskArgString(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_task(0), m_arg(0), m_value("")
  {
    cString largs(args);
    if (largs.GetSize()) m_task = largs.PopWord().AsInt();
    if (largs.GetSize()) m_arg = largs.PopWord().AsInt();
    if (largs.GetSize()) m_value = largs;
  }
  
  static const cString GetDescription() { return "Arguments: <int task> <int arg> <string value>"; }
  
  void Process(cAvidaContext&)
  {
    cEnvironment& env = m_world->GetEnvironment();
    if (m_task >= 0 && m_task < env.GetNumTasks()) {
      env.GetTask(m_task).GetArguments().SetString(m_arg, m_value);
    } else {
      m_world->GetDriver().Feedback().Error("Task specified in SetTaskArgString action does not exist");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }
};

class cActionSetOptimizeMinMax : public cAction
  {
    
  public:
    cActionSetOptimizeMinMax(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
    
    static const cString GetDescription() { return "No Arguments"; }
    
    void Process(cAvidaContext&)
    {
      cEnvironment& env = m_world->GetEnvironment();
      for (int j = 0; j < env.GetNumTasks(); j++)
      {
        double maxFx = 0.0;
        double minFx = 0.0;
        bool first = true;
        
        for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
          cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
          if (cell.IsOccupied() == false) continue;
          const cPhenotype& phen = cell.GetOrganism()->GetPhenotype();
          double val = phen.GetLastTaskValue()[j];
          if (first) {
            minFx = val;
            maxFx = val;
            first = false;
            continue;
          }
          if (val < minFx) minFx = val;
          if (val > maxFx) maxFx = val;
        }
        env.GetTask(j).GetArguments().SetDouble(1, maxFx);
        env.GetTask(j).GetArguments().SetDouble(2, minFx);
      }
    }
  };




class cActionSetConfig : public cAction
{
private:
  cString m_cvar;
  cString m_value;
  
public:
  cActionSetConfig(cWorld* world, const cString& args, Feedback&) : cAction(world, args)
  {
    cString largs(args);
    if (largs.GetSize()) m_cvar = largs.PopWord();
    if (largs.GetSize()) m_value = largs.PopWord();
    
    if (!m_world->GetConfig().HasEntry(m_cvar)) {
      m_world->GetDriver().Feedback().Error("Config variable specified in SetConfig action exist");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }
  
  static const cString GetDescription() { return "Arguments: <string config_var> <string value>"; }
  
  void Process(cAvidaContext&)
  {
    m_world->GetConfig().Set(m_cvar, m_value);
  }
};

void RegisterEnvironmentActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionInjectResource>("InjectResource");
  action_lib->Register<cActionInjectScaledResource>("InjectScaledResource");
  action_lib->Register<cActionOutflowScaledResource>("OutflowScaledResource");
  action_lib->Register<cActionSetResource>("SetResource");
  action_lib->Register<cZeroResources>("ZeroResources");
  action_lib->Register<cActionSetCellResource>("SetCellResource");
  action_lib->Register<cActionChangeEnvironment>("ChangeEnvironment");
  action_lib->Register<cActionSetDynamicResource>("SetDynamicResource");
  action_lib->Register<cActionSetDynamicResPlateauInflow>("SetDynamicResPlateauInflow");
  action_lib->Register<cActionSetDynamicResPlateauOutflow>("SetDynamicResPlateauOutflow");
  action_lib->Register<cActionSetDynamicResConeInflow>("SetDynamicResConeInflow");
  action_lib->Register<cActionSetDynamicResConeOutflow>("SetDynamicResConeOutflow");
  action_lib->Register<cActionSetDynamicResInflow>("SetDynamicResInflow");
  action_lib->Register<cActionSetDynamicResPlatVarInflow>("SetDynamicResPlatVarInflow");
  action_lib->Register<cActionSetPredatoryResource>("SetPredatoryResource");
  action_lib->Register<cActionSetProbabilisticResource>("SetProbabilisticResource");

  action_lib->Register<cActionSetReactionValue>("SetReactionValue");
  action_lib->Register<cActionSetReactionValueMult>("SetReactionValueMult");
  action_lib->Register<cActionSetReactionInst>("SetReactionInst");
  action_lib->Register<cActionSetReactionMinTaskCount>("SetReactionMinTaskCount");
  action_lib->Register<cActionSetReactionMaxTaskCount>("SetReactionMaxTaskCount");
  action_lib->Register<cActionSetReactionTask>("SetReactionTask");

  action_lib->Register<cActionSetResourceInflow>("SetResourceInflow");
  action_lib->Register<cActionSetResourceOutflow>("SetResourceOutflow");

  action_lib->Register<cActionSetEnvironmentInputs>("SetEnvironmentInputs");
  action_lib->Register<cActionSetEnvironmentRandomMask>("SetEnvironmentRandomMask");

  action_lib->Register<cActionSetSeasonalResource>("SetSeasonalResource");
  action_lib->Register<cActionSetSeasonalResource1Kyears_1To_1>("SetSeasonalResource1Kyears_1To_1");
  action_lib->Register<cActionSetSeasonalResource10Kyears_1To_1>("SetSeasonalResource10Kyears_1To_1");
  action_lib->Register<cActionSetPeriodicResource>("SetPeriodicResource");

  action_lib->Register<cActionSetTaskArgInt>("SetTaskArgInt");
  action_lib->Register<cActionSetTaskArgDouble>("SetTaskArgDouble");
  action_lib->Register<cActionSetTaskArgString>("SetTaskArgString");
  action_lib->Register<cActionSetOptimizeMinMax>("SetOptimizeMinMax");

  action_lib->Register<cActionSetConfig>("SetConfig");
};
