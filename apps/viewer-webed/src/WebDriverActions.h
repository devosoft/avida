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
#include <cstring>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <sstream>
#include <fstream>


#include "avida/core/Feedback.h"
#include "avida/core/Types.h"
#include "avida/environment/Manager.h"
#include "cActionLibrary.h"

#include "avida/viewer/OrganismTrace.h"
#include "avida/private/systematics/Genotype.h"
#include "avida/private/systematics/Clade.h"

#include "apto/core/FileSystem.h"

#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cWorld.h"

#include "Driver.h"
#include "WebDebug.h"
#include "WebDriverActions.h"
#include "Types.h"

using namespace Avida::WebViewer;







class cWebAction : public cAction
{
  protected:
    Avida::Feedback& m_feedback;
    bool m_json_args;
    
    /*
      Return a json object
      If the argument string contains a UNIT_SEP as the first
      character, treat the arguments as a JSON object.  If
      the arguments are not a json object, return an empty json
      object.
    */
    static json GetJSONArgs()
    {
      if (m_json_args)
        return json::parse(args.c_str()[1]);
      return json j;
    }
    
  public:
    cWebAction(cWorld* world, const cString& args, Avida::Feedback& fb) 
    : cAction(world,args) 
    , m_feedback(fb) 
    {
      m_json_args = (args.GetSize() == 0 || args[0] == UNIT_SEP);
    }
};




class cWebActionPopulationStats : public cWebAction {
public:
  cWebActionPopulationStats(cWorld* world, const cString& args, Avida::Feedback& fb) : cWebAction(world, args, fb)
  {
  }
  static const cString GetDescription() { return "no arguments"; }
  void Process(cAvidaContext& ctx){
    D_(D_ACTIONS, "cWebActionPopulationStats::Processs");
    const cStats& stats = m_world->GetStats();
    int update = stats.GetUpdate();;
    double ave_fitness = stats.GetAveFitness();;
    double ave_gestation_time = stats.GetAveGestation();
    double ave_metabolic_rate = stats.GetAveMerit();
    int org_count = stats.GetNumCreatures();
    double ave_age = stats.GetAveCreatureAge();
    
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
    D_(D_ACTIONS, "cWebActionPopulationStats::Process [completed]");
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
      
      json j;
      j["didDivide"] = s.IsPostDivide();
      j["nextInstruction"] = s.NextInstruction().AsString().GetData();;

      std::map<std::string, std::string> regs;
      for (int i = 0; i < s.Registers().GetSize(); ++i)
        regs[alphabet.substr(i,1) + "x"] =
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
        m_mutation_rate = (largs.GetSize()) ? largs.PopWord().AsDouble() : 0.0;
        m_seed = (largs.GetSize()) ? largs.PopWord().AsInt() : -1;
      } else {
        m_feedback.Warning("webOrgTraceBySequence: a genome sequence is a required argument.");
      }
    }
    
    static const cString GetDescription() { return "Arguments: GenomeSequence PointMutationRate Seed"; }
    
    void Process(cAvidaContext& ctx)
    {
      D_(D_ACTIONS, "cWebActionOrtTraceBySequence::Process" );

      //Trace the genome sequence
      GenomePtr genome = 
        GenomePtr(new Genome(Apto::String( (m_sequence).c_str())));
      D_(D_ACTIONS, "\tAbout to Trace with settings " << m_world
           <<  "," << m_sequence << "," << m_mutation_rate << "," << m_seed, 1 );
      Viewer::OrganismTrace trace(m_world, genome, m_mutation_rate, m_seed);
      
      D_(D_ACTIONS, "Trace ready.", 1);
      vector<json> snapshots;
      WebViewerMsg retval = { 
                        {"type","data"},
                        {"name","webOrgTraceBySequence"}
                        };
                        
      for (int i = 0; i < trace.SnapshotCount(); ++i)
      {
        snapshots.push_back(ParseSnapshot(trace.Snapshot(i)));
      }
      retval["snapshots"] = snapshots;
      
      D_(D_ACTIONS, "cWebOrgTraceBySequence: About to make feedback",1);
      m_feedback.Data(WebViewerMsg(retval).dump().c_str());
      D_(D_ACTIONS, "cWebOrgTraceBySequence::Process completed");
    }
};  //End cWebActionOrgTraceBySequence



