/*
 *  cBirthMatingTypeGlobalHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cBirthMatingTypeGlobalHandler.h"

#include "avida/core/Genome.h"
#include "avida/output/File.h"

#include "cBirthChamber.h"
#include "cOrganism.h"
#include "cWorld.h"
#include "cEnvironment.h"
#include "cTaskEntry.h"
#include "cString.h"

#include <iostream>


cBirthMatingTypeGlobalHandler::~cBirthMatingTypeGlobalHandler()
{
}

cBirthEntry* cBirthMatingTypeGlobalHandler::SelectOffspring(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent)
{
  int parent_sex = parent->GetPhenotype().GetMatingType();
  
  //Parent has not sexually matured
  if (parent_sex == MATING_TYPE_JUVENILE) {
    return NULL;
  }
  
  //Parent was a female
  if (parent_sex == MATING_TYPE_FEMALE) {
    int mate_choice_method = parent->GetPhenotype().GetMatePreference();
    return selectMate(ctx, offspring, parent, MATING_TYPE_MALE, mate_choice_method);
  }
  
  //Parent was a male
  if (parent_sex == MATING_TYPE_MALE) {
    if (m_world->GetConfig().LEKKING.Get() != 0) {
      //Lekking is turned on, so if the parent is a male, force the offspring into the birth chamber,
      //  and wait for a female
      storeOffspring(ctx, offspring, parent);
      return NULL;
    } else {
      //Lekking is off, so find a mate
      //if there is none, store the current one
      //if there is, mate with it
      int mate_choice_method = parent->GetPhenotype().GetMatePreference();
      return selectMate(ctx, offspring, parent, MATING_TYPE_FEMALE, mate_choice_method);
    }
  }
  
  //One of the previous conditions should have been met, so hopefully the following line will never
  //be executed, but just in case...
  return NULL;
}


//Returns the number of offspring of a specific mating type waiting in the birth chamber
int cBirthMatingTypeGlobalHandler::GetWaitingOffspringNumber(int which_mating_type)
{
  //if (which_mating_type == -1) return 0;
  int num_waiting = 0;
  
  for (int i = 0; i < m_entries.GetSize(); i++) {
    if (m_bc->ValidateBirthEntry(m_entries[i])) {
      if (m_entries[i].GetMatingType() == which_mating_type) num_waiting++;
    }
  }
  return num_waiting;
}

/*
//Fills in results_array with the min, mean, and maximum parent_task_count of the offspring waiting in the birth chamber for task_id
void cBirthMatingTypeGlobalHandler::GetWaitingOffspringTaskData(int task_id, float results_array[])
{  
  
  int min = -1, max = -1;
  int sum = 0, valid_count = 0;
  int num_waiting = 0;
  
  //Do the females
  num_waiting = m_entries_female.GetSize();
  //Don't bother doing anything if the birth chamber is empty
  if (num_waiting > 0) {
    //Loop through all the offspring and collect stats
    for (int i = 0; i < num_waiting; i++) {
      if (m_bc->ValidateBirthEntry(m_entries_female[i])) {
        
        //If the current offspring count is less than the previous min, save it
        if (min == -1) min = m_entries_female[i].GetParentTaskCount()[task_id];
        else min = ( (m_entries_female[i].GetParentTaskCount()[task_id] < min) ? m_entries_female[i].GetParentTaskCount()[task_id] : min);
        
        //If the current offspring count is greater than the previous max, save it
        if (max == -1) max = m_entries_female[i].GetParentTaskCount()[task_id];
        else max = ( (m_entries_female[i].GetParentTaskCount()[task_id] > max) ? m_entries_female[i].GetParentTaskCount()[task_id] : max);
        
        //Add the current task count to the sum and increase the counter of actual valid offspring
        sum += m_entries_female[i].GetParentTaskCount()[task_id];
        valid_count++;      
      }
    }
  }
  
  //Do the males
  num_waiting = m_entries_male.GetSize();
  //Don't bother doing anything if the birth chamber is empty
  if (num_waiting > 0) {
    //Loop through all the offspring and collect stats
    for (int i = 0; i < num_waiting; i++) {
      if (m_bc->ValidateBirthEntry(m_entries_male[i])) {
      
        //If the current offspring count is less than the previous min, save it
        if (min == -1) min = m_entries_male[i].GetParentTaskCount()[task_id];
        else min = ( (m_entries_male[i].GetParentTaskCount()[task_id] < min) ? m_entries_male[i].GetParentTaskCount()[task_id] : min);
      
        //If the current offspring count is greater than the previous max, save it
        if (max == -1) max = m_entries_male[i].GetParentTaskCount()[task_id];
        else max = ( (m_entries_male[i].GetParentTaskCount()[task_id] > max) ? m_entries_male[i].GetParentTaskCount()[task_id] : max);
      
        //Add the current task count to the sum and increase the counter of actual valid offspring
        sum += m_entries_male[i].GetParentTaskCount()[task_id];
        valid_count++;      
      }
    }
  }
  
  //Do the undefined mating type offspring
 if (m_bc->ValidateBirthEntry(m_entry_undefined)) {      
    //If the current offspring count is less than the previous min, save it
    if (min == -1) min = m_entry_undefined.GetParentTaskCount()[task_id];
    else min = ( (m_entry_undefined.GetParentTaskCount()[task_id] < min) ? m_entry_undefined.GetParentTaskCount()[task_id] : min);
    
    //If the current offspring count is greater than the previous max, save it
    if (max == -1) max = m_entry_undefined.GetParentTaskCount()[task_id];
    else max = ( (m_entry_undefined.GetParentTaskCount()[task_id] > max) ? m_entry_undefined.GetParentTaskCount()[task_id] : max);
    
    //Add the current task count to the sum and increase the counter of actual valid offspring
    sum += m_entry_undefined.GetParentTaskCount()[task_id];
    valid_count++;      
  } 
  
  results_array[0] = min;
  results_array[1] = ( valid_count == 0 ? (float) -1 : ((float) sum) / ((float) valid_count)  );
  results_array[2] = max;
  
} //end GetWaitingOffspringTaskData
*/

