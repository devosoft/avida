#ifndef ENVIRONMENT_HH
#include "main/environment.hh"
#endif
#ifndef TASKS_HH
#include "main/tasks.hh"
#endif
#ifndef TEST_CPU_HH
#include "cpu/test_cpu.hh"
#endif

#ifndef AVD_ANALYZER_DATA_ENTRY_HH
#include "avd_analyzer_data_entry.hh"
#endif
#ifndef AVD_ANALYZER_UTILS_HH
#include "avd_analyzer_utils.hh"
#endif

#ifndef AVD_ANALYZER_MODEL_HH
#include "avd_analyzer_model.hh"
#endif


bool avd_o_AnalyzerModel::LoadDetailDump(cString cur_string)
{
  AnGuiDebug << "entered.";
  // LOAD_DETAIL_DUMP

  cString filename = cur_string.PopWord();

  AnGuiInfo << "Loading: " << filename;

  cInitFile input_file(filename);
  if (!input_file.IsOpen()) {
    AnGuiError << "Cannot load file: \"" << filename << "\".";
    return false;
  }
  input_file.Load();
  input_file.Compress();
  input_file.Close();

  // Setup the genome...

  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);

    // Setup the genotype for this line...

    int id_num      = cur_line.PopWord().AsInt();
    int parent_id   = cur_line.PopWord().AsInt();
    int parent_dist = cur_line.PopWord().AsInt();
    int num_cpus    = cur_line.PopWord().AsInt();
    int total_cpus  = cur_line.PopWord().AsInt();
    int length      = cur_line.PopWord().AsInt();
    double merit    = cur_line.PopWord().AsDouble();
    int gest_time   = cur_line.PopWord().AsInt();
    double fitness  = cur_line.PopWord().AsDouble();
    int update_born = cur_line.PopWord().AsInt();
    int update_dead = cur_line.PopWord().AsInt();
    int depth       = cur_line.PopWord().AsInt();
    cString name = cStringUtil::Stringf("org-%d", id_num);

    cAnalyzeGenotype * genotype =
      new cAnalyzeGenotype(cur_line.PopWord(), inst_set);
    if(!genotype){
      AnGuiFatal << "Couldn't make new cAnalyzeGenotype.  Out of memory?";
    }

    genotype->SetID(id_num);
    genotype->SetParentID(parent_id);
    genotype->SetParentDist(parent_dist);
    genotype->SetNumCPUs(num_cpus);
    genotype->SetTotalCPUs(total_cpus);
    genotype->SetLength(length);
    genotype->SetMerit(merit);
    genotype->SetGestTime(gest_time);
    genotype->SetFitness(fitness);
    genotype->SetUpdateBorn(update_born);
    genotype->SetUpdateDead(update_dead);
    genotype->SetDepth(depth);
    genotype->SetName(name);

    // Add this genotype to the proper batch.
    batch[cur_batch].List().PushRear(genotype);
  }

  // Adjust the flags on this batch
  batch[cur_batch].SetLineage(false);
  batch[cur_batch].SetAligned(false);

  AnGuiDebug << "done.";
  return true;
}

bool avd_o_AnalyzerModel::LoadFile(cString cur_string)
{
  // LOAD
  AnGuiDebug << "entered.";

  cString filename = cur_string.PopWord();

  AnGuiInfo << "Loading: " << filename;

  cInitFile input_file(filename);
  if (!input_file.IsOpen()) {
    AnGuiError << "Cannot load file: \"" << filename << "\".";
    /* FIXME:  make an error window to give user feedback. -- kgn */
    /* no other cleanup appears needed. -- kgn */
    return false;
  }
  input_file.Load();
  input_file.ReadHeader();
  input_file.Compress();
  input_file.Close();

  const cString filetype = input_file.GetFiletype();
  if (filetype != "population_data") {
    AnGuiError << "Cannot load files of type \"" << filetype << "\".";
    /* FIXME:  make an error window to give user feedback. -- kgn */
    /* no other cleanup appears needed. -- kgn */
    return false;
  }

  if (verbose == true) {
    AnGuiInfo << "Loading file of type: " << filetype;
  }


  // Construct a linked list of data types that can be loaded...
  tList< tDataEntryCommand<cAnalyzeGenotype> > output_list;
  tListIterator< tDataEntryCommand<cAnalyzeGenotype> > output_it(output_list);
  LoadGenotypeDataList(input_file.GetFormat(), output_list);


  // Setup the genome...
  cGenome default_genome(1);
  int load_count = 0;

  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);

    cAnalyzeGenotype * genotype =
      new cAnalyzeGenotype(default_genome, inst_set);
    if(!genotype){
      AnGuiFatal << "Couldn't make new cAnalyzeGenotype.  Out of memory?";
    }

    output_it.Reset();
    tDataEntryCommand<cAnalyzeGenotype> * data_command = NULL;
    while ((data_command = output_it.Next()) != NULL) {
      data_command->SetTarget(genotype);
//        genotype->SetSpecialArgs(data_command->GetArgs());
      data_command->SetValue(cur_line.PopWord());
    }

    // Make sure this genotype has a name...
    if (genotype->GetName() == "") {
      cString name = cStringUtil::Stringf("org-D%d", load_count++);
      genotype->SetName(name);
    }

    // Add this genotype to the proper batch.
    batch[cur_batch].List().PushRear(genotype);
  }

  // Adjust the flags on this batch
  batch[cur_batch].SetLineage(false);
  batch[cur_batch].SetAligned(false);


  AnGuiDebug << "done.";
  return true;
}

