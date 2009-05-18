/*
 *  cOrgMovementPredicate.h
 *  Avida
 *
 *  Created by Benjamin Beckmann on 11/6/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

/*
 *  cOrgMessagePredicate.h
 *  Avida
 *
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
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
#ifndef cOrgMovementPredicate_h
#define cOrgMovementPredicate_h

#include <iostream>
#include <functional>
#include <set>

#include "cDemeCellEvent.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cStats.h"


/*! \brief An STL-compatible predicate on movement.  The intent here is to
provide a straightforward way to track arbitrary movement *wherever* they appear
in the population.  */
struct cOrgMovementPredicate : public std::unary_function<cOrganism, bool> 
{
  virtual ~cOrgMovementPredicate() { }
  virtual bool operator()(cOrganism& org) = 0;
  virtual void Print(std::ostream& out) { }
  virtual void Reset() { }
  virtual bool PreviouslySatisfied() = 0;
  virtual cString GetName() = 0;
  virtual void UpdateStats(cStats& stats) {}
  virtual cDemeCellEvent* GetEvent(int i) { return NULL; }
  virtual int GetNumEvents() { return 1; }
};

/*! A predicate that returns true if an organism has made it to the center of the gradient event
*/
struct cOrgMovementPred_EventMovedIntoCenter : public cOrgMovementPredicate {
  cOrgMovementPred_EventMovedIntoCenter(cDemeCellEvent* event, cPopulation& population, int times) :
  pop(population)
  , m_event_received(false)
  , m_stats_updated(false)
  , m_event(event)
  , m_total_times(times)
  , m_current_times(0){ }
  
  ~cOrgMovementPred_EventMovedIntoCenter() { }

  virtual bool operator()(cOrganism& org) {
    int deme_id = org.GetOrgInterface().GetDemeID();
    
    if(deme_id != m_event->GetDeme()->GetDemeID() || m_event->IsDead()) {
      return false;
    }

    if(m_event->IsActive()) {
      // find organism coordinates
      int absolute_cell_ID = org.GetCellID();
      std::pair<int, int> pos = pop.GetDeme(deme_id).GetCellPosition(absolute_cell_ID); 

      // does receiver have x cordinate of zero
      if(m_event->InCenter(pos)) {
        m_current_times++;
        if(m_current_times >= m_total_times) {
          m_event_received = true;
        }
      }
    }
    return m_event_received;
  }
  
  virtual void Print(std::ostream& out) {
    if(m_event->IsDead()) {
      return;
    }
    // WHAT TO PRINT
  }
  
  virtual void Reset() { 
    m_event_received = false;
    m_stats_updated = false;
    m_current_times = 0;
  }

  virtual bool PreviouslySatisfied() {
    return m_event_received;
  }

  virtual cString GetName() {
    return "EventMovedIntoCenter";
  }

  virtual void UpdateStats(cStats& stats) {
    if(m_event_received && !m_stats_updated) {
/*      int eventCell = m_event->GetNextEventCellID();
      while(eventCell != -1) {
        stats.IncPredSat(eventCell);
        eventCell = m_event->GetNextEventCellID();
      }*/
      m_stats_updated = true;
    }
  }
  
  cDemeCellEvent* GetEvent(int i) { return m_event; }
  
  cPopulation& pop;
  bool m_event_received;
  bool m_stats_updated;
  cDemeCellEvent* m_event;
  int m_total_times;
  int m_current_times;
};



/*! A predicate that returns true if an organism has made it to a target cell.
 *  Modified from cOrgMovementPred_EventMovedIntoCenter
*/
struct cOrgMovementPred_EventMovedBetweenTargets : public cOrgMovementPredicate {
  cOrgMovementPred_EventMovedBetweenTargets(tVector<cDemeCellEvent *> events, cPopulation& population, unsigned int times) :
  pop(population)
  , m_event_received(false)
  , m_stats_updated(false)
  , m_events(events)
  , m_total_times(times)
  , m_total_orgs(1) {  // TODO: eventually, want to make this a parameter.  problem is cPopulation::AddDemePred takes only 1 arg

    m_event_success.clear();
    for(int i = 0; i < m_events.Size(); i++) {
      set<int> p;
      p.clear();
      m_event_success.push_back(p);
    }
    m_successful_orgs.clear();
  } //End constructor

  ~cOrgMovementPred_EventMovedBetweenTargets() { }