//Returns the id number that corresponds to a particular task name
//Returns -1 if the task is not being monitored in the current environment
int cBirthMatingTypeGlobalHandler::getTaskID(cString task_name, cWorld* world)
{
  int num_tasks = world->GetEnvironment().GetNumTasks();
  for (int i = 0; i < num_tasks; i++) {
    if (world->GetEnvironment().GetTask(i).GetName() == task_name) return i;
  }
  return -1;
}

//Stores the specified offspring in the specified birth chamber
void cBirthMatingTypeGlobalHandler::storeOffspring(cAvidaContext&, const Genome& offspring, cOrganism* parent)
{
  //First, don't bother doing ANYTHING if LEKKING is turned on and the parent is a female --
  // -- in this case, there's no point in putting her offspring in the birth chamber because 
  // the males will never choose it; the males always go directly into the birth chamber
  if ((m_world->GetConfig().LEKKING.Get() != 0) & (parent->GetPhenotype().GetMatingType() == MATING_TYPE_FEMALE)) {
    return;
  }
  
  //Find an empty entry
  //If there are none, make room for one
  //But if the birth chamber is at the size limit already, over-write the oldest one
  int oldest_index = -1;
  int entry_list_size = m_entries.GetSize();
  //Loop through all the birth entries
  for (int i = 0; i < entry_list_size; i++) {
    if (m_bc->ValidateBirthEntry(m_entries[i])) {
      //Current entry is valid, so let's just keep track of the oldest one
      if (oldest_index == -1) oldest_index = i;
      else oldest_index = (m_entries[i].timestamp < m_entries[oldest_index].timestamp ? i : oldest_index);
    } else {
      //Current entry is empty, so let's use this one
      m_bc->ClearEntry(m_entries[i]);
      m_bc->StoreAsEntry(offspring, parent, m_entries[i]);
      return;
    }
  }

  //If we're still here, it means we didn't find any empty entries
  //So, let's make room for one and then store it; but if the list is already at its max size,
  // we'll just have to over-write the oldest one
  int store_index = m_entries.GetSize();
  int max_buffer_size = m_world->GetConfig().MAX_GLOBAL_BIRTH_CHAMBER_SIZE.Get();
  if (store_index >= max_buffer_size) {
    //m_bc->ClearEntry(m_entries[oldest_index]);
    store_index = oldest_index;
  } else {
    m_entries.Resize(store_index + 1);
  }
  
  m_bc->ClearEntry(m_entries[store_index]);
  m_bc->StoreAsEntry(offspring, parent, m_entries[store_index]);
}