class cWebActionOrgDataByCellID : public cWebAction {
private:
  static constexpr int NO_SELECTION = -1;
  int m_cell_id;
  
  double GetTestFitness(Systematics::GenotypePtr& gptr) 
  {
    Apto::RNG::AvidaRNG rng(100); 
    cAvidaContext ctx(&m_world->GetDriver(),rng); 
    return Systematics::GenomeTestMetrics::GetMetrics(m_world,ctx,gptr)->GetFitness();
  }
  
  double GetTestMerit(Systematics::GenotypePtr& gptr) 
  {
    Apto::RNG::AvidaRNG rng(100); 
    cAvidaContext ctx(&m_world->GetDriver(),rng); 
    return Systematics::GenomeTestMetrics::GetMetrics(m_world,ctx,gptr)->GetMerit();
  }
  
  double GetTestGestationTime(Systematics::GenotypePtr& gptr) 
  {
    Apto::RNG::AvidaRNG rng(100); 
    cAvidaContext ctx(&m_world->GetDriver(),rng); 
    return Systematics::GenomeTestMetrics::GetMetrics(m_world,ctx,gptr)->GetGestationTime();
  }
  
  int GetTestTaskCount(Systematics::GenotypePtr& gptr, cString& task_name)
  {
    Apto::RNG::AvidaRNG rng(100); 
    cAvidaContext ctx(&m_world->GetDriver(),rng); 
    Environment::ManagerPtr env = Avida::Environment::Manager::Of(m_world->GetNewWorld());
    return Systematics::GenomeTestMetrics::GetMetrics(m_world,ctx,gptr)->GetTaskCounts()[env->GetActionTrigger(task_name.GetData())->TempOrdering()];
  }
  
  
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
    D_(D_ACTIONS, "cWebActionOrgDataByCellID::Process");
    D_(D_ACTIONS, "Selected cellID: " << m_cell_id, 1);
    if (m_cell_id == NO_SELECTION) 
    { 
      D_(D_ACTIONS, "cWebActionOrgDataByCellID::Process completed.");
      return;
    }
    cOrganism* org = m_world->GetPopulation().GetCell(m_cell_id).GetOrganism();
    
  
    WebViewerMsg data = { {"type","data"}, {"name","webOrgDataByCellID"} };
    
    if (org == nullptr){
      data["genotypeName"] = "-";
      data["fitness"] = NaN;
      data["metabolism"] = NaN;
      data["gestation"] = NaN;
      data["age"] = NaN;
      data["ancestor"] = NaN;
      data["genome"] = "";
      data["isEstimate"] = false;
      data["tasks"] = {};
    } else {
      // We're going to emulate AvidaEd OSX right now and go through the
      // genotype rather than the organism for information.
      Systematics::GenotypePtr gptr;
      gptr.DynamicCastFrom(org->SystematicsGroup("genotype"));
      data["genotypeName"] = gptr->Properties().Get("name").StringValue().Substring(4).GetData();
      data["genome"] = gptr->Properties().Get("genome").StringValue().GetData();
      data["age"] = org->GetPhenotype().GetAge();
      Systematics::CladePtr cptr;
      cptr.DynamicCastFrom(org->SystematicsGroup("clade"));
      data["ancestor"] = (cptr == nullptr) ? json(NaN) : json(cptr->Properties().Get("name").StringValue());
      //TODO: Clades have names?  How?
      
      bool has_gestated = (gptr->Properties().Get("total_gestation_count").IntValue() > 0);
      data["isEstimate"] = (has_gestated) ? false : true;
      data["fitness"] = (has_gestated) ? gptr->Properties().Get("ave_fitness").DoubleValue() : GetTestFitness(gptr);
      data["metabolism"] = (has_gestated) ? gptr->Properties().Get("ave_metabolic_rate").DoubleValue() : GetTestMerit(gptr);
      data["gestation"] = (has_gestated) ? gptr->Properties().Get("ave_gestation").DoubleValue() : GetTestGestationTime(gptr);
      
      //TODO: It doesn't look like genotypes actually track task counts right now
      //Instead, AvidaEd OSX goes through a test cpu to always grab the information
      map<string,double> task_count;
      cEnvironment& env = m_world->GetEnvironment();
      for (int t=0; t<env.GetNumTasks(); t++){
        cString task_name = env.GetTask(t).GetName();
        task_count[task_name.GetData()] = GetTestTaskCount(gptr, task_name);
      }
      data["tasks"] = task_count;
    }
    
