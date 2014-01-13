/*
 *  cGermline.h
 *  Avida
 *
 *  Copyright 2007-2011 Michigan State University. All rights reserved.
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

#ifndef cGermline_h
#define cGermline_h

#include <vector>

#include "avida/core/Genome.h"

using namespace Avida;


class cGermline
{
protected:
	std::vector<Genome> m_germline;

  cGermline(const cGermline&); // @not_implmented
  
  
public:
  cGermline() { ; }
    
	Genome& GetLatest() { assert(m_germline.size() > 0); return m_germline.back(); }
	void Add(const Genome& genome) { m_germline.push_back(genome); }
	unsigned int Size() const { return m_germline.size(); }
};

#endif
