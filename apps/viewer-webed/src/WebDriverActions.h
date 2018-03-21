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
#include <memory>


#include "avida/core/Feedback.h"
#include "avida/core/Types.h"
#include "avida/environment/Manager.h"
#include "cActionLibrary.h"

#include "avida/viewer/OrganismTrace.h"
#include "avida/private/systematics/Genotype.h"
#include "avida/private/systematics/Clade.h"
#include "avida/private/systematics/CladeArbiter.h"


#include "apto/core/FileSystem.h"

#include "cEnvironment.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cWorld.h"
#include "cStats.h"

#include "Driver.h"
#include "WebDebug.h"
#include "WebDriverActions.h"
#include "Types.h"


using namespace Avida::WebViewer;
using namespace std;

namespace Actions{
  
  
  const char* WA_POP_STATS = "webPopulationStats";
  const char* WA_ORG_TRACE = "webOrgTraceBySequence";
  const char* WA_CELL_DATA = "webOrgDataByCellID";
  const char* WA_GRID_DATA = "webGridData";
  const char* WA_SPAT_RES = "spatialResourceGrid";
  const char* WA_INJECT_SEQ = "webInjectSequence";
  const char* WA_EXPORT_EXPR = "exportExpr";
  const char* WA_IMPORT_EXPR = "importExpr";
  const char* WA_SET_UPDATE = "setUpdate";
  const char* WA_RESET = "reset";
  
  const int NO_SELECTION = -1;
  
  
  
  
  
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
    json GetJSONArgs()
    {
      if (m_json_args)
        return json::parse(&m_args.GetData()[1]);
      json j;
      return j;
    }
    
  public:
    cWebAction(cWorld* world, const cString& args, Avida::Feedback& fb) 
    : cAction(world,args) 
    , m_feedback(fb) 
    {
      m_json_args = (args.GetSize() > 0 && args[0] == UNIT_SEP);
    }
    
