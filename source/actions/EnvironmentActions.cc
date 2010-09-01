/*
 *  EnvironmentActions.cc
 *  Avida
 *
 *  Created by David Bryson on 8/9/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation, 
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "math.h"
#include "EnvironmentActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cResource.h"
#include "cStats.h"
#include "cWorld.h"

class cActionInjectResource : public cAction
{
private:
  cString m_res_name;
  double m_res_count;
  
public:
  cActionInjectResource(cWorld* world, const cString& args) : cAction(world, args), m_res_name(""), m_res_count(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
  }

  static const cString GetDescription() { return "Arguments: <string res_name> <double res_count>"; }

  void Process(cAvidaContext& ctx)
  {
    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
    if (res != NULL) m_world->GetPopulation().UpdateResource(res->GetID(), m_res_count);
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
  cActionInjectScaledResource(cWorld* world, const cString& args) : cAction(world, args), m_res_name(""), m_res_count(0.0)
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

    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
    if (res != NULL) m_world->GetPopulation().UpdateResource(res->GetID(), (m_res_count / ave_merit));
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
  cActionOutflowScaledResource(cWorld* world, const cString& args) : cAction(world, args), m_res_name(""), m_res_percent(0.0)
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

    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
    
    double res_level = m_world->GetPopulation().GetResource(res->GetID());
    double scaled_perc = 1 / (1 + ave_merit * (1 - m_res_percent) / m_res_percent);
    res_level -= res_level * scaled_perc;
    
    if (res != NULL) m_world->GetPopulation().UpdateResource(res->GetID(), res_level);
  }
};

/* Change the amount of a particular global resource */

class cActionSetResource : public cAction
{
private:
  cString m_res_name;
  double m_res_count;
  
public:
  cActionSetResource(cWorld* world, const cString& args) : cAction(world, args), m_res_name(""), m_res_count(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string res_name> <double res_count>"; }
  
  void Process(cAvidaContext& ctx)
  {
    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
    if (res != NULL) m_world->GetPopulation().SetResource(res->GetID(), m_res_count);
  }
};

class cActionSetDemeResource : public cAction
	{
	private:
		cString m_res_name;
		double m_res_count;
		
	public:
		cActionSetDemeResource(cWorld* world, const cString& args) : cAction(world, args), m_res_name(""), m_res_count(0.0)
		{
			cString largs(args);
			if (largs.GetSize()) m_res_name = largs.PopWord();
			if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
		}
		
		static const cString GetDescription() { return "Arguments: <string res_name> <double res_count>"; }
		
		void Process(cAvidaContext& ctx)
		{
			cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
			if (res != NULL) {
				cPopulation& pop = m_world->GetPopulation();
				int numDemes =  pop.GetNumDemes();
				for(int demeID = 0; demeID < numDemes; ++demeID) {
					cDeme& deme = pop.GetDeme(demeID);
					deme.SetResource(res->GetID(), m_res_count);
				}
			}
		}
	};

class cZeroResources : public cAction
{
private:
  
public:
  cZeroResources(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
  }
  
  static const cString GetDescription() { return "Arguments: none"; }
  
  void Process(cAvidaContext& ctx)
  {
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    for (int i=0; i < res_lib.GetSize(); i++)  {
      cResource* res = res_lib.GetResource(i);
      m_world->GetPopulation().SetResource(res->GetID(), 0.0);
    }
  }
};

/* Change the amount of a particular resource in a particular cells */

class cActionSetCellResource : public cAction
{
private:
  tArray<int> m_cell_list;
  cString m_res_name;
  double m_res_count;
  int m_res_id;
  
public:
  cActionSetCellResource(cWorld* world, const cString& args) : cAction(world, args), m_cell_list(0), m_res_name(""), m_res_count(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) 
    {
      cString s = largs.PopWord();
      m_cell_list = cStringUtil::ReturnArray(s);    
    }
    if (largs.GetSize()) m_res_name = largs.PopWord();
    if (largs.GetSize()) m_res_count = largs.PopWord().AsDouble();
    
    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
    assert(res);
    m_res_id = res->GetID(); // Save the id so we don't have to do many string conversions
  }

  static const cString GetDescription() { return "Arguments: <int cell_id> <string res_name> <double res_count>"; }

