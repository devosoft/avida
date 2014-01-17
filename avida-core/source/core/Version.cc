/*
 *  core/Version.cc
 *  avida-core
 *
 *  Created by David on 4/17/11.
 *  Copyright 2011-2014 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/core/Version.h"

#include "apto/core/StringUtils.h"

Apto::String Avida::Version::Banner()
{
  Apto::String banner("Avida ");
  banner += Avida::Version::String();
  banner += "\n";
  banner += "--------------------------------------------------------------------------------\n";
  banner += "by Charles Ofria\n";
  
  banner += "Lead Developer: David M. Bryson\n\n";
  
  banner += "Active developers include:\n";
  banner += "Aaron P. Wagner and Anya E. Johnson\n";
  
  banner += "For a more complete list of contributors, see the AUTHORS file.\n\n";
  
  banner += "Copyright 1999-2014 Michigan State University.\n";
  banner += "Copyright 1993-2003 California Institute of Technology.\n\n";
  
  banner += "Avida comes with ABSOLUTELY NO WARRANTY.\n";
  banner += "This is free software, and you are welcome to redistribute it under certain\n";
  banner += "conditions. See file COPYING for details.\n\n";
  
  banner += "For more information, see: http://avida.devosoft.org/\n";
  banner += "--------------------------------------------------------------------------------\n";
  
  return banner;
}


bool Avida::Version::CheckCompatibility(const char* version)
{
  Apto::String lv(version);
  
  Apto::String major = lv.Pop('.');
  Apto::String minor = lv.Pop('.');
  
  if (!major.GetSize() || Avida::Version::Major() != (int)Apto::StrAs(major)) return false;
  if (!minor.GetSize() || Avida::Version::Minor() != (int)Apto::StrAs(minor)) return false;
  
  return true;
}