//Compares two birth entries and decides which one is preferred
//Returns true if the first one is "better"
//Returns false if the second one is
bool cBirthMatingTypeGlobalHandler::compareBirthEntries(cAvidaContext& ctx, int mate_choice_method, const cBirthEntry& entry1, const cBirthEntry& entry2)
{
  assert(mate_choice_method != MATE_PREFERENCE_RANDOM);
  
  double cv = m_world->GetConfig().MATE_ASSESSMENT_CV.Get();
  double value1 = 0;
  double value2 = 0;
  
  switch (mate_choice_method) {
    case MATE_PREFERENCE_HIGHEST_DISPLAY_A: //Prefers to mate with the organism with the highest value of mating display A
      value1 = (double) entry1.GetMatingDisplayA();
      value2 = (double) entry2.GetMatingDisplayA();
      if (m_world->GetConfig().NOISY_MATE_ASSESSMENT.Get()) {
        value1 += ctx.GetRandom().GetRandNormal(0, value1*cv);
        value2 += ctx.GetRandom().GetRandNormal(0, value2*cv);
      }
      return (value1 > value2);
      break;
    
    case MATE_PREFERENCE_HIGHEST_DISPLAY_B: //Prefers to mate with the organism with the highest value of mating display B
      value1 = (double) entry1.GetMatingDisplayB();
      value2 = (double) entry2.GetMatingDisplayB();
      if (m_world->GetConfig().NOISY_MATE_ASSESSMENT.Get()) {
        value1 += ctx.GetRandom().GetRandNormal(0, value1*cv);
        value2 += ctx.GetRandom().GetRandNormal(0, value2*cv);
      }
      return (value1 > value2);
      break;
      
    case MATE_PREFERENCE_HIGHEST_MERIT:
      value1 = (double) entry1.merit.GetDouble();
      value2 = (double) entry2.merit.GetDouble();
      if (m_world->GetConfig().NOISY_MATE_ASSESSMENT.Get()) {
        value1 += ctx.GetRandom().GetRandNormal(0, value1*cv);
        value2 += ctx.GetRandom().GetRandNormal(0, value2*cv);
      }
      return (value1 > value2);
      break;
    case MATE_PREFERENCE_LOWEST_DISPLAY_A: //Prefers to mate with the organism with the highest value of mating display A
      value1 = (double) entry1.GetMatingDisplayA();
      value2 = (double) entry2.GetMatingDisplayA();
      if (m_world->GetConfig().NOISY_MATE_ASSESSMENT.Get()) {
        value1 += ctx.GetRandom().GetRandNormal(0, value1*cv);
        value2 += ctx.GetRandom().GetRandNormal(0, value2*cv);
      }
      return (value1 < value2);
      break;
    
    case MATE_PREFERENCE_LOWEST_DISPLAY_B: //Prefers to mate with the organism with the highest value of mating display B
      value1 = (double) entry1.GetMatingDisplayB();
      value2 = (double) entry2.GetMatingDisplayB();
      if (m_world->GetConfig().NOISY_MATE_ASSESSMENT.Get()) {
        value1 += ctx.GetRandom().GetRandNormal(0, value1*cv);
        value2 += ctx.GetRandom().GetRandNormal(0, value2*cv);
      }
      return (value1 < value2);
      break;
      
    case MATE_PREFERENCE_LOWEST_MERIT:
      value1 = (double) entry1.merit.GetDouble();
      value2 = (double) entry2.merit.GetDouble();
      if (m_world->GetConfig().NOISY_MATE_ASSESSMENT.Get()) {
        value1 += ctx.GetRandom().GetRandNormal(0, value1*cv);
        value2 += ctx.GetRandom().GetRandNormal(0, value2*cv);
      }
      return (value1 < value2);
      break;
  }
  
  //If we're still here... just decide randomly since we need to give some return value...
  return ctx.GetRandom().P(0.5);
}