  void Process(cAvidaContext& ctx)
  {
    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_id);
    for(int i=0; i<m_cell_list.GetSize(); i++)
    {
      int m_cell_id = m_cell_list[i];
      tArray<double> counts = m_world->GetPopulation().GetResourceCount().GetCellResources(m_cell_id);
      if ((res != NULL) && (res->GetID() < counts.GetSize()))
      {
        counts[res->GetID()] = m_res_count;
        m_world->GetPopulation().GetResourceCount().SetCellResources(m_cell_id, counts);
      }
    }
  }
};


/* Change Environment settings */

class cActionChangeEnvironment : public cAction
{
private:
  cString env_string;
  
public:
  cActionChangeEnvironment(cWorld* world, const cString& args) : cAction(world, args), env_string("")
  {
    cString largs(args);
    if (largs.GetSize()) env_string = largs;
  }
  
  static const cString GetDescription() { return "Arguments: <string env_string>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().LoadLine(env_string);
    m_world->GetPopulation().UpdateResourceCount(m_world->GetVerbosity());
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
  cActionSetReactionValue(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_value(0.0)
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
  cActionSetReactionValueMult(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_value(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_value = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <double value>"; }
  
  void Process(cAvidaContext& ctx)
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
  cActionSetReactionInst(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_inst("")
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_inst = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <string inst>"; }
  
  void Process(cAvidaContext& ctx)
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
  cActionSetReactionMinTaskCount(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_min_count(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_min_count = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <int min_count>"; }
  
  void Process(cAvidaContext& ctx)
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
  cActionSetReactionMaxTaskCount(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_max_count(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_max_count = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <int max_count>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().SetReactionMaxTaskCount(m_name, m_max_count);
  }
};

class cActionSetReactionTask : public cAction
{
private:
  cString m_name;
  cString m_task;
  
public:
  cActionSetReactionTask(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_task("")
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_task = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <string reaction_name> <string task_name>"; }
  
  void Process(cAvidaContext& ctx)
  {
    bool success = m_world->GetEnvironment().SetReactionTask(m_name, m_task);
    if (!success) m_world->GetDriver().RaiseFatalException(-2,"SetReactionTask action failed");
  }
};

class cActionSetResourceInflow : public cAction
{
private:
  cString m_name;
  double m_inflow;
  
public:
  cActionSetResourceInflow(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_inflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_inflow = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <int inflow>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().SetResourceInflow(m_name, m_inflow);
      //This doesn't actually update the rate in the population, so...
    m_world->GetPopulation().GetResourceCount().SetInflow(m_name, m_inflow);
  }
};

/* Set the inflow of a given deme.  Currently only works for global (deme) resources.
 
 Parameters:
 deme id - the deme whose resource to set
 name - the name of the resource
 inflow - value to set as amount of resource added
 
 */

class cActionSetDemeResourceInflow : public cAction
  {
  private:
    int m_demeid;
    cString m_name;
    double m_inflow;
    
  public:
    cActionSetDemeResourceInflow(cWorld* world, const cString& args) : cAction(world, args), m_demeid(-1), m_name(""), m_inflow(0.0)
    {
      cString largs(args);
      if (largs.GetSize()) m_demeid = largs.PopWord().AsInt();
      if (largs.GetSize()) m_name = largs.PopWord();
      if (largs.GetSize()) m_inflow = largs.PopWord().AsDouble();
      
      assert(m_inflow >= 0);
      assert(m_demeid >= 0);
      assert(m_demeid < m_world->GetConfig().NUM_DEMES.Get());
      
    }
    
    static const cString GetDescription() { return "Arguments: <int deme id> <string resource_name> <int inflow>"; }
    
    void Process(cAvidaContext& ctx)
    {
      m_world->GetEnvironment().SetResourceInflow(m_name, m_inflow);
      //This doesn't actually update the rate in the population, so...
      m_world->GetPopulation().GetDeme(m_demeid).GetDemeResources().SetInflow(m_name, m_inflow);
    }
  };


class cActionSetResourceOutflow : public cAction
{
private:
  cString m_name;
  double m_outflow;
  
public:
  cActionSetResourceOutflow(cWorld* world, const cString& args) : cAction(world, args), m_name(""), m_outflow(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_name = largs.PopWord();
    if (largs.GetSize()) m_outflow = largs.PopWord().AsDouble();
    assert(m_outflow <= 1.0);
    assert(m_outflow >= 0.0);
  }
  
  static const cString GetDescription() { return "Arguments: <string resource_name> <int outflow>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetEnvironment().SetResourceOutflow(m_name, m_outflow);
      //This doesn't actually update the rate in the population, so...
    m_world->GetPopulation().GetResourceCount().SetDecay(m_name, 1-m_outflow);
  }
};

/* Set the outflow (decay) of a given deme.  Currently only works for global (deme) resources.
 
Parameters:
   deme id - the deme whose resource to set
   name - the name of the resource
   outflow - value to set as percentage of the resource decayed continuously
 
*/

class cActionSetDemeResourceOutflow : public cAction
  {
  private:
    int m_demeid;
    cString m_name;
    double m_outflow;
    
  public:
    cActionSetDemeResourceOutflow(cWorld* world, const cString& args) : cAction(world, args), m_demeid(-1), m_name(""), m_outflow(0.0)
    {
      cString largs(args);
      if (largs.GetSize()) m_demeid = largs.PopWord().AsInt();
      if (largs.GetSize()) m_name = largs.PopWord();
      if (largs.GetSize()) m_outflow = largs.PopWord().AsDouble();
      assert(m_demeid >= 0);
      assert(m_outflow <= 1.0);
      assert(m_outflow >= 0.0);
      assert(m_demeid < m_world->GetConfig().NUM_DEMES.Get());
    }
    
    static const cString GetDescription() { return "Arguments: <int deme id> <string resource_name> <int outflow>"; }
    
    void Process(cAvidaContext& ctx)
    {
      m_world->GetEnvironment().SetResourceOutflow(m_name, m_outflow);
      //This doesn't actually update the rate in the population, so...
      m_world->GetPopulation().GetDeme(m_demeid).GetDemeResources().SetDecay(m_name, 1-m_outflow);
      
    }
  };


class cActionSetEnvironmentInputs : public cAction
{
private:
  tArray<int> m_inputs;
  
public:
  cActionSetEnvironmentInputs(cWorld* world, const cString& args) : cAction(world, args), m_inputs()
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
  cActionSetEnvironmentRandomMask(cWorld* world, const cString& args) : cAction(world, args), m_mask(0)
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
  cActionSetTaskArgInt(cWorld* world, const cString& args) : cAction(world, args), m_task(0), m_arg(0), m_value(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_task = largs.PopWord().AsInt();
    if (largs.GetSize()) m_arg = largs.PopWord().AsInt();
    if (largs.GetSize()) m_value = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int task> <int arg> <int value>"; }
  
  void Process(cAvidaContext& ctx)
  {
    cEnvironment& env = m_world->GetEnvironment();
    if (m_task >= 0 && m_task < env.GetNumTasks()) {
      env.GetTask(m_task).GetArguments().SetInt(m_arg, m_value);
    } else {
      m_world->GetDriver().RaiseFatalException(-2,"Task specified in SetTaskArgInt action does not exist");
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
	cActionSetSeasonalResource(cWorld* world, const cString& args): cAction(world, args), m_res_name("")
	{
		cString largs(args);
		if (largs.GetSize()) m_res_name = largs.PopWord();
	}
		
	static const cString GetDescription() { return "Arguments: <string reaction_name>"; }
		
	void Process(cAvidaContext& ctx)
	{
		int time = m_world->GetStats().GetUpdate();
		double m_res_count = -1*(0.4*tanh(((double)time-182500.0)/50000.0)+0.5)*(0.5*sin((double)time/58.091)+0.5)+1;
		cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
		if (res != NULL)
			m_world->GetPopulation().SetResource(res->GetID(), m_res_count);
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
	cActionSetSeasonalResource1Kyears_1To_1(cWorld* world, const cString& args): cAction(world, args), m_res_name(""), m_scale(1.0)
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
		cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
		if (res != NULL)
			m_world->GetPopulation().SetResource(res->GetID(), m_res_count);			
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
	cActionSetSeasonalResource10Kyears_1To_1(cWorld* world, const cString& args): cAction(world, args), m_res_name(""), m_scale(1.0)
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
		cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
		if (res != NULL)
			m_world->GetPopulation().SetResource(res->GetID(), m_res_count);			
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
  cActionSetPeriodicResource(cWorld* world, const cString& args): cAction(world, args), m_res_name(""), amplitude(1.0),
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
    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
    if (res != NULL) m_world->GetPopulation().SetResource(res->GetID(), m_res_count);

  }
};


/**
Sets energy model config value NumInstBefore0Energy 
 */

class cActionSetNumInstBefore0Energy : public cAction
{
private:
  int newValue;

public:
  cActionSetNumInstBefore0Energy(cWorld* world, const cString& args) : cAction(world, args), newValue(0)
  {
    cString largs(args);
    if (largs.GetSize()) newValue = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int new_value>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetConfig().NUM_CYCLES_EXC_BEFORE_0_ENERGY.Set(newValue);
  }
};

/*
class cActionSetDoublePeriodicResource : public cAction
{
private:
  cString m_res_name;
  double m_res_count;
  double amplitude;
  double frequency;
  double phaseShift;
  double initY;

public:
  cActionSetDoublePeriodicResource(cWorld* world, const cString& args): cAction(world, args), m_res_name(""), amplitude(1.0),
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
    m_res_count = ampliture*(sin(pi/frequency1*x-pi*phaseShift1)+1+cos(pi/frequency2*x-pi*phaseShift1)+1)/4;
    
    std::cout << "Update " << time << " Y = " << m_res_count << std::endl;
//    std::cout << m_res_count <<" = " << amplitude <<" * sin("<<frequency <<" * " << time <<" - "<< phaseShift<<") + "<<initY<<std::endl;
    cResource* res = m_world->GetEnvironment().GetResourceLib().GetResource(m_res_name);
    if (res != NULL) m_world->GetPopulation().SetResource(res->GetID(), m_res_count);

  }
};
*/

class cActionSetTaskArgDouble : public cAction
{
private:
  int m_task;
  int m_arg;
  double m_value;
  
public:
  cActionSetTaskArgDouble(cWorld* world, const cString& args) : cAction(world, args), m_task(0), m_arg(0), m_value(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_task = largs.PopWord().AsInt();
    if (largs.GetSize()) m_arg = largs.PopWord().AsInt();
    if (largs.GetSize()) m_value = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <int task> <int arg> <double value>"; }
  
  void Process(cAvidaContext& ctx)
  {
    cEnvironment& env = m_world->GetEnvironment();
    if (m_task >= 0 && m_task < env.GetNumTasks()) {
      env.GetTask(m_task).GetArguments().SetDouble(m_arg, m_value);
    } else {
      m_world->GetDriver().RaiseFatalException(-2,"Task specified in SetTaskArgDouble action does not exist");
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
  cActionSetTaskArgString(cWorld* world, const cString& args) : cAction(world, args), m_task(0), m_arg(0), m_value("")
  {
    cString largs(args);
    if (largs.GetSize()) m_task = largs.PopWord().AsInt();
    if (largs.GetSize()) m_arg = largs.PopWord().AsInt();
    if (largs.GetSize()) m_value = largs;
  }
  
  static const cString GetDescription() { return "Arguments: <int task> <int arg> <string value>"; }
  
  void Process(cAvidaContext& ctx)
  {
    cEnvironment& env = m_world->GetEnvironment();
    if (m_task >= 0 && m_task < env.GetNumTasks()) {
      env.GetTask(m_task).GetArguments().SetString(m_arg, m_value);
    } else {
      m_world->GetDriver().RaiseFatalException(-2,"Task specified in SetTaskArgString action does not exist");
    }
  }
};

class cActionSetOptimizeMinMax : public cAction
  {
    
  public:
    cActionSetOptimizeMinMax(cWorld* world, const cString& args) : cAction(world, args) { ; }
    
    static const cString GetDescription() { return "No Arguments"; }
    
    void Process(cAvidaContext& ctx)
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


class cActionDelayedDemeEvent : public cAction
{
private:
  int m_x1, m_y1, m_x2, m_y2; // bounding box of event in deme
  int m_delay; // deme age when event occurs
  int m_duration; // length of event; subverted when deme is reset
  bool m_static_pos;
  int m_total_events; // total number of unique event to create; they may overlab
  bool m_static_position;
  
public:
  cActionDelayedDemeEvent(cWorld* world, const cString& args) : 
    cAction(world, args)
  , m_x1(-1)
  , m_y1(-1)
  , m_x2(-1)
  , m_y2(-1)
  , m_delay(-1)
  , m_duration(-1)
  , m_total_events(1)
  , m_static_position(true)
  {
    cString largs(args);
    if (largs.GetSize()) m_x1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_y1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_x2 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_y2 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_delay = largs.PopWord().AsInt();
    if (largs.GetSize()) m_duration = largs.PopWord().AsInt();
    if (largs.GetSize()) m_static_position = static_cast<bool>(largs.PopWord().AsInt());
    if (largs.GetSize()) m_total_events = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int x1> <int y1> <int x2> <int y2> <int delay> <int duraion> <bool static_position> <int total_events>"; }
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& pop = m_world->GetPopulation();
    int numDemes = pop.GetNumDemes();
    for(int i = 0; i < numDemes; i++) {
      pop.GetDeme(i).SetCellEvent(m_x1, m_y1, m_x2, m_y2, m_delay, m_duration, m_static_position, m_total_events);
    }
  }
};

class cActionDelayedDemeEventsPerSlots : public cAction
{
private:
  int m_x1, m_y1, m_x2, m_y2; // bounding box of event in deme
  int m_delay; // deme age when event occurs
  int m_duration; // length of event; subverted when deme is reset
  int m_total_slots; // total number of slots
  int m_total_events_per_slot_max; // maximum number of unique event to create per slot; they may overlab
  int m_total_events_per_slot_min; // minimum number of unique event to create per slot; they may overlab
  int m_tolal_event_flow_levels; // total number of evenly spaced event flow levels; not all flow levels will be represented in a single deme
  bool m_static_position;
  
public:
  cActionDelayedDemeEventsPerSlots(cWorld* world, const cString& args) : 
    cAction(world, args)
  , m_x1(-1)
  , m_y1(-1)
  , m_x2(-1)
  , m_y2(-1)
  , m_delay(-1)
  , m_duration(-1)
  , m_total_slots(1)
  , m_total_events_per_slot_max(1)
  , m_total_events_per_slot_min(1)
  , m_tolal_event_flow_levels(1)
  , m_static_position(true)
  {
    cString largs(args);
    if (largs.GetSize()) m_x1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_y1 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_x2 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_y2 = largs.PopWord().AsInt();
    if (largs.GetSize()) m_delay = largs.PopWord().AsInt();
    if (largs.GetSize()) m_duration = largs.PopWord().AsInt();
    if (largs.GetSize()) m_static_position = static_cast<bool>(largs.PopWord().AsInt());
    if (largs.GetSize()) m_total_slots = largs.PopWord().AsInt();
    if (largs.GetSize()) m_total_events_per_slot_max = largs.PopWord().AsInt();
    if (largs.GetSize()) m_total_events_per_slot_min = largs.PopWord().AsInt();
    if (largs.GetSize()) m_tolal_event_flow_levels = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <int x1> <int y1> <int x2> <int y2> <int delay> <int duraion> <bool static_position> <int total_slots_per_deme> <int total_events_per_slot_max> <int total_events_per_slot_min> <int tolal_event_flow_levels>"; }
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& pop = m_world->GetPopulation();
    int numDemes = pop.GetNumDemes();
    for(int i = 0; i < numDemes; i++) {
      pop.GetDeme(i).SetCellEventSlots(m_x1, m_y1, m_x2, m_y2, m_delay, m_duration, m_static_position, m_total_slots, m_total_events_per_slot_max, m_total_events_per_slot_min, m_tolal_event_flow_levels);
    }
  }
};

class cActionSetFracDemeTreatable : public cAction {
private:
	double factionTreatable; // total number of unique event to create; they may overlab
	
public:
	cActionSetFracDemeTreatable(cWorld* world, const cString& args) : cAction(world, args), factionTreatable(0.0) {
		cString largs(args);
		if (largs.GetSize()) factionTreatable = largs.PopWord().AsDouble();
	}
	
	static const cString GetDescription() { return "Arguments: <double factionTreatable>"; }
	
	void Process(cAvidaContext& ctx) {
		cPopulation& pop = m_world->GetPopulation();
		int numDemes = pop.GetNumDemes();
		for(int i = 0; i < numDemes; i++) {
			if(ctx.GetRandom().P(factionTreatable))
				pop.GetDeme(i).setTreatable(true);
			else
				pop.GetDeme(i).setTreatable(false);
		}
	}
};

class cActionSetConfig : public cAction
{
private:
  cString m_cvar;
  cString m_value;
  
public:
  cActionSetConfig(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs.GetSize()) m_cvar = largs.PopWord();
    if (largs.GetSize()) m_value = largs.PopWord();
    
    if (!m_world->GetConfig().HasEntry(m_cvar))
      m_world->GetDriver().RaiseFatalException(-2, "Config variable specified in SetConfig action exist");
  }
  
  static const cString GetDescription() { return "Arguments: <string config_var> <string value>"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetConfig().Set(m_cvar, m_value);
  }
};



void RegisterEnvironmentActions(cActionLibrary* action_lib)
{
	action_lib->Register<cActionSetFracDemeTreatable>("SetFracDemeTreatable");
  action_lib->Register<cActionDelayedDemeEvent>("DelayedDemeEvent");
  action_lib->Register<cActionDelayedDemeEventsPerSlots>("DelayedDemeEventsPerSlots");
  action_lib->Register<cActionInjectResource>("InjectResource");
  action_lib->Register<cActionInjectScaledResource>("InjectScaledResource");
  action_lib->Register<cActionOutflowScaledResource>("OutflowScaledResource");
  action_lib->Register<cActionSetResource>("SetResource");
  action_lib->Register<cActionSetDemeResource>("SetDemeResource");
  action_lib->Register<cZeroResources>("ZeroResources");
  action_lib->Register<cActionSetCellResource>("SetCellResource");
  action_lib->Register<cActionChangeEnvironment>("ChangeEnvironment");

  action_lib->Register<cActionSetReactionValue>("SetReactionValue");
  action_lib->Register<cActionSetReactionValueMult>("SetReactionValueMult");
  action_lib->Register<cActionSetReactionInst>("SetReactionInst");
  action_lib->Register<cActionSetReactionMinTaskCount>("SetReactionMinTaskCount");
  action_lib->Register<cActionSetReactionMaxTaskCount>("SetReactionMaxTaskCount");
  action_lib->Register<cActionSetReactionTask>("SetReactionTask");

  action_lib->Register<cActionSetResourceInflow>("SetResourceInflow");
  action_lib->Register<cActionSetResourceOutflow>("SetResourceOutflow");
  action_lib->Register<cActionSetDemeResourceInflow>("SetDemeResourceInflow");
  action_lib->Register<cActionSetDemeResourceOutflow>("SetDemeResourceOutflow");

  action_lib->Register<cActionSetEnvironmentInputs>("SetEnvironmentInputs");
  action_lib->Register<cActionSetEnvironmentRandomMask>("SetEnvironmentRandomMask");

	action_lib->Register<cActionSetSeasonalResource>("SetSeasonalResource");
	action_lib->Register<cActionSetSeasonalResource1Kyears_1To_1>("SetSeasonalResource1Kyears_1To_1");
	action_lib->Register<cActionSetSeasonalResource10Kyears_1To_1>("SetSeasonalResource10Kyears_1To_1");
  action_lib->Register<cActionSetPeriodicResource>("SetPeriodicResource");
  action_lib->Register<cActionSetNumInstBefore0Energy>("SetNumInstBefore0Energy");

  action_lib->Register<cActionSetTaskArgInt>("SetTaskArgInt");
  action_lib->Register<cActionSetTaskArgDouble>("SetTaskArgDouble");
  action_lib->Register<cActionSetTaskArgString>("SetTaskArgString");
  action_lib->Register<cActionSetOptimizeMinMax>("SetOptimizeMinMax");
  
  action_lib->Register<cActionSetConfig>("SetConfig");
}
