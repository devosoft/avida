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

#include <cmath>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>

#include "avida/core/Feedback.h"
#include "avida/core/Types.h"
#include "cActionLibrary.h"

#include "avida/viewer/OrganismTrace.h"
#include "avida/private/systematics/Genotype.h"


#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cWorld.h"

#include "Driver.h"
#include "WebDebug.h"
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
    //cerr << "webPopulationStats" << endl;
    const cStats& stats = m_world->GetStats();
    int update = stats.GetUpdate();;
    double ave_fitness = stats.GetAveFitness();;
    double ave_gestation_time = stats.GetAveGestation();
    double ave_metabolic_rate = stats.GetAveMerit();
    int org_count = stats.GetNumCreatures();
    double ave_age = stats.GetAveCreatureAge();
    
    /*
    cerr << update << " "
         << ave_fitness << " "
         << ave_gestation_time << " " 
         << ave_metabolic_rate << " "
         << org_count << " " 
         << ave_age << "  ?"
         << stats.GetTaskLastCountSize() << endl;
    */
    
    WebViewerMsg pop_data = {
      {"type", "data"}
      ,{"name", "webPopulationStats"}
      ,{"update", update}
      ,{"ave_fitness", ave_fitness}
      ,{"ave_gestation_time", ave_gestation_time}
      ,{"ave_metabolic_rate", ave_metabolic_rate}
      ,{"organisms", org_count}
      ,{"ave_age", ave_age}
    };
    
    cEnvironment& env = m_world->GetEnvironment();
    for (int t=0; t< env.GetNumTasks(); t++){
      pop_data[string(env.GetTask(t).GetName().GetData())] = 
        stats.GetTaskLastCount(t);
    }
    m_feedback.Data(pop_data.dump().c_str());
    //cerr << "\t\tdone." << endl;
  }
}; // cWebActionPopulationStats


class cWebActionOrgTraceBySequence : public cWebAction
{

  private:
  
    int m_seed;
    double m_mutation_rate;
    string m_sequence;
    
    string Int32ToBinary(unsigned int value){
      string retval = "";
      for (int i = 0; i < 32; i++)
        retval += ( (value >> i) & 1) ? "1" : "0";
      return retval;
    }
    
    json ParseSnapshot(const Viewer::HardwareSnapshot& s)
    {
      const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
      
      cerr << "\tTrace 1" << endl;
      json j;
      j["didDivide"] = s.IsPostDivide();
      j["nextInstruction"] = s.NextInstruction().AsString().GetData();;

      cerr << "\tTrace 2" << endl;
      std::map<std::string, std::string> regs;
      for (int i = 0; i < s.Registers().GetSize(); ++i)
        regs[alphabet.substr(i,1) + "X"] =
          Int32ToBinary(s.Register(i));
      j["registers"] = regs;
      
      cerr << "\tTrace 3" << endl;
      std::map<std::string, std::vector<string>> bufs;
      for (auto it_i = s.Buffers().Begin(); it_i.Next();){
        vector<string> entries;
        for (auto it_j = it_i.Get()->Value2()->Begin(); it_j.Next();){
          entries.push_back(Int32ToBinary(*it_j.Get()));
        }
        bufs[it_i.Get()->Value1().GetData()] = entries;
      }
      j["buffers"] = bufs;
      
      cerr << "\tTrace 4" << endl;
      std::map<std::string, int> functions;
      for (auto it = s.Functions().Begin(); it.Next();)
        functions[it.Get()->Value1().GetData()] = *(it.Get()->Value2());
      j["functions"] = functions;
      
      
      cerr << "\tTrace 5" << endl;
      std::vector<std::map<string,int>> jumps;
      for (auto it = s.Jumps().Begin(); it.Next();){
        std::map<string,int> a_jump;
        const Viewer::HardwareSnapshot::Jump& this_jump = *(it.Get());
        a_jump["fromMemSpace"] = this_jump.from_mem_space;
        a_jump["fromIDX"] = this_jump.from_idx;
        a_jump["toMemSpace"] = this_jump.to_mem_space;
        a_jump["toIDX"] = this_jump.to_idx;
        a_jump["freq"] = this_jump.freq;
        jumps.push_back(a_jump);
      }
      j["jumps"] = jumps;
      
      cerr << "\tTrace 6" << endl;
      std::vector<json> memspace;
      for (auto it = s.MemorySpace().Begin(); it.Next();){
        json this_space;
        this_space["label"] = it.Get()->label;
        vector<string> memory;
        for (auto it_mem = it.Get()->memory.Begin(); it_mem.Next();)
          memory.push_back(it_mem.Get()->GetSymbol().GetData());
        this_space["memory"] = memory;
        vector<int> mutated;
        for (auto it_mutated = it.Get()->mutated.Begin(); it_mutated.Next();)
          mutated.push_back( (*(it_mutated.Get())) ? 1 : 0 );
        this_space["mutated"] = mutated;
        std::map<string, int> heads;
        for (auto it_heads = it.Get()->heads.Begin(); it_heads.Next();)
          heads[it_heads.Get()->Value1().AsLower().GetData()] = *(it_heads.Get()->Value2());
        this_space["heads"] = heads;
        memspace.push_back(this_space);
      }
      j["memSpace"] = memspace;
      cerr << "\tTrace Done." << endl;
      return j;
    }

