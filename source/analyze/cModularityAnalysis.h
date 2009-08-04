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
#ifndef cCPUTestInfo_h
#include "cCPUTestInfo.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cAvidaContext;
class cAnalyzeGenotype;
class cFlexVar;
class cString;


class cModularityAnalysis
{
private:
  cAnalyzeGenotype* m_genotype;
  cCPUTestInfo m_test_info;
public:
  cModularityAnalysis(cAnalyzeGenotype* genotype, const cCPUTestInfo& test_info) : m_genotype(genotype), m_test_info(test_info) { ; }
  
  static void Initialize();
  
  void CalcFunctionalModularity(cAvidaContext& ctx);
  
  static cFlexVar GetTasksDoneFor(const cAnalyzeGenotype* genotype);
  static cFlexVar GetInstsInvolvedInTasksFor(const cAnalyzeGenotype* genotype);
  static cFlexVar GetTaskProportionFor(const cAnalyzeGenotype* genotype);
  static cFlexVar GetAveTasksPerSiteFor(const cAnalyzeGenotype* genotype);
  static cFlexVar GetAveSitesPerTaskFor(const cAnalyzeGenotype* genotype);
  static cFlexVar GetPropNonoverlapFor(const cAnalyzeGenotype* genotype);
  
  static cFlexVar GetSitesPerTaskFor(const cAnalyzeGenotype* genotype, int idx);
  static cString DescSitesPerTask(const cAnalyzeGenotype* genotype, int idx);  
  static cFlexVar GetSitesInvolvedInXTasksFor(const cAnalyzeGenotype* genotype, int idx);
  static cString DescSitesInvolvedInXTasks(const cAnalyzeGenotype* genotype, int idx);
  static cFlexVar GetTaskLengthFor(const cAnalyzeGenotype* genotype, int idx);
  static cString DescTaskLength(const cAnalyzeGenotype* genotype, int idx);
  static cFlexVar GetAveTaskPositionFor(const cAnalyzeGenotype* genotype, int idx);
  static cString DescAveTaskPosition(const cAnalyzeGenotype* genotype, int idx);

  
private:
  class cModularityData : public cGenotypeData
  {
  public:
    int tasks_done;                           // number of tasks performed by this genotype
    int insts_tasks;                          // number of instructions involved in task performance
    double tasks_prop;                        // proportion of sites used in tasks
    double ave_tasks_per_site;                // average tasks per site
    double ave_sites_per_task;                // average sites per task
    double ave_prop_nonoverlap;               // average proportion of nonoverlap
    tArray<int> sites_per_task;               // number sites used for each task
    tArray<int> sites_inv_x_tasks;            // Number of sites involved in 0, 1, 2, 3... tasks
    tArray<int> task_length;                  // Length of each task from first to last instruction
    tArray<double> ave_task_position;         // Average task position
    
    cModularityData();
  };
};

#endif
