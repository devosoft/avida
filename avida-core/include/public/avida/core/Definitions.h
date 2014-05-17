/*
 *  core/Defintions.h
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

#ifndef AvidaCoreDefinitions_h
#define AvidaCoreDefinitions_h

const int MIN_GENOME_LENGTH = 8;
const int MAX_GENOME_LENGTH = 2048;

const int MIN_INJECT_SIZE = 8;


// Number of distinct input and outputs stored in the IOBufs (to test tasks)
const int INPUT_SIZE_DEFAULT = 3;
const int OUTPUT_SIZE_DEFAULT = 1;

const int RECEIVED_MESSAGES_SIZE = 10;


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
  HARDWARE_TYPE_CPU_GP8,
  HARDWARE_TYPE_CPU_BCR,
};

enum eTIMESLICE
{
  SLICE_CONSTANT = 0,
  SLICE_PROB_MERIT,
  SLICE_INTEGRATED_MERIT,
  SLICE_DEME_PROB_MERIT,
  SLICE_PROB_DEMESIZE_PROB_MERIT,
  SLICE_PROB_INTEGRATED_MERIT,
};

enum ePOSITION_OFFSPRING
{
  POSITION_OFFSPRING_RANDOM = 0,
  POSITION_OFFSPRING_AGE,
  POSITION_OFFSPRING_MERIT,
  POSITION_OFFSPRING_EMPTY,
  POSITION_OFFSPRING_FULL_SOUP_RANDOM,
  POSITION_OFFSPRING_FULL_SOUP_ELDEST,
  POSITION_OFFSPRING_DEME_RANDOM,
  POSITION_OFFSPRING_PARENT_FACING,
  POSITION_OFFSPRING_NEXT_CELL,
  POSITION_OFFSPRING_FULL_SOUP_ENERGY_USED,
  POSITION_OFFSPRING_NEIGHBORHOOD_ENERGY_USED,
  POSITION_OFFSPRING_DISPERSAL,
};
const int NUM_LOCAL_POSITION_OFFSPRING = POSITION_OFFSPRING_FULL_SOUP_RANDOM;

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
  BASE_MERIT_NUM_BONUS_INST,
  BASE_MERIT_GESTATION_TIME
  
};

enum eINST_CODE_DEFAULT
{
  INST_CODE_ZEROS = 0,
  INST_CODE_INSTNUM
};


enum ePHENPLAST_BONUS_METHOD{
  DEFAULT = 0,
  NO_BONUS,
  FRAC_BONUS,
  FULL_BONUS
};

enum eDEME_TRIGGERS {
  DEME_TRIGGER_ALL = 0,
  DEME_TRIGGER_FULL,                // 1
  DEME_TRIGGER_CORNERS,             // 2
  DEME_TRIGGER_AGE,                 // 3
  DEME_TRIGGER_BIRTHS,              // 4
  DEME_TRIGGER_MOVE_PREDATORS,      // 5
  DEME_TRIGGER_GROUP_KILL,          // 6
  DEME_TRIGGER_MESSAGE_PREDATORS,   // 7
  DEME_TRIGGER_PREDICATE,           // 8
  DEME_TRIGGER_PERFECT_REACTIONS,   // 9
  DEME_TRIGGER_CONSUME_RESOURCES,   // 10
  DEME_TRIGGER_UNKNOWN              // 11
};

enum eSELECTION_TYPE {
  SELECTION_TYPE_PROPORTIONAL = 0,
  SELECTION_TYPE_TOURNAMENT
};

enum eMP_SCHEDULING {
	MP_SCHEDULING_NULL = 0,
	MP_SCHEDULING_INTEGRATED
};

enum eVerbosity {
  VERBOSE_SILENT = 0,   // 0: No output at all
  VERBOSE_NORMAL,       // 1: Notification at start of commands.
  VERBOSE_ON,           // 2: Verbose output, detailing progress
  VERBOSE_DETAILS,      // 3: High level of details, as available.
  VERBOSE_DEBUG         // 4: Print Debug Information, as applicable.
};



enum eMatingTypes {
  MATING_TYPE_JUVENILE = -1,
  MATING_TYPE_FEMALE = 0,
  MATING_TYPE_MALE = 1
};

enum eMatePreferences {
  MATE_PREFERENCE_RANDOM,
  MATE_PREFERENCE_HIGHEST_DISPLAY_A,
  MATE_PREFERENCE_HIGHEST_DISPLAY_B,
  MATE_PREFERENCE_HIGHEST_MERIT,
  MATE_PREFERENCE_LOWEST_DISPLAY_A,
  MATE_PREFERENCE_LOWEST_DISPLAY_B,
  MATE_PREFERENCE_LOWEST_MERIT
};


#endif
