#ifndef ENVIRONMENT_HH
#include "main/environment.hh"
#endif
#ifndef TASKS_HH
#include "main/tasks.hh"
#endif
#ifndef TEST_CPU_HH
#include "cpu/test_cpu.hh"
#endif

#ifndef AVD_N_ANALYZER_DBG_MSGS_HH
#include "avd_n_analyzer_dbg_msgs.hh"
#endif

#ifndef AVD_N_ANALYZER_MDL_HH
#include "avd_n_analyzer_mdl.hh"
#endif


bool avdAnalyzerMdl::LoadDetailDump(cString cur_string)
{
  Debug << "entered.";
  // LOAD_DETAIL_DUMP

  cString filename = cur_string.PopWord();

  Info << "Loading: " << filename;

  cInitFile input_file(filename);
  if (!input_file.IsOpen()) {
    Error << "Cannot load file: \"" << filename << "\".";
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

    cAnalyzeGenotype *genotype =
      new cAnalyzeGenotype(cur_line.PopWord(), inst_set);
    if(!genotype){
      Fatal << "Couldn't make new cAnalyzeGenotype.  Out of memory?";
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

  Debug << "done.";
  return true;
} 
  

bool avdAnalyzerMdl::LoadFile(cString cur_string)
{ 
  // LOAD
  Debug << "entered.";

  cString filename = cur_string.PopWord();

  Info << "Loading: " << filename;

  cInitFile input_file(filename);
  if (!input_file.IsOpen()) {
    Error << "Cannot load file: \"" << filename << "\".";
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
    Error << "Cannot load files of type \"" << filetype << "\".";
    /* FIXME:  make an error window to give user feedback. -- kgn */
    /* no other cleanup appears needed. -- kgn */
    return false;
  }

  if (verbose == true) {
    Info << "Loading file of type: " << filetype;
  }


  // Construct a linked list of data types that can be loaded...
  tRetainableList< avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > output_list;
  tRetainableListIter< avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > output_it(output_list);
  LoadGenotypeDataList(input_file.GetFormat(), output_list);


  // Setup the genome...
  cGenome default_genome(1);
  int load_count = 0;

  for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
    cString cur_line = input_file.GetLine(line_id);

    cAnalyzeGenotype * genotype =
      new cAnalyzeGenotype(default_genome, inst_set);
    if(!genotype){
      Fatal << "Couldn't make new cAnalyzeGenotype.  Out of memory?";
    }

    output_it.Reset();
    avdAnalyzerDataEntryCommand<cAnalyzeGenotype> * data_command = NULL;
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


  Debug << "done.";
  return true;
}

void avdAnalyzerMdl::FindGenotype(cString cur_string)
{
  // If no arguments are passed in, just find max num_cpus.
  if (cur_string.GetSize() == 0) cur_string = "num_cpus";

  if (verbose == true) {
    Debug << "Reducing batch " << cur_batch << " to genotypes: ";
  }

  tList<cAnalyzeGenotype> & gen_list = batch[cur_batch].List();
  tList<cAnalyzeGenotype> found_list;
  while (cur_string.CountNumWords() > 0) {
    cString gen_desc(cur_string.PopWord());
    if (verbose) Debug << gen_desc << " ";

    // Determine by lin_type which genotype were are tracking...
    cAnalyzeGenotype * found_gen = PopGenotype(gen_desc, cur_batch);

    if (found_gen == NULL) {
      Error << "  Warning: genotype not found!";
      continue;
    }

    // Save this genotype...
    found_list.Push(found_gen);
  } 

  // Delete all genotypes other than the ones found!
  while (gen_list.GetSize() > 0) delete gen_list.Pop();

  // And fill it back in with the good stuff.
  while (found_list.GetSize() > 0) gen_list.Push(found_list.Pop());

  // Adjust the flags on this batch
  batch[cur_batch].SetLineage(false);
  batch[cur_batch].SetAligned(false);
}

void avdAnalyzerMdl::FindLineage(cString cur_string)
{
  cString lin_type = "num_cpus";
  if (cur_string.CountNumWords() > 0) lin_type = cur_string.PopWord();

  if (verbose == true) {
    Debug << "Reducing batch " << cur_batch
   << " to " << lin_type << " lineage ";
  } else Debug << "Performing lineage scan...";


  // Determine by lin_type which genotype we are tracking...
  cAnalyzeGenotype * found_gen = PopGenotype(lin_type, cur_batch);

  if (found_gen == NULL) {
    Debug << "  Warning: Genotype " << lin_type
   << " not found.  Lineage scan aborted.";
    return;
  } 

  // Otherwise, trace back through the id numbers to mark all of those
  // in the ancestral lineage...

  // Construct a list of genotypes found...

  tList<cAnalyzeGenotype> found_list;
  found_list.Push(found_gen);
  int next_id = found_gen->GetParentID();
  bool found = true;
  while (found == true) {
    found = false;

    tListIterator<cAnalyzeGenotype> batch_it(batch[cur_batch].List());
    while ((found_gen = batch_it.Next()) != NULL) {
      if (found_gen->GetID() == next_id) {
  batch_it.Remove();
  found_list.Push(found_gen);
  next_id = found_gen->GetParentID();
  found = true;
  break;
      }
    }
  } 

  // We now have all of the genotypes in this lineage, delete everything
  // else.

  const int total_removed = batch[cur_batch].List().GetSize();
  while (batch[cur_batch].List().GetSize() > 0) {
    delete batch[cur_batch].List().Pop();
  } 

  // And fill it back in with the good stuff.
  int total_kept = found_list.GetSize();
  while (found_list.GetSize() > 0) {
    batch[cur_batch].List().PushRear(found_list.Pop());
  } 

  if (verbose == true) {
    Debug << "  Lineage has " << total_kept << " genotypes; "
   << total_removed << " were removed.";
  } 

  // Adjust the flags on this batch
  batch[cur_batch].SetLineage(true);
  batch[cur_batch].SetAligned(false);
}

void avdAnalyzerMdl::FindClade(cString cur_string)
{
  if (cur_string.GetSize() == 0) {
    Error << "  Warning: No clade specified for FIND_CLADE.  Aborting.";
    return;
  }

  cString clade_type( cur_string.PopWord() );

  if (verbose == true) {
    Debug << "Reducing batch " << cur_batch
   << " to clade " << clade_type << ".";
  } else Debug << "Performing clade scan...";


  // Determine by clade_type which genotype we are tracking...
  cAnalyzeGenotype * found_gen = PopGenotype(clade_type, cur_batch);

  if (found_gen == NULL) {
    Debug << "  Warning: Ancestral genotype " << clade_type
   << " not found.  Clade scan aborted.";
    return;
  }

  // Do this the brute force way... scan for one step at a time.

  // Construct a list of genotypes found...

  tList<cAnalyzeGenotype> found_list; // Found and finished.
  tList<cAnalyzeGenotype> scan_list;  // Found, but need to scan for children.
  scan_list.Push(found_gen);

  // Keep going as long as there is something in the scan list...
  while (scan_list.GetSize() > 0) {
    // Move the next genotype from the scan list to the found_list.
    found_gen = scan_list.Pop();
    int parent_id = found_gen->GetID();
    found_list.Push(found_gen);

    // Seach for all of the children of this genotype...
    tListIterator<cAnalyzeGenotype> batch_it(batch[cur_batch].List());
    while ((found_gen = batch_it.Next()) != NULL) {
      // If we found a child, place it into the scan list.
      if (found_gen->GetParentID() == parent_id) {
  batch_it.Remove();
  scan_list.Push(found_gen);
      }
    }
  }

  // We now have all of the genotypes in this clade, delete everything else.

  const int total_removed = batch[cur_batch].List().GetSize();
  while (batch[cur_batch].List().GetSize() > 0) {
    delete batch[cur_batch].List().Pop();
  }

  // And fill it back in with the good stuff.
  int total_kept = found_list.GetSize();
  while (found_list.GetSize() > 0) {
    batch[cur_batch].List().PushRear(found_list.Pop());
  }

  if (verbose == true) {
    Debug << "  Clade has " << total_kept << " genotypes; "
   << total_removed << " were removed.";
  }

  // Adjust the flags on this batch
  batch[cur_batch].SetLineage(false);
  batch[cur_batch].SetAligned(false);
}


// Pass in the arguments for a command and fill out the entries in list
// format....

void avdAnalyzerMdl::LoadGenotypeDataList(
  cStringList arg_list,
  tRetainableList< avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > & output_list)
{ 
  // For the moment, just put everything into the output list.
  SetupGenotypeDataList();

  // If no args were given, load all of the stats.
  if (arg_list.GetSize() == 0) {
    tRetainableListIter< avdAnalyzerDataEntryBase<cAnalyzeGenotype> >
      genotype_data_it(m_genotype_data_list);
    while (genotype_data_it.Next() != (void *) NULL) {
      avdAnalyzerDataEntryCommand<cAnalyzeGenotype> * entry_command =
        new avdAnalyzerDataEntryCommand<cAnalyzeGenotype>(genotype_data_it.Get());
      output_list.PushRear(entry_command);
      entry_command->nameWatched("<avdAnalyzerMdl::LoadGenotypeDataList(entry_command)>");
      entry_command->release();
    }
  }
  // Otherwise, load only those listed.
  else {
    while (arg_list.GetSize() != 0) {
      // Setup the next entry
      cString cur_args = arg_list.Pop();
      cString cur_entry = cur_args.Pop(':');
      bool found_entry = false;

      // Scan the genotype data list for the current entry
      tRetainableListIter< avdAnalyzerDataEntryBase<cAnalyzeGenotype> >
        genotype_data_it(m_genotype_data_list);

      while (genotype_data_it.Next() != (void *) NULL) {
        if (genotype_data_it.Get()->GetName() == cur_entry) {
          avdAnalyzerDataEntryCommand<cAnalyzeGenotype> * entry_command =
            new avdAnalyzerDataEntryCommand<cAnalyzeGenotype>(genotype_data_it.Get(), cur_args);
          output_list.PushRear(entry_command);
          entry_command->nameWatched("(avdAnalyzerDataEntryCommand)");
          entry_command->release();
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

        Error << "Warning: Format entry \"" << cur_entry
             << "\" not found.  Best match is \"" << best_entry << "\".";
      }
    }
  }
}

void avdAnalyzerMdl::SetupGenotypeDataList()
{
  if (m_genotype_data_list.GetSize() != 0) return; // List already setup.
  avdAnalyzerDataEntryBase<cAnalyzeGenotype> *de;
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, bool>
    ("viable",      "Is Viable (0/1)", &cAnalyzeGenotype::GetViable,
      &cAnalyzeGenotype::SetViable);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) viable");
  de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("id",          "Genome ID",       &cAnalyzeGenotype::GetID,
      &cAnalyzeGenotype::SetID);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) id"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, const cString &>
    ("tag",         "Genotype Tag",    &cAnalyzeGenotype::GetTag,
      &cAnalyzeGenotype::SetTag,
      &cAnalyzeGenotype::CompareNULL, "(none)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) tag"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("parent_id",   "Parent ID",       &cAnalyzeGenotype::GetParentID,
      &cAnalyzeGenotype::SetParentID);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) parent_id"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("parent_dist", "Parent Distance", &cAnalyzeGenotype::GetParentDist,
      &cAnalyzeGenotype::SetParentDist);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) parent_dist"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("ancestor_dist","Ancestor Distance",&cAnalyzeGenotype::GetAncestorDist,
      &cAnalyzeGenotype::SetAncestorDist);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) ancestor_dist"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("num_cpus",    "Number of CPUs",  &cAnalyzeGenotype::GetNumCPUs,
      &cAnalyzeGenotype::SetNumCPUs);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) num_cpus"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("total_cpus",  "Total CPUs Ever", &cAnalyzeGenotype::GetTotalCPUs,
      &cAnalyzeGenotype::SetTotalCPUs);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) total_cpus"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("length",      "Genome Length",   &cAnalyzeGenotype::GetLength,
      &cAnalyzeGenotype::SetLength, &cAnalyzeGenotype::CompareLength);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) length"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("copy_length", "Copied Length",   &cAnalyzeGenotype::GetCopyLength,
      &cAnalyzeGenotype::SetCopyLength);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) copy_length"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("exe_length",  "Executed Length", &cAnalyzeGenotype::GetExeLength,
      &cAnalyzeGenotype::SetExeLength);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) exe_length"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("merit",       "Merit",           &cAnalyzeGenotype::GetMerit,
      &cAnalyzeGenotype::SetMerit, &cAnalyzeGenotype::CompareMerit);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) merit"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("comp_merit",  "Computational Merit", &cAnalyzeGenotype::GetCompMerit,
      (void (cAnalyzeGenotype::*)(double)) NULL, &cAnalyzeGenotype::CompareCompMerit);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) comp_merit"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("comp_merit_ratio", "Computational Merit Ratio",
      &cAnalyzeGenotype::GetCompMeritRatio,
      (void (cAnalyzeGenotype::*)(double)) NULL,
      &cAnalyzeGenotype::CompareCompMerit);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) comp_merit_ratio"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("gest_time",   "Gestation Time",  &cAnalyzeGenotype::GetGestTime,
      &cAnalyzeGenotype::SetGestTime,
      &cAnalyzeGenotype::CompareGestTime, "Inf.");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) gest_time"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("efficiency",  "Rep. Efficiency", &cAnalyzeGenotype::GetEfficiency,
      (void (cAnalyzeGenotype::*)(double)) NULL,
      &cAnalyzeGenotype::CompareEfficiency);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) efficiency"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("efficiency_ratio", "Rep. Efficiency Ratio",
      &cAnalyzeGenotype::GetEfficiencyRatio,
      (void (cAnalyzeGenotype::*)(double)) NULL,
      &cAnalyzeGenotype::CompareEfficiency);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) efficiency_ratio"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("fitness",     "Fitness",         &cAnalyzeGenotype::GetFitness,
      &cAnalyzeGenotype::SetFitness, &cAnalyzeGenotype::CompareFitness);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) fitness"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("div_type",     "Divide Type",         &cAnalyzeGenotype::GetDivType,
      &cAnalyzeGenotype::SetDivType);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) div_type"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("fitness_ratio", "Fitness Ratio", &cAnalyzeGenotype::GetFitnessRatio,
      (void (cAnalyzeGenotype::*)(double)) NULL,
      &cAnalyzeGenotype::CompareFitness);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) fitness_ratio"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("update_born", "Update Born",     &cAnalyzeGenotype::GetUpdateBorn,
      &cAnalyzeGenotype::SetUpdateBorn);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) update_born"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("update_dead", "Update Dead",     &cAnalyzeGenotype::GetUpdateDead,
      &cAnalyzeGenotype::SetUpdateDead);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) update_dead"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("depth",       "Tree Depth",      &cAnalyzeGenotype::GetDepth,
      &cAnalyzeGenotype::SetDepth);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) depth"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("frac_dead",   "Fraction Mutations Lethal",
      &cAnalyzeGenotype::GetFracDead,
      (void (cAnalyzeGenotype::*)(double)) NULL);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) frac_dead"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("frac_neg",   "Fraction Mutations Detrimental",
      &cAnalyzeGenotype::GetFracNeg,
      (void (cAnalyzeGenotype::*)(double)) NULL);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) frac_neg"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("frac_neut",   "Fraction Mutations Neutral",
      &cAnalyzeGenotype::GetFracNeut,
      (void (cAnalyzeGenotype::*)(double)) NULL);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) frac_neut"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, double>
    ("frac_pos",   "Fraction Mutations Beneficial",
      &cAnalyzeGenotype::GetFracPos,
      (void (cAnalyzeGenotype::*)(double)) NULL);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) frac_pos"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, const cString &>
    ("parent_muts", "Mutations from Parent",
      &cAnalyzeGenotype::GetParentMuts, &cAnalyzeGenotype::SetParentMuts,
      &cAnalyzeGenotype::CompareNULL, "(none)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) parent_muts"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, const cString &>
    ("task_order", "Task Performance Order",
      &cAnalyzeGenotype::GetTaskOrder, &cAnalyzeGenotype::SetTaskOrder,
      &cAnalyzeGenotype::CompareNULL, "(none)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) task_order"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, cString>
    ("sequence",    "Genome Sequence",
      &cAnalyzeGenotype::GetSequence, &cAnalyzeGenotype::SetSequence,
      &cAnalyzeGenotype::CompareNULL, "(N/A)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) sequence"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, const cString &>
    ("alignment",   "Aligned Sequence",
      &cAnalyzeGenotype::GetAlignedSequence,
      &cAnalyzeGenotype::SetAlignedSequence,
      &cAnalyzeGenotype::CompareNULL, "(N/A)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) alignment"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, cString>
    ("task_list",    "List of all tasks performed",
      &cAnalyzeGenotype::GetTaskList,
      (void (cAnalyzeGenotype::*)(cString)) NULL,
      &cAnalyzeGenotype::CompareNULL, "(N/A)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) task_list"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, cString>
    ("link.tasksites", "Phenotype Map", &cAnalyzeGenotype::GetMapLink,
      (void (cAnalyzeGenotype::*)(cString)) NULL);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) link.tasksites"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, cString>
    ("html.sequence",  "Genome Sequence",
      &cAnalyzeGenotype::GetHTMLSequence,
      (void (cAnalyzeGenotype::*)(cString)) NULL,
      &cAnalyzeGenotype::CompareNULL, "(N/A)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) html.sequence"); de->release();
  const cTaskLib & task_lib = cTestCPU::GetEnvironment()->GetTaskLib();
  for (int i = 0; i < task_lib.GetSize(); i++) {
    cString t_name, t_desc;
    t_name.Set("task.%d", i);
    t_desc = task_lib.GetTask(i).GetDesc();
    de = new avdAnalyzerArgDataEntry<cAnalyzeGenotype, int, int>
      (t_name, t_desc, &cAnalyzeGenotype::GetTaskCount, i,
        &cAnalyzeGenotype::CompareTaskCount);
    m_genotype_data_list.PushRear(de);
    de->nameWatched("(avdAnalyzerDataEntry) a task"); de->release();
  }
  // The remaining values should actually go in a seperate list called
  // "population_data_list", but for the moment we're going to put them
  // here so that we only need to worry about a single system to load and
  // save genotype information.
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("update",       "Update Output",
      &cAnalyzeGenotype::GetUpdateDead, &cAnalyzeGenotype::SetUpdateDead);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) update"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("dom_num_cpus", "Number of Dominant Organisms",
      &cAnalyzeGenotype::GetNumCPUs, &cAnalyzeGenotype::SetNumCPUs);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) dom_num_cpus"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("dom_depth",    "Tree Depth of Dominant Genotype",
      &cAnalyzeGenotype::GetDepth, &cAnalyzeGenotype::SetDepth);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) dom_depth"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, int>
    ("dom_id",       "Dominant Genotype ID",
      &cAnalyzeGenotype::GetID, &cAnalyzeGenotype::SetID);
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) dom_id"); de->release();
  de = new avdAnalyzerDataEntry<cAnalyzeGenotype, cString>
    ("dom_sequence", "Dominant Genotype Sequence",
      &cAnalyzeGenotype::GetSequence, &cAnalyzeGenotype::SetSequence,
      &cAnalyzeGenotype::CompareNULL, "(N/A)", "");
  m_genotype_data_list.PushRear(de);
  de->nameWatched("(avdAnalyzerDataEntry) dom_sequence"); de->release();
}

