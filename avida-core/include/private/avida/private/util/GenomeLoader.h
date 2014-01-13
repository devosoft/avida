/*
 *  private/util/GenomeLoader.h
 *  Avida
 *
 *  Created by David on 12/22/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#ifndef AvidaUtilGenomeLoader_h
#define AvidaUtilGenomeLoader_h

#include "avida/core/Types.h"

class cHardwareManager;
class cString;


namespace Avida {
  namespace Util {
    GenomePtr LoadGenomeDetailFile(const cString& filename, const cString& working_dir, cHardwareManager& hwm, Feedback& feedback, Apto::String instset = "");
  };
};

#endif
