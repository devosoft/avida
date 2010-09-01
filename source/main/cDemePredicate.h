/*
 *  cDemePredicate.h
 *  Avida
 *
 *  Created by Benjamin Beckmann on 5/18/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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
#ifndef cDemePredicate_h
#define cDemePredicate_h

#include <iostream>
#include <functional>
//#include <set>


#ifndef cResourceCount_h
#include "cResourceCount.h"
#endif
#ifndef cStats_h
#include "cStats.h"
#endif

/*! \brief An STL-compatible predicate on movement.  The intent here is to
 provide a straightforward way to track arbitrary movement *wherever* they appear
 in the population.  */
class cDemePredicate : public std::unary_function<void*, bool> {
	public:
		virtual ~cDemePredicate() { }
		virtual bool operator()(void* arg=NULL) = 0;
		virtual void Print(std::ostream& out) = 0;
		virtual void Reset() = 0;
		virtual bool PreviouslySatisfied() = 0;
		virtual cString GetName() = 0;
		virtual void UpdateStats(cStats& stats) = 0;
};

class cDemeResourceThresholdPredicate : public cDemePredicate {
private:
	cString demeResourceName;
	cString compareOperator;
	double resourceThresholdValue;
	bool previouslySatisfied;
	
public:
	cDemeResourceThresholdPredicate(cString resourceName, cString comparisonOperator, double threasholdValue) :
		demeResourceName(resourceName),
		compareOperator(comparisonOperator),
		resourceThresholdValue(threasholdValue),
		previouslySatisfied(false)
		{;}
	
	bool operator()(void* arg) {
		assert(arg != NULL);
		double resourceLevel = static_cast<cResourceCount*>(arg)->Get(static_cast<cResourceCount*>(arg)->GetResourceCountID(demeResourceName));

		if(compareOperator == ">=") {
			if(resourceLevel >= resourceThresholdValue) {
				previouslySatisfied = true;
			}
		} else if(compareOperator == "<=") {
			if(resourceLevel <= resourceThresholdValue) {
				previouslySatisfied = true;
			}
		} else {
			std::cout << "Undefined operator " << compareOperator << ".  Exit!\n";
			exit(-1);
		}
		
		return true;
	}
	
	void Print(std::ostream& out) {;}
	void Reset() { previouslySatisfied = false; }
	bool PreviouslySatisfied() { return previouslySatisfied; }
	cString GetName() { return cString("cDemeResourceThreshold"); }
	void UpdateStats(cStats& stats) {
		cString name = demeResourceName + " " + compareOperator + cStringUtil::Stringf(" %f", resourceThresholdValue);
		stats.IncDemeResourceThresholdPredicate(name);
	}
};

#endif
