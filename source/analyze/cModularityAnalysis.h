/*
 *  cModularityAnalysis.h
 *  Avida
 *
 *  Created by David on 1/11/09.
 *  Michigan State University. All rights reserved.
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

#ifndef cModularityAnalysis_h
#define cModularityAnalysis_h

#ifndef cGenotypeData_h
#include "cGenotypeData.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cAvidaContext;
class cAnalyzeGenotype;


class cModularityAnalysis
{
private:
  cAnalyzeGenotype* m_genotype;
  
public:
  cModularityAnalysis(cAnalyzeGenotype* genotype) : m_genotype(genotype) { ; }
  
  static void Initialize();
  
  void CalcFunctionalModularity(cAvidaContext& ctx);
  
  
private:
  class cModularityData : public cGenotypeData
  {
  public:
    int tasks_done;
    int insts_tasks;
    double tasks_prop;
    double ave_tasks_per_site;
    double ave_sites_per_task;
    double ave_prop_nonoverlap;
    tArray<double> ave_sites_per_task_indv;
    tArray<double> std_sites_per_task_indv;
    tArray<double> ave_sites_inv_per_task;
    tArray<double> ave_task_length;
    
    cModularityData();
  };
};

#endif