  virtual bool operator()(cOrganism& org) {
    int deme_id = org.GetOrgInterface().GetDemeID();
    set<int>::iterator it;
    int other_event;    // Index of the other event

    m_event_received = false;

    for(int i = 0; i < m_events.Size(); i++) {
      cDemeCellEvent *event = m_events[i];
      other_event = (i+1) % 2;
 
      if( (event->IsDead() == false) && (event->IsActive()) &&
          (deme_id == event->GetDeme()->GetDemeID()) ) {

        int absolute_cell_ID = org.GetCellID();
        int relative_cell_ID = pop.GetDeme(deme_id).GetRelativeCellID(absolute_cell_ID);  


        if(event->OnEventCell(relative_cell_ID)) {

          int org_id = -1;
          org_id = ((cOrganism&) org).GetID();
	  
	  m_event_success[i].insert(org_id);

          it = m_event_success[other_event].find(org_id);

          if(it != m_event_success[other_event].end())
          {
             // See how many times this org has gone back and forth.  If it has
             // done it enough times, it is considered successful
             unsigned int num_backforth = org.GetNumTaskCellsReached()/2;

             if(num_backforth >= m_total_times) {
               m_successful_orgs.insert(org_id);

               //---- do some logging ----------------------------------------------------------------------
               ofstream predlog;
               predlog.open("data/predlog.dat", ios::app);
               predlog << "Organism " << org_id << " in deme " << deme_id ;
               predlog << " has touched both targets " << num_backforth << " times. " << m_successful_orgs.size();
               predlog << " orgs have done this out of " << m_total_orgs << endl;

               set<int>::iterator it;
               for(unsigned int q = 0; q < m_event_success.size(); q++) {
                 predlog << "ORGS FOR EVENT " << q;
                   for ( it=m_event_success[q].begin() ; it != m_event_success[q].end(); it++ )
                     predlog << " " << *it;
                 predlog << endl;
               }

               predlog.close();
               //---- end of logging -----------------------------------------------------------------------

               // If enough organisms have touched both targets, predicate is satisfied.
               if(m_successful_orgs.size() >= m_total_orgs) {
                 m_event_received = true;
                 return m_event_received;
               }
             }


          } //End if this organism has gone to the other target as well

        } //End if org is in event area

      } //End if this event matches the predicate

    } //End going through events 

    return m_event_received;

  } //End operator()
  
  virtual void Print(std::ostream& out) {
    // WHAT TO PRINT
  }

  virtual void Reset() { 
    m_event_received = false;
    m_stats_updated = false;

    m_event_success.clear();
    for(int i = 0; i < m_events.Size(); i++) {
      set<int> p;
      p.clear();
      m_event_success.push_back(p);
    }
    m_successful_orgs.clear();
  }

  virtual bool PreviouslySatisfied() {
    return m_event_received;
  }

  virtual cString GetName() {
    return "EventMovedBetweenTargets";
  }

  virtual void UpdateStats(cStats& stats) {
    if(m_event_received && !m_stats_updated) {
/*      int eventCell = m_event->GetNextEventCellID();
      while(eventCell != -1) {
        stats.IncPredSat(eventCell);
        eventCell = m_event->GetNextEventCellID();
      }*/
      m_stats_updated = true;
    }
  }
  
  cDemeCellEvent * GetEvent(int i) {
    return m_events[i];
  }

  virtual int GetNumEvents() {
    return m_events.Size();
  }
  
  cPopulation& pop;
  bool m_event_received;
  bool m_stats_updated;
  tVector<cDemeCellEvent *> m_events;
  unsigned int m_total_times;
  unsigned int m_total_orgs;

  vector< set<int> > m_event_success;
  set<int> m_successful_orgs;
};




/*! A predicate that returns true if the appropriate number of organisms touch
 *  target cells
*/

struct cOrgMovementPred_EventNUniqueIndividualsMovedIntoTarget : public cOrgMovementPredicate {
  cOrgMovementPred_EventNUniqueIndividualsMovedIntoTarget(cDemeCellEvent* event, cPopulation& population, unsigned int unique_individuals) :
  pop(population)
  , m_satisfied(false)
  , m_stats_updated(false)
  , m_event(event)
  , m_total_individuals(unique_individuals) {
    unique_org_ids.clear();
  }
  
  virtual bool operator()(cOrganism& org) {
    int deme_id = org.GetOrgInterface().GetDemeID();
    
    if(deme_id != m_event->GetDeme()->GetDemeID() || m_event->IsDead()) {
      return false;
    }

    if(m_event->IsActive()) {
      int absolute_cell_ID = org.GetCellID();
      int relative_cell_ID = pop.GetDeme(deme_id).GetRelativeCellID(absolute_cell_ID);  
      
      // is the organism in an event cell
      if(m_event->OnEventCell(relative_cell_ID)) {
        unique_org_ids.insert(org.GetID());

        // have enough unique organisms visited the event cells?
        if(unique_org_ids.size() >= m_total_individuals) {
          m_satisfied = true;
        }
      }
    }
    return m_satisfied;
  }
  
  virtual void Print(std::ostream& out) {
    if(m_event->IsDead()) {
      return;
    }
    // WHAT TO PRINT
  }
  
  virtual void Reset() {
    m_satisfied = false;
    m_stats_updated = false;
    unique_org_ids.clear();
  }
  
  virtual bool PreviouslySatisfied() {
    return m_satisfied;
  }
  
  virtual cString GetName() {
    return "EventNUniqueIndividualsMovedIntoTarget";
  }
  
  virtual void UpdateStats(cStats& stats) {
    if(m_satisfied && !m_stats_updated) {
      m_stats_updated = true;
    }
  }
  
  virtual cDemeCellEvent* GetEvent(int i) {
    return m_event;
  }
  
  cPopulation& pop;
  bool m_satisfied;
  bool m_stats_updated;
  cDemeCellEvent* m_event;
  unsigned int m_total_individuals;
  set<int> unique_org_ids;
};

#endif
