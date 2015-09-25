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

#include "avida/core/Feedback.h"
#include "avida/core/Types.h"
#include "cActionLibrary.h"

#include "avida/viewer/OrganismTrace.h"
#include "avida/private/systematics/Genotype.h"

#include "cPopulation.h"
#include "cOrganism.h"
#include "cWorld.h"

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
}; // cWebActionPopulationStats


class cWebActionOrgTraceBySequence : public cWebAction
{

  private:
  
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
      json j;
      j["didDivide"] = s.IsPostDivide();
      j["nextInstruction"] = s.NextInstruction().AsString().GetData();;

      std::map<std::string, std::string> regs;
      for (int i = 0; i < s.Registers().GetSize(); ++i)
        regs[alphabet.substr(i,1) + "X"] =
          Int32ToBinary(s.Register(i));
      j["registers"] = regs;
      
      std::map<std::string, std::vector<string>> bufs;
      for (auto it_i = s.Buffers().Begin(); it_i.Next();){
        vector<string> entries;
        for (auto it_j = it_i.Get()->Value2()->Begin(); it_j.Next();){
          entries.push_back(Int32ToBinary(*it_j.Get()));
        }
        bufs[it_i.Get()->Value1().GetData()] = entries;
      }
      j["buffers"] = bufs;
      
