/*
 *  defs.h
 *  Avida
 *
 *  Called "defs.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef defs_h
#define defs_h

#define VERSION "2.7.0"
#define VERSION_TAG "Jedi Knight"

// -= Various view modes
// #define VIEW_ANSI


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
#ifndef CLASSIC_FULLY_ASSOCIATIVE
#define CLASSIC_FULLY_ASSOCIATIVE 0
#endif
#ifndef SMT_FULLY_ASSOCIATIVE
#define SMT_FULLY_ASSOCIATIVE 1
#endif
#ifndef FATAL_ERRORS
#define FATAL_ERRORS 0
#endif
#ifndef FATAL_WARNINGS
#define FATAL_WARNINGS 0
#endif

// #define SPECIES_TEST


// -= Compiler Compatability Stuff
#define LONGINT long int


////// Don't change anything below this line. /////

#ifdef VIEW_ANSI
#define PLATFORM_WINDOWS     // enable to compile for Win32 console
#define MSVC_COMPILER        // enable to compile with Microsoft VC++
#endif

// Finally, if we have turned off Assertions, define NDEBUG
#ifndef DEBUG
#ifndef NDEBUG
#define NDEBUG
#endif
#endif

#ifdef PLATFORM_WINDOWS
 #ifdef UINT // Some modules will not compile w/out this (e.g. b_struct2)
 #undef UNIT
 #endif

 #include <windows.h>	// Defines, prototypes, etc. for
#endif

#ifdef MSVC_COMPILER
 #ifdef  _DEBUG         // VC++ debug define
 #define DEBUG
 #endif

 #ifndef _WINDEF_
 #include <string.h>
 #endif
#endif

#ifndef NULL
 #define NULL 0
#endif

const int MIN_CREATURE_SIZE = 8;
const int MAX_CREATURE_SIZE = 2048;

const int MIN_INJECT_SIZE = 8;

// Number of distinct input and outputs stored in the IOBufs (to test tasks)
const int INPUT_SIZE_DEFAULT = 3;
const int OUTPUT_SIZE_DEFAULT = 1;
#define RECEIVED_MESSAGES_SIZE 10
const int MARKET_SIZE = 10000;


enum tFileType
{
  FILE_TYPE_TEXT,
  FILE_TYPE_HTML
};

enum tHARDWARE_TYPE
{
	HARDWARE_TYPE_CPU_ORIGINAL = 0,
	HARDWARE_TYPE_CPU_SMT,
	HARDWARE_TYPE_CPU_TRANSSMT
};

enum tTIMESLICE
{
  SLICE_CONSTANT = 0,
  SLICE_PROB_MERIT,
  SLICE_INTEGRATED_MERIT
};

enum tPOSITION_CHILD
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
  POSITION_CHILD_FULL_SOUP_TIME_USED
};
const int NUM_LOCAL_POSITION_CHILD = POSITION_CHILD_FULL_SOUP_RANDOM;

enum tDEATH_METHOD
{
  DEATH_METHOD_OFF = 0,
  DEATH_METHOD_CONST,
  DEATH_METHOD_MULTIPLE
};

enum tALLOC_METHOD
{
  ALLOC_METHOD_DEFAULT = 0,
  ALLOC_METHOD_NECRO,
  ALLOC_METHOD_RANDOM
};

enum tDIVIDE_METHOD
{
  DIVIDE_METHOD_OFFSPRING = 0,
  DIVIDE_METHOD_SPLIT,
  DIVIDE_METHOD_BIRTH
};

enum tGENERATION_INCREMENT
{
  GENERATION_INC_OFFSPRING = 0,
  GENERATION_INC_BOTH
};

enum tBASE_MERIT
{
  BASE_MERIT_CONST = 0,
  BASE_MERIT_COPIED_SIZE,
  BASE_MERIT_EXE_SIZE,
  BASE_MERIT_FULL_SIZE,
  BASE_MERIT_LEAST_SIZE,
  BASE_MERIT_SQRT_LEAST_SIZE
};

typedef enum eVerbosity {
  VERBOSE_SILENT = 0,   // No output at all
  VERBOSE_NORMAL,       // Notification at start of commands.
  VERBOSE_ON,           // Verbose output, detailing progress
  VERBOSE_DETAILS,      // High level of details, as available.
  VERBOSE_DEBUG         // Print Debug Information, as applicable.
};


#endif
