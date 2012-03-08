//
//  cMigrationMatrix.h
//  Avida-Core
//
//  MIGRATION_MATRIX
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//


#ifndef cMigrationMatrix_h
#define cMigrationMatrix_h

#include "cAvidaContext.h"
#include "cRandom.h"
#include "cString.h"
#include "tSmartArray.h"

namespace Avida {
    class Feedback;
};

using namespace Avida;

class cMigrationMatrix
{
  
    public:
        cMigrationMatrix();
        ~cMigrationMatrix();

        bool AlterConnectionWeight(const int from_deme_id, const int to_deme_id, const double alter_amount);
        int GetProbabilisticDemeID(const int from_deme_id,cRandom& p_rng,bool p_is_parasite_migration);
        cString GetParasiteCountMatrixChars();
        cString GetOffspringCountMatrixChars();
        bool Load(const int num_demes, const cString& filename, const cString& working_dir,bool p_count_parasites, bool p_count_offspring, bool p_is_reload, Feedback& feedback);
        void Print();
        void ResetParasiteCounts();
        void ResetOffspringCounts();
    
    private:
        tSmartArray< tSmartArray<double> > m_migration_matrix;
        tSmartArray<double> m_row_connectivity_sums;
        tSmartArray< tSmartArray<int> > m_parasite_migration_counts;
        tSmartArray< tSmartArray<int> > m_offspring_migration_counts;
        

    
    
    
};

#endif