      std::map<std::string, int> functions;
      for (auto it = s.Functions().Begin(); it.Next();)
        functions[it.Get()->Value1().GetData()] = *(it.Get()->Value2());
      j["functions"] = functions;
      
      
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
      return j;
    }

  public:
    cWebActionOrgTraceBySequence(cWorld* world, const cString& args, Avida::Feedback& m_feedback)
    : cWebAction(world, args, m_feedback)
    {
      cString largs(args);
      if (largs.GetSize()){
        m_sequence = string(largs.PopWord().GetData());
      } else {
        m_feedback.Warning("webOrgTraceBySequence: a genome sequence is a required argument.");
      }
      m_mutation_rate = (largs.GetSize() > 0) ? largs.PopWord().AsDouble() : 0.0;
    }
    
    static const cString GetDescription() { return "Arguments: GenomeSequence PointMutationRate"; }
    
    void Process(cAvidaContext& ctx)
    {
      
      //Trace the genome sequence
      GenomePtr genome = 
        GenomePtr(new Genome(Apto::String(m_sequence.c_str())));
      Viewer::OrganismTrace trace(m_world, genome, m_mutation_rate);
      
      vector<json> snapshots;
      for (int i = 0; i < trace.SnapshotCount(); ++i)
      {
        snapshots.push_back(ParseSnapshot(trace.Snapshot(i)));
      }
      
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
  public:
    cWebActionGridData(cWorld* world, const cString& args, Avida::Feedback& fb) : cWebAction(world,args,fb)
    {
    }
    
    static const cString GetDescription() { return "Arguments: none";}
    
    void Process(cAvidaContext& ctx)
    {
      WebViewerMsg data = { {"type","data"}, {"name","webGridData"} };
      cPopulation& population = m_world->GetPopulation();
      vector<double> fitness;
      vector<double> gestation;
      vector<double> metabolism;
      vector<double> ancestor;
      vector<double> task_not;
      vector<double> task_nan;
      vector<double> task_and;
      vector<double> task_ornot;
      vector<double> task_andnot;
      vector<double> task_or;
      vector<double> task_nor;
      vector<double> task_xor;
      vector<double> task_equ;
      
      
      for (int i=0; i < population.GetSize(); i++)
      {
        cPopulationCell& cell = population.GetCell(i);
        cOrganism* org = cell.GetOrganism();
        
        
        if (org == nullptr){
          fitness.push_back(nan(""));
          gestation.push_back(nan(""));
          metabolism.push_back(nan(""));
          ancestor.push_back(nan(""));
          task_not.push_back(0);
          task_nan.push_back(0);
          task_and.push_back(0);
          task_ornot.push_back(0);
          task_andnot.push_back(0);
          task_or.push_back(0);
          task_nor.push_back(0);
          task_xor.push_back(0);
          task_equ.push_back(0);
        } else {
          cPhenotype& phen = org->GetPhenotype();
          fitness.push_back(phen.GetFitness());
          gestation.push_back(phen.GetGestationTime());
          metabolism.push_back(phen.GetMerit().GetDouble());
          ancestor.push_back(org->GetLineageLabel());
          task_not.push_back(phen.GetCurCountForTask(0));
          task_nan.push_back(phen.GetCurCountForTask(1));
          task_and.push_back(phen.GetCurCountForTask(2));
          task_ornot.push_back(phen.GetCurCountForTask(3));
          task_andnot.push_back(phen.GetCurCountForTask(4));
          task_or.push_back(phen.GetCurCountForTask(5));
          task_nor.push_back(phen.GetCurCountForTask(6));
          task_xor.push_back(phen.GetCurCountForTask(7));
          task_equ.push_back(phen.GetCurCountForTask(8));
        }
      }
      data["fitness"] = { 
                  {"data",fitness}, 
                  {"minVal",*std::min_element(std::begin(fitness),std::end(fitness))}, 
                  {"maxVal",*std::max_element(std::begin(fitness),std::end(fitness))} 
                  };
      data["metabolism"] = {
                  {"data",metabolism}, 
                  {"minVal",*std::min_element(std::begin(metabolism),std::end(metabolism))}, 
                  {"maxVal",*std::max_element(std::begin(metabolism),std::end(metabolism))} 
                  };
      data["gestation"] = {
                  {"data",gestation}, 
                  {"minVal",*std::min_element(std::begin(gestation),std::end(gestation))}, 
                  {"maxVal",*std::max_element(std::begin(gestation),std::end(gestation))} 
                  };

      data["ancestor"] = {
                  {"data",ancestor}, 
                  {"minVal",*std::min_element(std::begin(ancestor),std::end(ancestor))}, 
                  {"maxVal",*std::max_element(std::begin(ancestor),std::end(ancestor))} 
                  };

      data["not"] =  {
                  {"data",task_not}, 
                  {"minVal",*std::min_element(std::begin(task_not),std::end(task_not))}, 
                  {"maxVal",*std::max_element(std::begin(task_not),std::end(task_not))} 
                  };

      data["nan"] =  {
                  {"data",task_nan}, 
                  {"minVal",*std::min_element(std::begin(task_nan),std::end(task_nan))}, 
                  {"maxVal",*std::max_element(std::begin(task_nan),std::end(task_nan))} 
                  };
      data["and"] = {
                  {"data",task_and}, 
                  {"minVal",*std::min_element(std::begin(task_and),std::end(task_and))}, 
                  {"maxVal",*std::max_element(std::begin(task_and),std::end(task_and))} 
                  };
      data["ornot"] = {
                  {"data",task_ornot}, 
                  {"minVal",*std::min_element(std::begin(task_ornot),std::end(task_ornot))}, 
                  {"maxVal",*std::max_element(std::begin(task_ornot),std::end(task_ornot))} 
                  };
      data["andnot"] = {
                  {"data",task_andnot}, 
                  {"minVal",*std::min_element(std::begin(task_andnot),std::end(task_andnot))}, 
                  {"maxVal",*std::max_element(std::begin(task_andnot),std::end(task_andnot))} 
                  };
      data["or"] = {
                  {"data",task_or}, 
                  {"minVal",*std::min_element(std::begin(task_or),std::end(task_or))}, 
                  {"maxVal",*std::max_element(std::begin(task_or),std::end(task_or))} 
                  };
      data["nor"] =  {
                  {"data",task_nor}, 
                  {"minVal",*std::min_element(std::begin(task_nor),std::end(task_nor))}, 
                  {"maxVal",*std::max_element(std::begin(task_nor),std::end(task_nor))} 
                  };
      data["xor"] = {
                  {"data",task_xor}, 
                  {"minVal",*std::min_element(std::begin(task_xor),std::end(task_xor))}, 
                  {"maxVal",*std::max_element(std::begin(task_xor),std::end(task_xor))} 
                  };
      data["equ"] = {
                  {"data",task_equ}, 
                  {"minVal",*std::min_element(std::begin(task_equ),std::end(task_equ))}, 
                  {"maxVal",*std::max_element(std::begin(task_equ),std::end(task_equ))} 
                  };
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