    m_feedback.Data(data.dump().c_str());
    D_(D_ACTIONS, "cWebActionOrgDataByCellID::Process completed.");
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
        return NaN;
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
        return NaN;
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
      D_(D_ACTIONS, "cWebActionGridData::Process");
      WebViewerMsg data = { {"type","data"}, {"name","webGridData"} };
      cPopulation& population = m_world->GetPopulation();
      cEnvironment& env = m_world->GetEnvironment();
      vector<double> fitness;
      vector<double> gestation;
      vector<double> metabolism;
      vector<double> ancestor;
      map<string, vector<double>> tasks;
      vector<string> task_names;
      for (int t=0; t<env.GetNumTasks(); t++){
        task_names.push_back(string(env.GetTask(t).GetName().GetData()));
      }
      
      for (int i=0; i < population.GetSize(); i++)
      {
        cPopulationCell& cell = population.GetCell(i);
        cOrganism* org = cell.GetOrganism();
        
        
        if (org == nullptr){
          fitness.push_back(NaN);
          gestation.push_back(NaN);
          metabolism.push_back(NaN);
          ancestor.push_back(NaN);
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
      m_feedback.Data(data.dump().c_str());
      D_(D_ACTIONS, "cWebActionGridData::Process completed.");
    }
};


class importExpr : public cWebAction
{
  private:
    json m_files;
    constexpr string m_working_dir = "/working_dir";
    
  public:
    cWebActionGridData(cWorld* world, const cString& args, Avida::Feedback& fb) 
      : cWebAction(world,args,fb)
    {
        if (m_json_args){
          json jargs = GetJSONArgs();
          if (contains(jargs,"files")
            m_files = jargs["files"];
        }
    }
    
    static const cString GetDescription() { return "Arguments: [expr_name  \"{JSON-FORMATTED FILES}\"";}
    
    void Process(cAvidaContext& ctx)
    {
      //If the directory exists, recursively delete it
      Apto::FileSystem::RmDir(m_working_dir, true);
      
      //Create the directory
      Apto::FileSystem::MkDir(m_working_dir);
      
      if (m_files.empty() || !m_files.is_array())
        return;
        
      //Add the files
      for (auto it = m_files.begin(); it != m_files.end(); ++it){
        const json& j_file = *it;
        if (contains(j_file,"name")){
            string path = m_working_dir + j_file["name"];
            ofstream fot(path.c_str(), std::ofstream::out | std::ofstream::trunc);
            if (fot.is_open() && fot.good() && contains(j_file,"data") && j_file["data"].is_string()))
            {
              fot << j_file[data];
            }
            fot.close();
        }
      }
        
      WebDriver* driver = dynamic_cast<WebDriver*>(&ctx.GetDriver());
      if (driver == nullptr)
        return;
      driver->DoReset(m_working_dir);
    };
}


void RegisterWebDriverActions(cActionLibrary* action_lib)
{
    action_lib->Register<cWebActionPopulationStats>("webPopulationStats");
    action_lib->Register<cWebActionOrgTraceBySequence>("webOrgTraceBySequence");
    action_lib->Register<cWebActionOrgDataByCellID>("webOrgDataByCellID");
    action_lib->Register<cWebActionGridData>("webGridData");
    action_lib->Register<cWebActionImportExpr>("importExpr");
}

#endif
