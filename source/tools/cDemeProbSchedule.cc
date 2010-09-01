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

#include "cDemeProbSchedule.h"

#include "cChangeList.h"
#include "cDeme.h"
#include "cMerit.h"

// The larger merits cause problems here; things need to be re-thought out. 

/* 
 @AWC -- the previous comment is "inherited" from cProbScheduler, this is 
 still a within deme issue.  However, this class mitigates the problem somewhat
 when using more than one deme!

 Yes, I know we're not *suppose* to reuse code -- DEAL
*/

//get the next CPU cycle, awarded to the next populated deme and cycled in a round-robin fashion
int cDemeProbSchedule::GetNextID()
{
  // iterate the deme
  curr_deme = ++curr_deme % num_demes;

  //loop to check each deme at most once -- this could be a problem in sparse pops, best to start with populated or mostly-poulated demes
  for (int i = 0; i < num_demes; i++) {

    // check to see if deme is populated
    if (chart[curr_deme]->GetTotalWeight() == 0) { 
      // deme is empty -- iterate the deme
      curr_deme = ++curr_deme % num_demes;
    } else {
      // deme not empty -- return offset id

      // calculate the offset
      int offset = curr_deme * deme_size;
      
      // get the within postion of the node whos corresponding cell will get the CPU cycle
      const double position = m_rng.GetDouble(chart[curr_deme]->GetTotalWeight());

      // return the adjusted ID of the cell to get the CPU cycle
      return chart[curr_deme]->FindPosition(position) + offset;
    }
  }
  
  assert(false);
  return -1;
}


//adjust the weight of an org within deme
void cDemeProbSchedule::Adjust(int item_id, const cMerit& item_merit, int deme_id)
{
  //calculate the corrected id for the org to be adjusted
  int offset_id = item_id - (deme_id * deme_size);
  
  //who the hell knows what this crap does -- "ineherited" from cProbScheduler
  if (cChangeList *change_list = GetChangeList()) {
    change_list->MarkChange(item_id);
  }
  
  //adjust the merit of the org in the tree
  chart[deme_id]->SetWeight(offset_id, item_merit.GetDouble());
}

