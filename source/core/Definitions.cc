/*
 *  core/Definitions.cc
 *  avida-core
 *
 *  Created by David on 4/17/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#include "avida/core/Definitions.h"

namespace Avida {
  
  const char* const BioUnitSourceMap[] = {
    "deme:compete", // SRC_DEME_COMPETE
    "deme:copy", // SRC_DEME_COPY
    "deme:germline", // SRC_DEME_GERMLINE
    "deme:random", // SRC_DEME_RANDOM
    "deme:replicate", // SRC_DEME_REPLICATE
    "deme:spawn", // SRC_DEME_SPAWN
    "org:compete", // SRC_ORGANISM_COMPETE
    "org:divide", // SRC_ORGANISM_DIVIDE
    "org:file_load", // SRC_ORGANISM_FILE_LOAD
    "org:random", // SRC_ORGANISM_RANDOM
    "para:file_load", // SRC_PARASITE_FILE_LOAD
    "para:inject", // SRC_PARASITE_INJECT
    "testcpu", // SRC_TEST_CPU
  };
};