  public:
    cWebActionOrgTraceBySequence(cWorld* world, const cString& args, Avida::Feedback& m_feedback)
    : cWebAction(world, args, m_feedback)
    {
      cString largs(args);
      if (largs.GetSize()){
        m_sequence = string(largs.PopWord().GetData());
        m_mutation_rate = (largs.GetSize()) ? largs.PopWord().AsDouble() : 0.0;
        m_seed = (largs.GetSize()) ? largs.PopWord().AsInt() : -1;
      } else {
        m_feedback.Warning("webOrgTraceBySequence: a genome sequence is a required argument.");
      }
      m_mutation_rate = (largs.GetSize() > 0) ? largs.PopWord().AsDouble() : 0.0;
    }
    
    static const cString GetDescription() { return "Arguments: GenomeSequence PointMutationRate"; }
    
    void Process(cAvidaContext& ctx)
    {
      cerr << "cWebActionOrtTraceBySequence::Process" << endl;
      //Trace the genome sequence
      GenomePtr genome = 
        GenomePtr(new Genome(Apto::String(m_sequence.c_str())));
      Viewer::OrganismTrace trace(m_world, genome, m_mutation_rate, m_seed);
      
      cerr << "\tTrace ready" << endl;
      vector<WebViewerMsg> snapshots;
      for (int i = 0; i < trace.SnapshotCount(); ++i)
      {
        snapshots.push_back(ParseSnapshot(trace.Snapshot(i)));
      }
      
      cerr << "\tAbout to make feedback" << endl;
      m_feedback.Data(WebViewerMsg(snapshots).dump().c_str());
    }
};  //End cWebActionOrgTraceBySequence



class cWebActionOrgDataByCellID : public cWebAction {
private:
  int m_cell_id;
  
public:
  cWebActionOrgDataByCellID(cWorld* world, const cString& args, Avida::Feedback& fb) : cWebAction(world, args, fb)
  {
    cString largs(args);
    int pop_size = m_world->GetPopulation().GetSize();
    if (largs.GetSize()){
      m_cell_id = largs.PopWord().AsInt();
    } else {
      m_feedback.Warning("webOrgDataByCellID requires a cell_id argument");
    }
    if (m_cell_id > pop_size || m_cell_id < 0){
      m_feedback.Warning("webOrgDataCellID cell_id is not within the appropriate range.");
    }
  }
  static const cString GetDescription() { return "Arguments: cellID"; }
  
  void Process(cAvidaContext& ctx)
  {
    cOrganism* org = m_world->GetPopulation().GetCell(m_cell_id).GetOrganism();
    
    /*WebViewerMsg data = 
      {
        {"type","data"},
        {"name","webOrgDataByCellID"},
        {"orgName","-"},
        {"fitness",std::NaN},
        {"metabolism",std::NaN},
        {"gestation",std::NaN},
        {"age":,std:NaN},
        {"ancestor", "-"},
        {"tasks":
          {
            {"not",0},
            {"nan",0},
            {"and",0},
            {"ornot",0},
            {"or",0},
            {"nor",0},
            {"xor",0},
            {"equ",0}
          }
        }
       m_feedback.Data(data.dump().c_str());*/
  }
}; // cWebActionPopulationStats



