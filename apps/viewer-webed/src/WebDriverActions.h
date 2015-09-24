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

#include "avida/viewer/OrganismTrace.h"
#include "avida/private/systematics/Genotype.h"

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

};



void RegisterWebDriverActions(cActionLibrary* action_lib)
{
    action_lib->Register<cWebActionPopulationStats>("webPopulationStats");
    action_lib->Register<cWebActionOrgTraceBySequence>("webOrgTraceBySequence");
}

#endif