bool avdAnalyzerMdl::loadCurrentGenotypes(cString){
  Debug << "entered, but this method is currently empty.";
  return true;
}
bool avdAnalyzerMdl::loadHistoricGenotypes(cString filename){
  Debug << "entered.";
  bool result = LoadDetailDump(filename);
  Debug << "done.";
  return result;
}
bool avdAnalyzerMdl::open(cString filename){
  Debug << "entered.";
  bool result = LoadFile(filename);
  Debug << "done.";
  return result;
}
void avdAnalyzerMdl::findGenotype(QString cur_string){
  Debug << "entered.";
  FindGenotype(cur_string.ascii());
  Debug << "done.";
}
void avdAnalyzerMdl::findLineage(QString cur_string){
  Debug << "entered.";
  FindLineage(cur_string.ascii());
  Debug << "done.";
}
void avdAnalyzerMdl::findClade(QString cur_string){
  Debug << "entered.";
  FindClade(cur_string.ascii());
  Debug << "done.";
}
void avdAnalyzerMdl::batchPurge(int idx) { 
  Debug << "entered.  deleting batch[" << idx << "].";
  while (batch[idx].List().GetSize() > 0) {
    delete batch[idx].List().Pop();
  }

  batch[idx].SetLineage(false);
  batch[idx].SetAligned(false);
  Debug << "done.";
}
void avdAnalyzerMdl::batchMerge(int into, int src) { 
  Debug << "entered.  Merging batch[" << src << "] into batch[" << into << "].";
  tListIterator<cAnalyzeGenotype> batch_from_it(batch[src].List());
  cAnalyzeGenotype * genotype = NULL;
  while ((genotype = batch_from_it.Next()) != NULL) {
    cAnalyzeGenotype *new_genotype = new cAnalyzeGenotype(*genotype);
    batch[into].List().PushRear(new_genotype);
  }
  batch[into].SetLineage(false);
  batch[into].SetAligned(false);
  Debug << "done.";
}
bool avdAnalyzerMdl::batchName(int idx, cString name) { 
  Debug << "entered.";
  assert(0<=idx && idx<MAX_BATCHES);
  if (name.CountNumWords() == 0) {
    Error << "didn't rename batch because no new name was given.";
    return false;
  }
  batch[idx].Name() = name.PopWord();
  Debug << "new name: \"" << batch[idx].Name() << "\".";
  Debug << "done.";
  return true;
}