class cWebActionGridData : public cWebAction {
  private:

    /*
      Need to use our own min/max_val functions
      because of nans.
    */
    double min_val(const vector<double>& vec)
    {
      if (vec.empty())
        return std::numeric_limits<double>::quiet_NaN();
      double min = vec[0];
      for (auto val : vec){
        if (!isfinite(val))
          continue;
        if (val < min || !isfinite(min))
          min = val;
      }
      return min;
    }
    
    double max_val(const vector<double>& vec)
    {
      if (vec.empty())
        return std::numeric_limits<double>::quiet_NaN();
      double max = vec[0];
      for (auto val : vec){
        if (!isfinite(val))
          continue;
        if (val > max || !isfinite(max))
          max = val;
      }
      return max;
    }
    
  
  public:
    cWebActionGridData(cWorld* world, const cString& args, Avida::Feedback& fb) : cWebAction(world,args,fb)
    {
    }
    
    static const cString GetDescription() { return "Arguments: none";}
    
    void Process(cAvidaContext& ctx)
    {
      WebViewerMsg data = { {"type","data"}, {"name","webGridData"} };
      cPopulation& population = m_world->GetPopulation();
      cEnvironment& env = m_world->GetEnvironment();
      vector<double> fitness;
      vector<double> gestation;
      vector<double> metabolism;
      vector<double> ancestor;
      map<string, vector<double>> tasks;
      double nan = std::numeric_limits<double>::quiet_NaN();
      vector<string> task_names;
      for (int t=0; t<env.GetNumTasks(); t++){
        task_names.push_back(string(env.GetTask(t).GetName().GetData()));
      }
      
      for (int i=0; i < population.GetSize(); i++)
      {
        cPopulationCell& cell = population.GetCell(i);
        cOrganism* org = cell.GetOrganism();
        
        
        if (org == nullptr){
          fitness.push_back(nan);
          gestation.push_back(nan);
          metabolism.push_back(nan);
          ancestor.push_back(nan);
          for (int t=0; t<env.GetNumTasks(); t++){
            if (tasks.find(task_names[t]) == tasks.end())
              tasks[task_names[t]] = vector<double>(1,0.0);
            else
              tasks[task_names[t]].push_back(0.0);
          }          
        } else {
          cPhenotype& phen = org->GetPhenotype();
          fitness.push_back(phen.GetFitness());
          gestation.push_back(phen.GetGestationTime());
          metabolism.push_back(phen.GetMerit().GetDouble());
          ancestor.push_back(org->GetLineageLabel());
          for (int t=0; t<env.GetNumTasks(); t++){
            if (tasks.find(task_names[t]) == tasks.end())
              tasks[task_names[t]] = vector<double>(1,phen.GetCurCountForTask(t));
            else
              tasks[task_names[t]].push_back(phen.GetCurCountForTask(t));
          }          
        }
      }
      data["fitness"] = { 
                  {"data",fitness}, 
                  {"minVal",min_val(fitness)}, 
                  {"maxVal",max_val(fitness)} 
                  };
      data["metabolism"] = {
                  {"data",metabolism}, 
                  {"minVal",min_val(metabolism)}, 
                  {"maxVal",max_val(metabolism)} 
                  };
      data["gestation"] = {
                  {"data",gestation}, 
                  {"minVal",min_val(gestation)}, 
                  {"maxVal",max_val(gestation)} 
                  };

      data["ancestor"] = {
                  {"data",ancestor}, 
                  {"minVal",min_val(ancestor)}, 
                  {"maxVal",max_val(ancestor)} 
                  };
                  
                  
      for (auto it : tasks){
        data[it.first] = {
          {"data",it.second},
          {"minVal",min_val(it.second)},
          {"maxVal",max_val(it.second)}
        };
      }
      cerr << "Size of message is: " << data.dump().size() << endl;
      m_feedback.Data(data.dump().c_str());
    }
};


void RegisterWebDriverActions(cActionLibrary* action_lib)
{
    action_lib->Register<cWebActionPopulationStats>("webPopulationStats");
    action_lib->Register<cWebActionOrgTraceBySequence>("webOrgTraceBySequence");
    action_lib->Register<cWebActionOrgDataByCellID>("webOrgDataByCellID");
    action_lib->Register<cWebActionGridData>("webGridData");
}

#endif
