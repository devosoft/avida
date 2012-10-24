//
//  cMigrationMatrix.cc
//  Avida-Core
//
//  Created by Chad Byers on 2/16/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#include "cMigrationMatrix.h"

#include "avida/core/Feedback.h"
#include "cInitFile.h"
#include "cString.h"
#include "cStringUtil.h"

cMigrationMatrix::cMigrationMatrix(){
    
};

cMigrationMatrix::~cMigrationMatrix(){
    
}

int cMigrationMatrix::GetOffspringCountAt(int from_deme_id, int to_deme_id){
  assert(from_deme_id >= 0 && from_deme_id < m_offspring_migration_counts.GetSize());
  assert(to_deme_id >= 0 && to_deme_id < m_offspring_migration_counts[from_deme_id].GetSize());
  return m_offspring_migration_counts[from_deme_id][to_deme_id];
};

int cMigrationMatrix::GetParasiteCountAt(int from_deme_id, int to_deme_id){
  assert(from_deme_id >= 0 && from_deme_id < m_parasite_migration_counts.GetSize());
  assert(to_deme_id >= 0 && to_deme_id < m_parasite_migration_counts[from_deme_id].GetSize());  
  return m_parasite_migration_counts[from_deme_id][to_deme_id];
};

bool cMigrationMatrix::AlterConnectionWeight(const int from_deme_id, const int to_deme_id, const double alter_amount){
  m_migration_matrix[from_deme_id][to_deme_id] += alter_amount;
  m_row_connectivity_sums[from_deme_id] += alter_amount;
  double row_sum = 0.0;
  for(int col = 0; col < m_migration_matrix[from_deme_id].GetSize();col++){
    row_sum += m_migration_matrix[from_deme_id][col];
  }
  if(m_migration_matrix[from_deme_id][to_deme_id] < 0.0 || row_sum <= 0.0){
    return false;
  }
  else
    return true;
};

int cMigrationMatrix::GetProbabilisticDemeID(const int from_deme_id, Apto::Random& p_rng,bool p_is_parasite_migration){
    assert(0 <= from_deme_id && from_deme_id < m_migration_matrix.GetSize());
    double rand_dbl_value_in_range = p_rng.GetDouble(m_row_connectivity_sums[from_deme_id]);
    for(int col = 0; col < m_migration_matrix[from_deme_id].GetSize(); col++){
        rand_dbl_value_in_range -= m_migration_matrix[from_deme_id][col];
        if(rand_dbl_value_in_range <= 0.0){
            if(p_is_parasite_migration)
              m_parasite_migration_counts[from_deme_id][col] += 1;
            else
              m_offspring_migration_counts[from_deme_id][col] += 1;
          
            return col;
        }
    }    
    // Should never get to this point
    assert(false);
  return -1;
};

bool cMigrationMatrix::Load(const int num_demes, const cString& filename, const cString& working_dir,bool p_count_parasites, bool p_count_offspring, bool p_is_reload, Feedback& feedback){
  m_migration_matrix.ResizeClear(0);
  m_row_connectivity_sums.ResizeClear(0);
  cInitFile infile(filename, working_dir);
  if (!infile.WasOpened()) {
    for (int i = 0; i < infile.GetFeedback().GetNumMessages(); i++) {
      switch (infile.GetFeedback().GetMessageType(i)) {
        case cUserFeedback::UF_ERROR:
          feedback.Error(infile.GetFeedback().GetMessage(i));
          break;
        case cUserFeedback::UF_WARNING:
          feedback.Warning(infile.GetFeedback().GetMessage(i));
          break;
        default:
          feedback.Notify(infile.GetFeedback().GetMessage(i));
      }
    }
    feedback.Error("failed to load migration matrix '%s'", (const char*)filename);
    return false;
  }
  
  Apto::Array<double, Apto::Smart> f_temp_row;
  for (int line_id = 0; line_id < infile.GetNumLines(); line_id++) {
    // Load the next line from the file.
    f_temp_row.ResizeClear(0);
    cString f_curr_line = infile.GetLine(line_id);
    double f_row_sum = 0.0;
    while(!f_curr_line.IsEmpty()){
      double val = f_curr_line.Pop(',').AsDouble();
      if(val < 0.0){
        feedback.Error("Cannot have a negative connection in connection matrix");
        return false;
      }
      f_row_sum += val;
      f_temp_row.Push(val);
    }
    if(f_row_sum == 0.0){
      feedback.Error("Cannot have a row sum of 0.0 in connection matrix");
      return false;
    }
    else{
      m_row_connectivity_sums.Push(f_row_sum);
    }
    m_migration_matrix.Push(f_temp_row);
  }
  
  if(num_demes != m_migration_matrix.GetSize()){
    feedback.Error("The number of demes in the migration matrix (%i) did not match the NUM_DEMES (%i) parameter in avida.cfg.",m_migration_matrix.GetSize(),num_demes);
    return false;
  }
  for(int f_row = 0; f_row < m_migration_matrix.GetSize(); f_row++){
    if(m_migration_matrix[f_row].GetSize() != m_migration_matrix.GetSize()){
      feedback.Error("The number of columns in row %i did not match total number of demes",f_row);
      return false;
    }
    if(m_migration_matrix[f_row].GetSize() != num_demes){
      feedback.Error("The number of demes in the migration matrix (%i) did not match the NUM_DEMES (%i) parameter in avida.cfg.",m_migration_matrix.GetSize(),num_demes);
      return false;
    }
  }
  
  if(p_count_parasites && !p_is_reload){
    Apto::Array<int, Apto::Smart> blank(num_demes);
    blank.SetAll(0);
    
    m_parasite_migration_counts.Resize(num_demes, blank);
    ResetParasiteCounts();
  }
  
  if(p_count_offspring && !p_is_reload){
    Apto::Array<int, Apto::Smart> blank(num_demes);
    blank.SetAll(0);
    m_offspring_migration_counts.Resize(num_demes, blank);
    ResetOffspringCounts();
  }
  
  return true;
}

void cMigrationMatrix::Print(){
    for(int row = 0; row < m_migration_matrix.GetSize(); row++){
        for(int col = 0; col < m_migration_matrix[row].GetSize(); col++){
            std::cout << m_migration_matrix[row][col];
            if(col + 1 < m_migration_matrix[row].GetSize())
                std::cout << ",";
        }
        std::cout << std::endl;
    }
};

void cMigrationMatrix::ResetParasiteCounts(){
  for(int row = 0; row < m_parasite_migration_counts.GetSize(); row++){
    for(int col = 0; col < m_parasite_migration_counts[row].GetSize(); col++){
      m_parasite_migration_counts[row][col] = 0;
    }
  }
};

void cMigrationMatrix::ResetOffspringCounts(){
  for(int row = 0; row < m_offspring_migration_counts.GetSize(); row++){
    for(int col = 0; col < m_offspring_migration_counts[row].GetSize(); col++){
      m_offspring_migration_counts[row][col] = 0;
    }
  }
};