const cString &avdAnalyzerMdl::getBatchName(int idx){
  Debug << "...";
  return batch[idx].Name();
}


void avdAnalyzerMdl::setCurrentBatchIdx(int idx){
  Debug << "entered.";
  assert(0<=idx && idx<MAX_BATCHES);
  cur_batch = idx;
  Debug << "new index: " << idx << ".";
  Debug << "done.";
}
int avdAnalyzerMdl::getCurrentBatchIdx(void){
  return cur_batch;
}

cGenotypeBatch *avdAnalyzerMdl::getBatchAt(int idx){
  Debug << "entered.";
  assert(0<=idx && idx<MAX_BATCHES);
  Debug << "done.";
  return batch + idx;
  Debug << "done.";
  return 0;
}


void avdAnalyzerMdl::LoadDataEntryCommands(
  cStringList arg_list,
  tRetainableList< avdAnalyzerDataEntryCommand<cAnalyzeGenotype> > &output_list
){
  Debug << "entered.";

  // If no args were given, load all of the stats.
  if (arg_list.GetSize() == 0) {
    tRetainableListIter< avdAnalyzerDataEntryBase<cAnalyzeGenotype> >
      genotype_data_it(getGenotypeDataList());
    while (genotype_data_it.Next() != (void *) NULL) {
      avdAnalyzerDataEntryCommand<cAnalyzeGenotype> * entry_command =
        new avdAnalyzerDataEntryCommand<cAnalyzeGenotype>(genotype_data_it.Get());
      output_list.PushRear(entry_command);
      entry_command->nameWatched("<avdAnalyzerMdl::LoadDataEntryCommands(entry_command)>");
      entry_command->release();
    }
  }
  // Otherwise, load only those listed.
  else {
    while (arg_list.GetSize() != 0) {
      // Setup the next entry
      cString cur_args = arg_list.Pop();
      cString cur_entry = cur_args.Pop(':');
      bool found_entry = false;

      Debug << "new arg: " << cur_args << ", entry: " << cur_entry;

      // Scan the genotype data list for the current entry
      tRetainableListIter< avdAnalyzerDataEntryBase<cAnalyzeGenotype> >
        genotype_data_it(getGenotypeDataList());

      while (genotype_data_it.Next() != (void *) NULL) {
        if (genotype_data_it.Get()->GetName() == cur_entry) {
          avdAnalyzerDataEntryCommand<cAnalyzeGenotype> * entry_command =
            new avdAnalyzerDataEntryCommand<cAnalyzeGenotype>
            (genotype_data_it.Get(), cur_args);
          output_list.PushRear(entry_command);
          entry_command->nameWatched("<avdAnalyzerMdl::LoadDataEntryCommands(entry_command)>");
          entry_command->release();
          Debug << "added new entry command.";
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

        Error << "Warning: Format entry \"" << cur_entry
             << "\" not found.  Best match is \""
             << best_entry << "\".";
      }

    }
  }

  Debug << "output_list.GetSize(): " << output_list.GetSize();
  Debug << "done.";
}

tRetainableList< avdAnalyzerDataEntryBase<cAnalyzeGenotype> > &
avdAnalyzerMdl::getGenotypeDataList(void){
  Debug << "entered.";
  SetupGenotypeDataList();
  if(m_genotype_data_list.GetSize() == 0){
    tRetainableListIter<avdAnalyzerDataEntryBase<cAnalyzeGenotype> >
      it(m_genotype_data_list);
    it.Reset();
    while(it.Next() != 0)
      /*
      This is so not typesafe:
      */
      m_genotype_data_list.PushRear(
        static_cast<avdAnalyzerDataEntryBase<cAnalyzeGenotype> *>(it.Get())
      );
      /*
      It only works because avdAnalyzerMdl::SetupGenotypeDataList(),
      defined above, has pushed avdAnalyzerDataEntry objects into
      m_genotype_data_list, instead of tDataEntry objects.
      */
  }
  Debug << "done.";
  return m_genotype_data_list;
}

avdAnalyzerMdl::avdAnalyzerMdl():cAnalyze(){
  Debug << "constructor.";
  m_genotype_data_list.nameWatched(
    "<avdAnalyzerMdl(m_genotype_data_list)>");
}

// arch-tag: implementation file for analyzer model object