void avd_o_AnalyzerModel::SetupGenotypeDataList()
{
  if (genotype_data_list.GetSize() != 0) return; // List already setup.

  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, bool>
       ("viable",      "Is Viable (0/1)", &cAnalyzeGenotype::GetViable,
        &cAnalyzeGenotype::SetViable));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("id",          "Genome ID",       &cAnalyzeGenotype::GetID,
        &cAnalyzeGenotype::SetID));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, const cString &>
       ("tag",         "Genotype Tag",    &cAnalyzeGenotype::GetTag,
        &cAnalyzeGenotype::SetTag,
        &cAnalyzeGenotype::CompareNULL, "(none)", ""));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("parent_id",   "Parent ID",       &cAnalyzeGenotype::GetParentID,
        &cAnalyzeGenotype::SetParentID));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("parent_dist", "Parent Distance", &cAnalyzeGenotype::GetParentDist,
        &cAnalyzeGenotype::SetParentDist));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("ancestor_dist","Ancestor Distance",&cAnalyzeGenotype::GetAncestorDist,
        &cAnalyzeGenotype::SetAncestorDist));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("num_cpus",    "Number of CPUs",  &cAnalyzeGenotype::GetNumCPUs,
        &cAnalyzeGenotype::SetNumCPUs));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("total_cpus",  "Total CPUs Ever", &cAnalyzeGenotype::GetTotalCPUs,
        &cAnalyzeGenotype::SetTotalCPUs));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("length",      "Genome Length",   &cAnalyzeGenotype::GetLength,
        &cAnalyzeGenotype::SetLength, &cAnalyzeGenotype::CompareLength));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("copy_length", "Copied Length",   &cAnalyzeGenotype::GetCopyLength,
        &cAnalyzeGenotype::SetCopyLength));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("exe_length",  "Executed Length", &cAnalyzeGenotype::GetExeLength,
        &cAnalyzeGenotype::SetExeLength));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("merit",       "Merit",           &cAnalyzeGenotype::GetMerit,
        &cAnalyzeGenotype::SetMerit, &cAnalyzeGenotype::CompareMerit));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("comp_merit",  "Computational Merit", &cAnalyzeGenotype::GetCompMerit,
        (void (cAnalyzeGenotype::*)(double)) NULL, &cAnalyzeGenotype::CompareCompMerit));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("comp_merit_ratio", "Computational Merit Ratio",
        &cAnalyzeGenotype::GetCompMeritRatio,
        (void (cAnalyzeGenotype::*)(double)) NULL,
        &cAnalyzeGenotype::CompareCompMerit));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("gest_time",   "Gestation Time",  &cAnalyzeGenotype::GetGestTime,
        &cAnalyzeGenotype::SetGestTime,
        &cAnalyzeGenotype::CompareGestTime, "Inf."));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("efficiency",  "Rep. Efficiency", &cAnalyzeGenotype::GetEfficiency,
        (void (cAnalyzeGenotype::*)(double)) NULL,
        &cAnalyzeGenotype::CompareEfficiency));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("efficiency_ratio", "Rep. Efficiency Ratio",
        &cAnalyzeGenotype::GetEfficiencyRatio,
        (void (cAnalyzeGenotype::*)(double)) NULL,
        &cAnalyzeGenotype::CompareEfficiency));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("fitness",     "Fitness",         &cAnalyzeGenotype::GetFitness,
        &cAnalyzeGenotype::SetFitness, &cAnalyzeGenotype::CompareFitness));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("div_type",     "Divide Type",         &cAnalyzeGenotype::GetDivType,
        &cAnalyzeGenotype::SetDivType));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("fitness_ratio", "Fitness Ratio", &cAnalyzeGenotype::GetFitnessRatio,
        (void (cAnalyzeGenotype::*)(double)) NULL,
        &cAnalyzeGenotype::CompareFitness));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("update_born", "Update Born",     &cAnalyzeGenotype::GetUpdateBorn,
        &cAnalyzeGenotype::SetUpdateBorn));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("update_dead", "Update Dead",     &cAnalyzeGenotype::GetUpdateDead,
        &cAnalyzeGenotype::SetUpdateDead));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("depth",       "Tree Depth",      &cAnalyzeGenotype::GetDepth,
        &cAnalyzeGenotype::SetDepth));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("frac_dead",   "Fraction Mutations Lethal",
        &cAnalyzeGenotype::GetFracDead,
        (void (cAnalyzeGenotype::*)(double)) NULL));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("frac_neg",   "Fraction Mutations Detrimental",
        &cAnalyzeGenotype::GetFracNeg,
        (void (cAnalyzeGenotype::*)(double)) NULL));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("frac_neut",   "Fraction Mutations Neutral",
        &cAnalyzeGenotype::GetFracNeut,
        (void (cAnalyzeGenotype::*)(double)) NULL));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, double>
       ("frac_pos",   "Fraction Mutations Beneficial",
        &cAnalyzeGenotype::GetFracPos,
        (void (cAnalyzeGenotype::*)(double)) NULL));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, const cString &>
       ("parent_muts", "Mutations from Parent",
        &cAnalyzeGenotype::GetParentMuts, &cAnalyzeGenotype::SetParentMuts,
        &cAnalyzeGenotype::CompareNULL, "(none)", ""));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, const cString &>
       ("task_order", "Task Performance Order",
        &cAnalyzeGenotype::GetTaskOrder, &cAnalyzeGenotype::SetTaskOrder,
        &cAnalyzeGenotype::CompareNULL, "(none)", ""));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, cString>
       ("sequence",    "Genome Sequence",
        &cAnalyzeGenotype::GetSequence, &cAnalyzeGenotype::SetSequence,
        &cAnalyzeGenotype::CompareNULL, "(N/A)", ""));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, const cString &>
       ("alignment",   "Aligned Sequence",
        &cAnalyzeGenotype::GetAlignedSequence,
        &cAnalyzeGenotype::SetAlignedSequence,
        &cAnalyzeGenotype::CompareNULL, "(N/A)", ""));

  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, cString>
       ("task_list",    "List of all tasks performed",
        &cAnalyzeGenotype::GetTaskList,
        (void (cAnalyzeGenotype::*)(cString)) NULL,
        &cAnalyzeGenotype::CompareNULL, "(N/A)", ""));

  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, cString>
       ("link.tasksites", "Phenotype Map", &cAnalyzeGenotype::GetMapLink,
        (void (cAnalyzeGenotype::*)(cString)) NULL));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, cString>
       ("html.sequence",  "Genome Sequence",
        &cAnalyzeGenotype::GetHTMLSequence,
        (void (cAnalyzeGenotype::*)(cString)) NULL,
        &cAnalyzeGenotype::CompareNULL, "(N/A)", ""));

  const cTaskLib & task_lib = cTestCPU::GetEnvironment()->GetTaskLib();
  for (int i = 0; i < task_lib.GetSize(); i++) {
    cString t_name, t_desc;
    t_name.Set("task.%d", i);
    t_desc = task_lib.GetTask(i).GetDesc();
    genotype_data_list.PushRear(new avd_o_AnalyzerArgDataEntry<cAnalyzeGenotype, int, int>
        (t_name, t_desc, &cAnalyzeGenotype::GetTaskCount, i,
         &cAnalyzeGenotype::CompareTaskCount));
  }

  // The remaining values should actually go in a seperate list called
  // "population_data_list", but for the moment we're going to put them
  // here so that we only need to worry about a single system to load and
  // save genotype information.
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("update",       "Update Output",
        &cAnalyzeGenotype::GetUpdateDead, &cAnalyzeGenotype::SetUpdateDead));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("dom_num_cpus", "Number of Dominant Organisms",
        &cAnalyzeGenotype::GetNumCPUs, &cAnalyzeGenotype::SetNumCPUs));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("dom_depth",    "Tree Depth of Dominant Genotype",
        &cAnalyzeGenotype::GetDepth, &cAnalyzeGenotype::SetDepth));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, int>
       ("dom_id",       "Dominant Genotype ID",
        &cAnalyzeGenotype::GetID, &cAnalyzeGenotype::SetID));
  genotype_data_list.PushRear(new avd_o_AnalyzerDataEntry<cAnalyzeGenotype, cString>
       ("dom_sequence", "Dominant Genotype Sequence",
        &cAnalyzeGenotype::GetSequence, &cAnalyzeGenotype::SetSequence,
        &cAnalyzeGenotype::CompareNULL, "(N/A)", ""));
}