    void PackageData(const string& name, WebViewerMsg data, bool send_immediate = false)
    {
      WebViewerMsg retval;
      retval["data"] = data;
      retval["data"]["name"] = name;
      retval["sendImmediate"] = send_immediate;
      retval["data"]["update"] = m_world->GetStats().GetUpdate();
      retval["data"]["type"] = "data";
      m_feedback.Data(retval.dump().c_str());    //serilized json to send data to av_uis
    }
  };
  
  
  //----------------------------- cWebActionPrintSpatialResources ---------------------------------------------------
  // fb is feedback
  class cWebActionPrintSpatialResources : public cWebAction {
  public:
    cWebActionPrintSpatialResources(cWorld* world, const cString& args, Avida::Feedback& fb)
        : cWebAction(world, args, fb)
    {
    }
    static const cString GetDescription() { return "no arguments"; }

  // most of work
  void Process(cAvidaContext& ctx)
  {
    D_(D_ACTIONS, "cWebActionPrintSpatialResources::Process" );
    //map<string, vector<double> > res_data;
    m_world->GetPopulation().UpdateResStats(ctx);  //Synchronize copies of information about resources.
    const cStats& stats = m_world->GetStats();     //will hold copy of stats that we print out.

    // initalizing stuff used in the for loop through all the spatial resources
    vector<json> res_jlist;
    cPopulation& pop = m_world->GetPopulation();
    const cResourceCount& res_count = pop.GetResourceCount();
    int world_size = pop.GetSize();

    for (int res_id=0; res_id < stats.GetResources().GetSize(); res_id++){
      if (res_count.IsSpatialResource(res_id)){
        vector<double> res_data;
        string res_name = stats.GetResourceNames()[res_id].GetData();
        for (int cell_ndx=0; cell_ndx < world_size; cell_ndx++){
          res_data.push_back(stats.GetSpatialResourceCount()[res_id][cell_ndx]);
        }
        auto result = minmax_element(res_data.begin(), res_data.end());
        // add to json object
        json res_j;
        res_j["name"] = res_name;
        res_j["minVal"] = *result.first;   //first and second are pointers and to get the value just dereference
        res_j["maxVal"] = *result.second;
        res_j["data"] = res_data;
        res_jlist.push_back(res_j);
      }
    }  //end of for loop for each resource
    
        //WebViewerMsg means the same as json;
    WebViewerMsg getSpatialResources = {
      {"update", stats.GetUpdate()}
      ,{"resources", res_jlist}
    }; //end of webmessage

    PackageData(WA_SPAT_RES, getSpatialResources);
    D_(D_ACTIONS, "cWebActionPrintSpatialResources::Process [completed]");

  }  //end of Process
}; // cWebActionPopulationStats

  
  
  //----------------------------- cWebActionPopulationStats ------------------------------------------------------
  // fb is feedback
  class cWebActionPopulationStats : public cWebAction {
  public:
    cWebActionPopulationStats(cWorld* world, const cString& args, Avida::Feedback& fb) : cWebAction(world, args, fb)
    {
    }
    static const cString GetDescription() { return "no arguments"; }
    void Process(cAvidaContext& ctx){
      D_(D_ACTIONS, "cWebActionPopulationStats::Processs");
      const cStats& stats = m_world->GetStats();
      int update = stats.GetUpdate();
      
      const cPopulation& pop = m_world->GetPopulation();
      
      //calculating fitness, gestation, metablism averages for only viable organisms. Excluding non-viable organisms
      cDoubleSum fitness, gestation, metabolism;
      for (int kk=0; kk < pop.GetLiveOrgList().GetSize(); kk++){
        cOrganism* org = pop.GetLiveOrgList()[kk];
        const cPhenotype& phen = org->GetPhenotype();

        //@MRR Only works with pre-calculated organsims!   Needs to chagne when working with limited resources.
        if ( org->GetPhenotype().GetPrecalcIsViable() > 0) {
          fitness.Add( phen.GetFitness() );
          gestation.Add( phen.GetGestationTime() );
          metabolism.Add( phen.GetMerit().GetDouble() );
        }
      }
      
      int org_count = stats.GetNumCreatures();
      double ave_age = stats.GetAveCreatureAge();
      
      
      //WebViewerMsg = json;
      WebViewerMsg pop_data = {
        {"update", update}
        //,{"ave_fitness", ave_fitness}
        //,{"ave_gestation_time", ave_gestation_time}
        //,{"ave_metabolic_rate", ave_metabolic_rate}
        ,{"ave_fitness", fitness.Average()}
        ,{"ave_gestation_time", gestation.Average()}
        ,{"ave_metabolic_rate", metabolism.Average()}
        ,{"organisms", org_count}
        ,{"ave_age", ave_age}
      };
      
      cEnvironment& env = m_world->GetEnvironment();
      for (int t=0; t< env.GetNumTasks(); t++){
        pop_data[string(env.GetTask(t).GetName().GetData())] =
        stats.GetTaskLastCount(t);
      }
      PackageData(WA_POP_STATS, pop_data);
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
      WebViewerMsg data;
      
      for (int i = 0; i < trace.SnapshotCount(); ++i)
      {
        snapshots.push_back(ParseSnapshot(trace.Snapshot(i)));
      }
      data["snapshots"] = snapshots;
      
      PackageData(WA_ORG_TRACE, data, true);
      D_(D_ACTIONS, "cWebOrgTraceBySequence::Process completed");
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
        m_cell_id = NO_SELECTION;
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
      
      
      WebViewer::Driver* driver = dynamic_cast<WebViewer::Driver*>(&ctx.Driver());
      if (driver == nullptr)
        return;
      json data = driver->GetActiveCellData(m_cell_id);
      
      PackageData(WA_CELL_DATA, data, true);
      
      D_(D_ACTIONS, "cWebActionOrgDataByCellID::Process completed.");
    }
  }; // cWebActionPopulationStats
  
  
  
  
  
  class cWebActionGridData : public cWebAction {
  private:
    
    //Trying to keep data structures constructed
    //between Process events to see if there is
    //a speedup
    vector<string> task_names;
    
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
      cPopulation& population = m_world->GetPopulation();
      cEnvironment& env = m_world->GetEnvironment();
      for (int t=0; t<env.GetNumTasks(); t++){
        const string task_name = env.GetTask(t).GetName().GetData();
        task_names.push_back(task_name);
      }
    }
    
    static const cString GetDescription() { return "Arguments: none";}
    
    void Process(cAvidaContext& ctx)
    {
      D_(D_ACTIONS, "cWebActionGridData::Process");
      WebViewerMsg data;
      
      //Reset our vectors
      int world_size = m_world->GetPopulation().GetSize();
      vector<double> fitness(world_size, NaN);
      vector<double> gestation(world_size, NaN);
      vector<double> metabolism(world_size, NaN);
      vector<string> ancestor(world_size, "-");
    
      map<string, vector<double>> tasks;
      for (auto t : task_names){
        tasks[t] = vector<double>(world_size,NaN);
      }
      
      
      cPopulation& pop = m_world->GetPopulation();
      for (int i=0; i < world_size; i++)
      {
        if (!pop.GetCell(i).IsOccupied())
          continue;
        
        cPopulationCell& cell = pop.GetCell(i);
        cOrganism* org = cell.GetOrganism();
        
        
        Systematics::CladePtr cptr;
        cptr.DynamicCastFrom(org->SystematicsGroup("clade"));
        
        cPhenotype& phen = org->GetPhenotype();
        fitness[i]    = phen.GetFitness();
        gestation[i]  = phen.GetGestationTime();
        metabolism[i] = phen.GetMerit().GetDouble();
        ancestor[i]   = (cptr == NULL) ? "-" : cptr->Properties().Get("name").StringValue().GetData();
        for (size_t t = 0; t < task_names.size(); t++){
          tasks[task_names[t]][i] = phen.GetLastCountForTask(t);          
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
        {"data", ancestor}
      };
      
      for (auto it : tasks){
        data[it.first] = {
          {"data",it.second},
          {"minVal",min_val(it.second)},
          {"maxVal",max_val(it.second)}
        };
      }
      
      PackageData(WA_GRID_DATA, data);
      
      D_(D_ACTIONS, "cWebActionGridData::Process completed.");
    }
  };
  
  
  
  class cWebActionInjectSequence : public cWebAction
  {
  private:
    string m_genome;
    int m_start_id;
    int m_end_id;
    string m_clade_name;
    
  public:
    cWebActionInjectSequence(cWorld* world, const cString& args, Avida::Feedback& fb)
    : cWebAction(world,args,fb)
    {
      if (!m_json_args){
        fb.Error("cWebActionInjectSequence needs to have json arguments.");
      } else {
        json jargs = GetJSONArgs();
        if (contains(jargs,"genome")){
          m_genome = jargs["genome"].get<string>();
          if (contains(jargs,"start_cell_id")){
            m_start_id = jargs["start_cell_id"].get<int>();
            m_end_id = contains(jargs,"end_cell_id") ? jargs["end_cell_id"].get<int>() : -1;
            m_clade_name = (contains(jargs,"clade_name") ? jargs["clade_name"].get<string>() : "none");
          } else {
            fb.Error("cWebActionInjectSequence requires a 'start_cell_id' int");
          }
        } else {
          fb.Error("cWebActionInjectSequence requires a 'genome' string.");
        }
      }
    }
    
    static const cString GetDescription() { return "Arguments: JSON {genome:STRING, start_cell_id:INT, end_cell_id:INT [Default -1; single cell inject], m_clade_name:STRING"; }
    
    void Process(cAvidaContext& ctx)
    {
      D_(D_ACTIONS, "cWebActionInjectSequence::Process");
      WebViewer::Driver* driver = dynamic_cast<WebViewer::Driver*>(&ctx.Driver());
      if (driver != nullptr){
        if (m_end_id == -1) m_end_id = m_start_id + 1;
        for (int cell_id=m_start_id; cell_id < m_end_id; cell_id++){
          Systematics::RoleClassificationHints hints;
          hints["clade"]["name"] = m_clade_name.c_str();
          Genome genome(Apto::String(m_genome.c_str()));
          driver->GetWorld()->GetPopulation().InjectGenome(
                                                           cell_id, Systematics::Source(Systematics::DIVISION, "", true), genome, ctx, 0, true, &hints);
        }
      } else {
        m_feedback.Error("cWebActionInjectSequence::Process unable to get the driver.");
      }
      D_(D_ACTIONS, "cWebActionInjectSequence::Process [Done]");
    }
  };
  
  
  
  class cWebActionExportExpr : public cWebAction
  {
  private:
    const string m_export_dir = "/export";
    string m_pop_name;
    bool m_save_files;
    bool m_send_data;
    
    void SaveFiles(cAvidaContext& ctx)
    {
      //Make way for a fresh directory
      if (Apto::FileSystem::IsFile(m_export_dir.c_str())){
        Apto::FileSystem::RmFile(m_export_dir.c_str());
      } else if (Apto::FileSystem::IsDir(m_export_dir.c_str())){
        Apto::FileSystem::RmDir(m_export_dir.c_str(), true);
      }
      
      //Copy our files from the current working directory to the export directory
      Apto::FileSystem::MkDir(m_export_dir.c_str());
      vector<string> copy_files = {"avida.cfg", "environment.cfg", "events.cfg", "instset.cfg"};
      for (auto file : copy_files){
        string dst_file = m_export_dir + "/" + file;
        Apto::FileSystem::CpFile(file.c_str(), dst_file.c_str());
      }
      
      WebViewer::Driver* driver = dynamic_cast<WebViewer::Driver*>(&ctx.Driver());
      if (driver != nullptr){
        string m_pop_path = m_export_dir + "/detail.spop";
        if (!driver->GetWorld()->GetPopulation().SavePopulation(m_pop_path.c_str(), true)){
          m_feedback.Error("cWebActionExportExpr::Process is unable to save the population");
          return;
        };
        string m_clade_path = m_export_dir + "/clade.ssg";
        if (!driver->GetWorld()->GetPopulation().SaveStructuredSystematicsGroup("clade", m_clade_path.c_str())){
          m_feedback.Error("cWebActionExportExpr::Process is unable to save clade information");
        };
        string m_events_path = m_export_dir + "/events.cfg";
        std::ofstream fot(m_events_path.c_str());
        if (!fot.good()){
          m_feedback.Error("cWebActionExportExpr::Process is unable to write the events file");
        } else {
          fot << "u begin LoadPopulation detail.spop" << std::endl;
          fot << "u begin LoadStructuredSystematicsGroup  role=clade:filename=clade.ssg" << std::endl;
          fot.close();
        }
      } else {
        m_feedback.Error("cWebActionExportExpr::Process cannot locate driver.");
      }
    }
    
    json JSONifyDir()
    {
      D_(D_ACTIONS, "JSONIFYING directory",1);
      Apto::Array<Apto::String, Apto::Smart> entries;
      if (!Apto::FileSystem::ReadDir(m_export_dir.c_str(), entries)){
        m_feedback.Error("cWebActionExportExpr::JSONifyDir is unable to get export directory contents");
        return json(nullptr);
      }
      vector<json> files;
      for (int i = 0; i < entries.GetSize(); i++){
        Apto::String it = entries[i];
        string filename = string(it.GetData());
        string filepath = m_export_dir + "/" + string(it.GetData());
        if (filename[0] == '.')
          continue;
        
        D_(D_ACTIONS, "About to read file " << filename,1);
        ifstream fin(filepath.c_str());
        if (!fin.good()){
          m_feedback.Error(string("cWebActionExportExpr::JSONifyDir is unable to get export file " + filepath).c_str());
          D_(D_ACTIONS, "Unable to read filename " << filename);
          return json(nullptr);
        }
        ostringstream oss;
        oss << fin.rdbuf();
        fin.close();
        json jfile = { {"name",filename}, {"data",oss.str()} };
        files.push_back(jfile);
        D_(D_ACTIONS, filename << " has size of " << oss.str().size(),1);
      }
      D_(D_ACTIONS, "[Done] JSONIFYING directory.  Files read: " << files.size(),1);
      return json(files);
    }
    
  public:
    cWebActionExportExpr(cWorld* world, const cString& args, Avida::Feedback& fb)
    : cWebAction(world,args,fb)
    {
      m_pop_name = "untitled";
      m_save_files = false;
      m_send_data = true;
      if (m_json_args){
        json jargs = GetJSONArgs();
        if (contains(jargs, "popName")){
          m_pop_name = jargs["popName"].get<string>();
        }
        m_save_files = 
        contains(jargs, "saveFiles") ? jargs["saveFiles"].get<bool>() : false;
        m_send_data = 
        contains(jargs, "sendData") ? jargs["sendData"].get<bool>() : true;
      }
    }
    
    static const cString GetDescription() { return "Arguments: NONE"; }
    
    void Process(cAvidaContext& ctx)
    {
      D_(D_ACTIONS, "cWebActionExportExpr::Process saveFiles=" << m_save_files << "; sendData" << m_send_data);
      
      
      if (m_save_files){
        SaveFiles(ctx);
      }
      
      if (m_send_data){
        if (!Apto::FileSystem::IsDir(m_export_dir.c_str())){
          SaveFiles(ctx);
        }
        json data = {
          {"popName", m_pop_name},
          {"files", JSONifyDir()}
        };
        PackageData(WA_EXPORT_EXPR, data, true);
      }
      D_(D_ACTIONS, "cWebActionExportExpr::Process [Done]");
    }
  };
  
  
  
  
  
  
  class cWebActionImportExpr : public cWebAction
  {
  private:
    json m_files;
    string m_working_dir;
    bool m_do_amend;
    
  public:
    cWebActionImportExpr(cWorld* world, const cString& args, Avida::Feedback& fb)
    : cWebAction(world,args,fb)
    , m_working_dir("/working_dir")
    {
      D_(D_ACTIONS, "In cWebActionImportExpr::cWebActionImportExpr");
      if (m_json_args){
        json jargs = GetJSONArgs();
        if (contains(jargs,"files"))
          m_files = jargs["files"];
        m_do_amend = false;
        if (contains(jargs,"amend")){
          string do_amend = jargs["amend"].get<string>();
          if (do_amend == "true"){
            m_do_amend = true;
          }
        }
      }
      D_(D_ACTIONS, "Done cWebActionImportExpr::cWebActionImportExpr");
    }
    
    static const cString GetDescription() { return "Arguments: [expr_name  \"{JSON-FORMATTED FILES}\"";}
    
    void Process(cAvidaContext& ctx)
    {
      D_(D_ACTIONS, "In cWebImportExpr::Process");
      
      if (m_files.empty() || !m_files.is_array())
        return;
      
      const char* const wdir = m_working_dir.c_str();
      
      
      //If we're not amending what's already in the working directory,
      //the delete it and copy the default settings over
      if (!m_do_amend){
        D_(D_ACTIONS, "cWebImport::Expr::Process NOT amending");
        if (Apto::FileSystem::IsFile(wdir)){
          Apto::FileSystem::RmFile(wdir);
        } else if (Apto::FileSystem::IsDir(wdir)){
          Apto::FileSystem::RmDir(wdir,true);
        }
        
        //Create the directory
        Apto::FileSystem::MkDir(m_working_dir.c_str());
        
        //Copy default files
        vector<string> copy_files = {"/avida.cfg", "/default-heads.org", "/environment.cfg", "/events.cfg", "/instset.cfg"};
        for (auto f : copy_files){
          D_(D_ACTIONS, "cWebImport::Expr::Process copying default file to working dir: " << f,1);
          string dst = m_working_dir + f;    
          Apto::FileSystem::CpFile(f.c_str(), dst.c_str());
        } 
      }
      
      //Add the files
      for (auto it = m_files.begin(); it != m_files.end(); ++it){
        const json& j_file = *it;
        if (contains(j_file,"name") && j_file["name"].is_string()){
          string fn = j_file["name"].get<string>();
          string path =  m_working_dir + "/" + fn;
          ofstream fot(path.c_str(), std::ofstream::out | std::ofstream::trunc);
          if (fot.is_open() && fot.good() && contains(j_file,"data") && j_file["data"].is_string())
          {
            D_(D_ACTIONS, "Writing file from message: " + path,1);
            D_(D_ACTIONS, j_file["data"].get<string>() << endl << "^^^^^^^^^^^^^^^^^^^",2);
            fot << j_file["data"].get<string>();
          }
          fot.close();
        }
      }
      
      WebViewer::Driver* driver = dynamic_cast<WebViewer::Driver*>(&ctx.Driver());
      if (driver == nullptr)
        return;
      driver->DoReset(m_working_dir);
      D_(D_ACTIONS, "Done cWebImportExpr::Process");
      
    };
  };
  
  
  
  
  
  
  
  
  
  
  
  class cWebActionSetUpdate : public cWebAction
  {
  private:
    cString m_filename;
    
  public:
    cWebActionSetUpdate(cWorld* world, const cString& args, Avida::Feedback& fb)
    : cWebAction(world,args,fb)
    {
      cString largs(args);
      m_filename = (largs.GetSize()) ? largs.PopWord() : "update";
    }
    
    static const cString GetDescription() { return "Arguments: [expr_name  \"{JSON-FORMATTED FILES}\"";}
    
    void Process(cAvidaContext& ctx)
    {
      D_(D_ACTIONS, "In cWebActionSetUpdate::Process");
      if (Apto::FileSystem::IsFile(m_filename.GetData())){
        D_(D_ACTIONS, m_filename + " file exists");
        ifstream fin(m_filename.GetData());
        int update;
        fin >> update;
        if (!fin.bad()){
          D_(D_ACTIONS, "Sucessfully read in update as " << update);
          fin.close();
          WebViewer::Driver* driver = dynamic_cast<WebViewer::Driver*>(&ctx.Driver());
          if (driver != nullptr){
            D_(D_ACTIONS, "Notifying driver to change update to " << update);
            driver->GetWorld()->GetStats().SetCurrentUpdate(update);
            return;
          }
        }
        fin.close();
      }
      D_(D_ACTIONS, "There was an error reading and setting the update from a file");
      m_feedback.Error("cWebActionSetUpdate: unable to read update file.");
    }
  };
  
  
  
  
  class cWebActionReset : public cWebAction
  {
  private:
    
  public:
    cWebActionReset(cWorld* world, const cString& args, Avida::Feedback& fb)
    : cWebAction(world,args,fb)
    {
    }
    
    static const cString GetDescription() { return "Arguments: NONE"; }
    
    void Process(cAvidaContext& ctx)
    {
      D_(D_ACTIONS, "cWebActionReset::Process");
      
      //Clean up our working and export directory
      const char* wdir = "/working_dir";
      if (Apto::FileSystem::IsFile(wdir)){
        Apto::FileSystem::RmFile(wdir);
      } else if (Apto::FileSystem::IsDir(wdir)){
        Apto::FileSystem::RmDir(wdir, true);
      }
      const char* edir = "/export";
      if (Apto::FileSystem::IsFile(edir)){
        Apto::FileSystem::RmFile(edir);
      } else if (Apto::FileSystem::IsDir(edir)){
        Apto::FileSystem::RmDir(edir, true);
      }
      
      WebViewer::Driver* driver = dynamic_cast<WebViewer::Driver*>(&ctx.Driver());
      
      if (driver != nullptr){
        driver->DoReset("/");
      } else {
        m_feedback.Error("cWebActionReset::Process unable to reset driver.");
      }
      D_(D_ACTIONS, "cWebActionReset::Process [Done]");
    }
  };
  
  
  void RegisterWebDriverActions(cActionLibrary* action_lib)
  {
    action_lib->Register<cWebActionPrintSpatialResources>(WA_SPAT_RES);
    action_lib->Register<cWebActionPopulationStats>(WA_POP_STATS);
    action_lib->Register<cWebActionOrgTraceBySequence>(WA_ORG_TRACE);
    action_lib->Register<cWebActionOrgDataByCellID>(WA_CELL_DATA);
    action_lib->Register<cWebActionGridData>(WA_GRID_DATA);
    action_lib->Register<cWebActionInjectSequence>(WA_INJECT_SEQ);
    action_lib->Register<cWebActionExportExpr>(WA_EXPORT_EXPR);
    action_lib->Register<cWebActionImportExpr>(WA_IMPORT_EXPR);
    action_lib->Register<cWebActionSetUpdate>(WA_SET_UPDATE);
    action_lib->Register<cWebActionReset>(WA_RESET);
    
  }
  
};
#endif
