/*
 *  defs.h
 *  Avida
 *
 *  Called "defs.hh" prior to 12/7/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef defs_h
#define defs_h

#define VERSION "2.9.0"
#define VERSION_TAG "Jedi Master"


// -= Configurable Features
#ifndef BREAKPOINTS
#define BREAKPOINTS 0
#endif
#ifndef INSTRUCTION_COSTS
#define INSTRUCTION_COSTS 1
#endif
#ifndef INSTRUCTION_COUNT
#define INSTRUCTION_COUNT 1
#endif
#ifndef FATAL_ERRORS
#define FATAL_ERRORS 0
#endif
#ifndef FATAL_WARNINGS
#define FATAL_WARNINGS 0
#endif

// #define SPECIES_TEST


// Finally, if we have turned off Assertions, define NDEBUG
#ifndef DEBUG
# ifndef NDEBUG
#  define NDEBUG
# endif
#endif

#ifndef NULL
# define NULL 0
#endif

const int MIN_CREATURE_SIZE = 8;
const int MAX_CREATURE_SIZE = 2048;

const int MIN_INJECT_SIZE = 8;

// Number of distinct input and outputs stored in the IOBufs (to test tasks)
const int INPUT_SIZE_DEFAULT = 3;
const int OUTPUT_SIZE_DEFAULT = 1;
#define RECEIVED_MESSAGES_SIZE 10
const int MARKET_SIZE = 10000;


enum eFileType
{
  FILE_TYPE_TEXT,
  FILE_TYPE_HTML
};

enum eHARDWARE_TYPE
{
	HARDWARE_TYPE_CPU_ORIGINAL = 0,
	HARDWARE_TYPE_CPU_SMT,
	HARDWARE_TYPE_CPU_TRANSSMT,
	HARDWARE_TYPE_CPU_EXPERIMENTAL,
	HARDWARE_TYPE_CPU_GX
};

enum eTIMESLICE
{
  SLICE_CONSTANT = 0,
  SLICE_PROB_MERIT,
  SLICE_INTEGRATED_MERIT,
  SLICE_DEME_PROB_MERIT,
  SLICE_PROB_DEMESIZE_PROB_MERIT,
  SLICE_CONSTANT_BURST,
};

enum ePOSITION_CHILD
{
  POSITION_CHILD_RANDOM = 0,
  POSITION_CHILD_AGE,
  POSITION_CHILD_MERIT,
  POSITION_CHILD_EMPTY,
  POSITION_CHILD_FULL_SOUP_RANDOM,
  POSITION_CHILD_FULL_SOUP_ELDEST,
  POSITION_CHILD_DEME_RANDOM,
  POSITION_CHILD_PARENT_FACING,
  POSITION_CHILD_NEXT_CELL,
  POSITION_CHILD_FULL_SOUP_ENERGY_USED,
  POSITION_CHILD_NEIGHBORHOOD_ENERGY_USED
};
const int NUM_LOCAL_POSITION_CHILD = POSITION_CHILD_FULL_SOUP_RANDOM;

enum eDEATH_METHOD
{
  DEATH_METHOD_OFF = 0,
  DEATH_METHOD_CONST,
  DEATH_METHOD_MULTIPLE
};

enum eALLOC_METHOD
{
  ALLOC_METHOD_DEFAULT = 0,
  ALLOC_METHOD_NECRO,
  ALLOC_METHOD_RANDOM
};

enum eDIVIDE_METHOD
{
  DIVIDE_METHOD_OFFSPRING = 0,
  DIVIDE_METHOD_SPLIT,
  DIVIDE_METHOD_BIRTH,
};

enum eEPIGENETIC_METHOD
{
  EPIGENETIC_METHOD_NONE = 0,
  EPIGENETIC_METHOD_OFFSPRING,
  EPIGENETIC_METHOD_PARENT,
  EPIGENETIC_METHOD_BOTH
};

enum eINJECT_METHOD
{
  INJECT_METHOD_OFFSPRING = 0,
  INJECT_METHOD_SPLIT
};

enum eGENERATION_INCREMENT
{
  GENERATION_INC_OFFSPRING = 0,
  GENERATION_INC_BOTH
};

enum eBASE_MERIT
{
  BASE_MERIT_CONST = 0,
  BASE_MERIT_COPIED_SIZE,
  BASE_MERIT_EXE_SIZE,
  BASE_MERIT_FULL_SIZE,
  BASE_MERIT_LEAST_SIZE,
  BASE_MERIT_SQRT_LEAST_SIZE,
  BASE_MERIT_NUM_BONUS_INST
};

enum eINST_CODE_DEFAULT
{
  INST_CODE_ZEROS = 0,
  INST_CODE_INSTNUM
};

enum eVerbosity {
  VERBOSE_SILENT = 0,   // No output at all
  VERBOSE_NORMAL,       // Notification at start of commands.
  VERBOSE_ON,           // Verbose output, detailing progress
  VERBOSE_DETAILS,      // High level of details, as available.
  VERBOSE_DEBUG         // Print Debug Information, as applicable.
};


#endif