//Selects a mate for the current offspring/gamete
//If none is found, it returns NULL
cBirthEntry* cBirthMatingTypeGlobalHandler::selectMate(cAvidaContext& ctx, const Genome& offspring, cOrganism* parent, int which_mating_type, int mate_choice_method)
{
  //Loop through the entry list and find a mate
  //If none are found, store the current offspring and return NULL
  int num_waiting = m_entries.GetSize();
  if (num_waiting == 0) {
    storeOffspring(ctx, offspring, parent);
    return NULL;
  }
  
  int selected_index = -1;
  
  if (m_world->GetConfig().FORCED_MATE_PREFERENCE.Get() != -1) {
    mate_choice_method = m_world->GetConfig().FORCED_MATE_PREFERENCE.Get();
  }
  
  if (mate_choice_method == MATE_PREFERENCE_RANDOM) {
    //This is a non-choosy individual, so pick a mate randomly!
    //First, get a list of every element of m_entries that contains a waiting offspring (of the compatible sex)
    //Then pick one at random
    Apto::Array<int> compatible_entries; //This will hold a list of all the compatible birth entries waiting in the birth chamber
    compatible_entries.Resize(num_waiting, -1);
    int last_compatible = -1; //The index of the last entry in compatible_entries holding a compatible m_entries index
    for (int i = 0; i < num_waiting; i++) {
      if (m_bc->ValidateBirthEntry(m_entries[i])) { //Is the current entry valid/alive?
        //Here, check to see if the current entry belongs to the parent's group! @CHC
        bool groups_match = false;
        if (!(m_world->GetConfig().MATE_IN_GROUPS.Get())) {
          groups_match = true; //Within-group mating is turned off, so don't need to check
        } else {
          //Within-group mating is turned on, so we need to check if the parent's group is the same
          if (parent->HasOpinion()) {
            groups_match = (m_entries[i].GetGroupID() == parent->GetOpinion().first);
          }
        }
        if (groups_match) {
          if (m_entries[i].GetMatingType() == which_mating_type) { //Is the current entry a compatible mating type?
            last_compatible++;
            compatible_entries[last_compatible] = i;
          }
        }
      }
    }
    if (last_compatible > -1) { //Don't bother picking one if we haven't found any compatible entries
      selected_index = compatible_entries[ctx.GetRandom().GetUInt(last_compatible+1)];
    }    
  } else {
    //This is a choosy female, so go through all the mates and pick the "best" one!
    for (int i = 0; i < num_waiting; i++) {
      if (m_bc->ValidateBirthEntry(m_entries[i])) { //Is the current entry valid/alive?
        //Here, check to see if the current entry belongs to the parent's group! @CHC
        bool groups_match = false;
        if (!(m_world->GetConfig().MATE_IN_GROUPS.Get())) {
          groups_match = true; //Within-group mating is turned off, so don't need to check
        } else {
          //Within-group mating is turned on, so we need to check if the parent's group is the same
          if (parent->HasOpinion()) {
            groups_match = (m_entries[i].GetGroupID() == parent->GetOpinion().first);
          }
        }
        if (groups_match) {
          if (m_entries[i].GetMatingType() == which_mating_type) { //Is the current entry a compatible mating type?
            if (selected_index == -1) selected_index = i;
            else selected_index = compareBirthEntries(ctx, mate_choice_method, m_entries[i], m_entries[selected_index]) ? i : selected_index;
          }
        }
      }
    }
  }
  
  if (selected_index == -1) {
    //None found: Store the current one and return NULL
    storeOffspring(ctx, offspring, parent);
    return NULL;
  }
  //cout << "Selected " << m_entries[selected_index].GetPhenotypeString() << "\n";
  return &(m_entries[selected_index]);
  
}



int cBirthMatingTypeGlobalHandler::getWaitingOffspringMostTask(int which_mating_type, int task_id)
{
  int selected_index = -1;
  int num_waiting = m_entries.GetSize();
  if (num_waiting == 0) return -1;
  for (int i = 0; i < num_waiting; i++) {
    if (m_bc->ValidateBirthEntry(m_entries[i])) {
      if (m_entries[i].GetMatingType() == which_mating_type) {
        if (selected_index == -1) selected_index = i;
        else selected_index = ((m_entries[i].GetParentTaskCount()[task_id] > m_entries[selected_index].GetParentTaskCount()[task_id]) ? i : selected_index);
      }
    }
  }
  return selected_index;
}

//Outputs the entire birth chamber to a file
void cBirthMatingTypeGlobalHandler::PrintBirthChamber(const cString& filename)
{
  Apto::String file_path((const char*)filename);
  Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), file_path);
  df->WriteTimeStamp();
  df->WriteComment(cBirthEntry::GetPhenotypeStringFormat());
  df->Endl();
  
  std::ofstream& df_stream = df->OFStream();
  
  for (int i = 0; i < m_entries.GetSize(); i++) {
    if (m_bc->ValidateBirthEntry(m_entries[i])) {
      df_stream << m_entries[i].GetPhenotypeString() << endl;
    }
  }
}