bool avd_o_AnalyzerModel::loadCurrentGenotypes(cString filename){
  AnGuiDebug << "entered, but this method is currently empty.";
  return true;
}
bool avd_o_AnalyzerModel::loadHistoricGenotypes(cString filename){
  AnGuiDebug << "entered.";
  bool result = LoadDetailDump(filename);
  AnGuiDebug << "done.";
  return result;
}
bool avd_o_AnalyzerModel::open(cString filename){
  AnGuiDebug << "entered.";
  bool result = LoadFile(filename);
  AnGuiDebug << "done.";
  return result;
}
bool avd_o_AnalyzerModel::batchName(int idx, cString name) { 
  AnGuiDebug << "entered.";
  assert(0<=idx && idx<MAX_BATCHES);
  if (name.CountNumWords() == 0) {
    AnGuiError << "didn't rename batch because no new name was given.";
    return false;
  }
  batch[idx].Name() = name.PopWord();
  AnGuiDebug << "new name: \"" << batch[idx].Name() << "\".";
  AnGuiDebug << "done.";
  return true;
}


void avd_o_AnalyzerModel::setCurrentBatchIdx(int idx){
  AnGuiDebug << "entered.";
  assert(0<=idx && idx<MAX_BATCHES);
  cur_batch = idx;
  AnGuiDebug << "new index: " << idx << ".";
  AnGuiDebug << "done.";
}
int avd_o_AnalyzerModel::getCurrentBatchIdx(void){
  return cur_batch;
}

