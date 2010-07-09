/*
 *  cDemeProbSchedule.cc
 *  Avida
 *  Implemented by Art Covert 2/11/2008
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cProbDemeProbSchedule.h"

#include "cDeme.h"
#include "cChangeList.h"
#include "cMerit.h"

// The larger merits cause problems here; things need to be re-thought out. 

/* 
 @AWC -- the previous comment is "inherited" from cProbScheduler, this is 
 still a within deme issue.  However, this class mitigates the problem somewhat
 when using more than one deme!

 Yes, I know we're not *suppose* to reuse code -- DEAL
*/

//get the next CPU cycle, awarded probabistically to a deme based on population with non-zero merits
int cProbDemeProbSchedule::GetNextID()
{
  //choose a deme to schedule
  curr_deme = demeChart.FindPosition(m_rng.GetDouble(demeChart.GetTotalWeight()));

  //calculate the offset
  int offset = curr_deme * deme_size;
  
  // get the within postion of the node whos corresponding cell will get the CPU cycle
  const double position = m_rng.GetDouble(chart[curr_deme]->GetTotalWeight());
  
  // return the adjusted ID of the cell to get the CPU cycle
  return chart[curr_deme]->FindPosition(position) + offset;
}


//adjust the weight of an org within deme -- adjust the weighting for scheduling that deme
void cProbDemeProbSchedule::Adjust(int item_id, const cMerit& item_merit, int deme_id)
{
  //calculate the corrected id for the org to be adjusted
  int offset_id = item_id - (deme_id * deme_size);

  //is this cell about to be populated by a living organism?
  if (item_merit.GetDouble() > 0.0) {
    if (chart[deme_id]->GetWeight(offset_id) == 0.0) {  //...was it previously unpopulated?  -- if so, population size has increased
      demeChart.SetWeight(deme_id,demeChart.GetWeight(deme_id) + 1.0);//increment the deme's weight to reflect the new population size
    }
  } else { //by definition the merit is zero -- no such thing as merits less than 0.0 in Avida
    if (chart[deme_id]->GetWeight(offset_id) > 0.0) { //...was the cell previously populated -- is so, populatino size has decreased
      demeChart.SetWeight(deme_id,demeChart.GetWeight(deme_id) - 1.0);//decrement the deme's weight to reflect the new population size
    }
  }
       

  //"inherited" from cProbScheduler
  if (cChangeList *change_list = GetChangeList()) {
    change_list->MarkChange(item_id);
  }

  //adjust the merit of the org in the tree
  chart[deme_id]->SetWeight(offset_id, item_merit.GetDouble());
}
