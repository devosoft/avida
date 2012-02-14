/*
 *  cDemePredicate.h
 *  Avida
 *
 *  Created by Benjamin Beckmann on 5/18/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef cDemePredicate_h
#define cDemePredicate_h

#include <iostream>
#include <functional>

#include "cResourceCount.h"
#include "cStats.h"

class cAvidaContext;


/*! \brief An STL-compatible predicate on movement.  The intent here is to
 provide a straightforward way to track arbitrary movement *wherever* they appear
 in the population.  */
class cDemePredicate : public std::binary_function<cAvidaContext&, void*, bool>
{
	public:
		virtual ~cDemePredicate() { }
		virtual bool operator()(cAvidaContext& ctx, void* arg = NULL) = 0;
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
		{ ; }
	
	bool operator()(cAvidaContext& ctx, void* arg) {
		assert(arg != NULL);
		double resourceLevel = static_cast<cResourceCount*>(arg)->Get(ctx, static_cast<cResourceCount*>(arg)->GetResourceCountID(demeResourceName));

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
	
	void Print(std::ostream&) { ; }
	void Reset() { previouslySatisfied = false; }
	bool PreviouslySatisfied() { return previouslySatisfied; }
	cString GetName() { return cString("cDemeResourceThreshold"); }
	void UpdateStats(cStats&) { ; }
};

#endif