cGenotypeBatch *avd_o_AnalyzerModel::getBatchAt(int idx){
  AnGuiDebug << "entered.";
  assert(0<=idx && idx<MAX_BATCHES);
  AnGuiDebug << "done.";
  return batch + idx;
}

void avd_o_AnalyzerModel::LoadDataEntryCommands(
  cStringList arg_list,
  tList< avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> > &output_list
){
  AnGuiDebug << "entered.";

  // If no args were given, load all of the stats.
  if (arg_list.GetSize() == 0) {
    tListIterator< avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> >
      genotype_data_it(getGenotypeDataList());
    while (genotype_data_it.Next() != (void *) NULL) {
      avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> * entry_command =
        new avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype>(genotype_data_it.Get());
      output_list.PushRear(entry_command);
    }
  }
  // Otherwise, load only those listed.
  else {
    while (arg_list.GetSize() != 0) {
      // Setup the next entry
      cString cur_args = arg_list.Pop();
      cString cur_entry = cur_args.Pop(':');
      bool found_entry = false;

      AnGuiDebug << "new arg: " << cur_args << ", entry: " << cur_entry;

      // Scan the genotype data list for the current entry
      tListIterator< avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> >
        genotype_data_it(getGenotypeDataList());

      while (genotype_data_it.Next() != (void *) NULL) {
        if (genotype_data_it.Get()->GetName() == cur_entry) {
          avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype> * entry_command =
            new avd_o_AnalyzerDataEntryCommand<cAnalyzeGenotype>
            (genotype_data_it.Get(), cur_args);
          output_list.PushRear(entry_command);
          AnGuiDebug << "added new entry command.";
          found_entry = true;
          break;
        }
      }

      // If the entry was not found, give a warning.
      if (found_entry == false) {
        int best_match = 1000;
        cString best_entry;

        genotype_data_it.Reset();
        while (genotype_data_it.Next() != (void *) NULL) {
          const cString & test_str = genotype_data_it.Get()->GetName();
          const int test_dist = cStringUtil::EditDistance(test_str, cur_entry);
          if (test_dist < best_match) {
            best_match = test_dist;
            best_entry = test_str;
          }
        }

        AnGuiError << "Warning: Format entry \"" << cur_entry
             << "\" not found.  Best match is \""
             << best_entry << "\".";
      }

    }
  }

  AnGuiDebug << "output_list.GetSize(): " << output_list.GetSize();
  AnGuiDebug << "done.";
}

avd_o_AnalyzerModel::avd_o_AnalyzerModel():cAnalyze(){
  AnGuiDebug << "constructor.";
  //m_genotype_data_list.nameWatched(
  //  "<avd_o_AnalyzerModel(m_genotype_data_list)>");
}

tList< avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> > &
avd_o_AnalyzerModel::getGenotypeDataList(void){
  AnGuiDebug << "entered.";
  SetupGenotypeDataList();
  if(m_genotype_data_list.GetSize() == 0){
    tListIterator<tDataEntryBase<cAnalyzeGenotype> >
      it(genotype_data_list);
    it.Reset();
    while(it.Next() != 0)
      /*
      This is so not typesafe:
      */
      m_genotype_data_list.PushRear(
        static_cast<avd_o_AnalyzerDataEntryBase<cAnalyzeGenotype> *>(it.Get())
      );
      /*
      It only works because avd_o_AnalyzerModel::SetupGenotypeDataList(),
      defined above, has pushed avd_o_AnalyzerDataEntry objects into
      genotype_data_list, instead of tDataEntry objects.
      */
  }
  AnGuiDebug << "done.";
  return m_genotype_data_list;
}

// arch-tag: implementation file for old analyzer model
