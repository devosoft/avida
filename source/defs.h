//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef DEFS_HH
#define DEFS_HH

#define VERSION "2.3.1"
#define VERSION_TAG "Padawan"

/*
FIXME:  remove asap, switching to file-configureable option
*/
// path to html doc files
#ifndef HTMLDIR
#define HTMLDIR "../work/doc_html/"
#endif


// -= Various view modes =-
#define VIEW_NEWVIEWER
// #define VIEW_VISTA
// #define VIEW_PRIMITIVE
// #define VIEW_EXTERNAL
// #define VIEW_CURSES
// #define VIEW_NCURSES
// #define VIEW_ANSI


// -= Speedups =-     (define these to allow features to be turned on)

// #define BREAKPOINTS
// #define QUICK_BASE_TEST_CPU
// #define QUICK_HEAD_TEST_CPU
#define INSTRUCTION_COSTS
#define INSTRUCTION_COUNT

// -= Toggles =-      (define these to actually turn features on)

#define WRITE_PROTECTION
// #define FATAL_ERRORS
// #define FATAL_WARNINGS
#define SINGLE_IO_BUFFER

// Test to see if search approx equals size... value is +- tolerance
// #define TEST_SEARCH_SIZE .25

// #define DEBUG
// #define DEBUG_CHECK_OK
// #define DEBUG_MEMTRACK
// #define INTEGRITY_CHECK
// #define DEBUG_MEM_CHECK
// #define TEST


////// Compiler Compatability Stuff //////////

// old compiler //
//#define explicit
//#define LONGINT int

// snazzy compiler //
#define LONGINT long int


////// Don't change anything below this line. /////
#ifdef QUICK_BASE_TEST_CPU
#define QUICK_TEST_CPU
#endif

#ifdef QUICK_HEAD_TEST_CPU
#define QUICK_TEST_CPU
#endif

#ifdef VIEW_primitive
#define VIEW_PRIMITIVE
#endif
#ifdef VIEW_external
#define VIEW_EXTERNAL
#endif
#ifdef VIEW_curses
#define VIEW_CURSES
#endif
#ifdef VIEW_ncurses
#define VIEW_NCURSES
#endif
#ifdef VIEW_vista
#define VIEW_VISTA
#define VIEW_PRIMITIVE
#endif

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

 #ifdef UCHAR
 #undef UCHAR
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

#define MAX_INT 4294967295
#define MAX_INST_SET_SIZE 64

// #ifndef FALSE
//  #define FALSE 0
//  #define TRUE !FALSE
// #endif

#ifndef NULL
 #define NULL 0
#endif

#ifndef _WINDEF_               // Defined by WINDOWS.H
 #define UCHAR unsigned char
 #undef UINT
 #define UINT  unsigned int
#endif

#define TOOLS_DIR tools
#define CPU_DIR   cpu
#define MAIN_DIR  main
#define VIEW_DIR  viewers

#define MIN_CREATURE_SIZE 8
#define MAX_CREATURE_SIZE 2048

#define MIN_INJECT_SIZE 8

// Number of distinct input and outputs stored in the IOBufs (to test tasks)
#define INPUT_BUF_SIZE  3
#define OUTPUT_BUF_SIZE 1
#define SEND_BUF_SIZE 3
#define RECEIVE_BUF_SIZE 3

// Task Merit Method //
#define TASK_MERIT_NONE   0
#define TASK_MERIT_NORMAL 1  // Bonus just equals the task bonus

#define MERIT_TIME_AT_TASK     0
#define MERIT_TIME_AT_DIVIDE   1

#define FILE_TYPE_TEXT 0
#define FILE_TYPE_HTML 1

enum tHARDWARE_TYPE
{
	HARDWARE_TYPE_CPU_ORIGINAL = 0,
	HARDWARE_TYPE_CPU_4STACK,
	HARDWARE_TYPE_CPU_SMT
};

#define SLICE_CONSTANT         0
#define SLICE_PROB_MERIT       1
#define SLICE_INTEGRATED_MERIT 2

#define POSITION_CHILD_RANDOM           0
#define POSITION_CHILD_AGE              1
#define POSITION_CHILD_MERIT            2
#define POSITION_CHILD_EMPTY            3
#define NUM_LOCAL_POSITION_CHILD        4

#define POSITION_CHILD_FULL_SOUP_RANDOM 4
#define POSITION_CHILD_FULL_SOUP_ELDEST 5
#define POSITION_CHILD_DEME_RANDOM      6

#define DEATH_METHOD_OFF       0
#define DEATH_METHOD_CONST     1
#define DEATH_METHOD_MULTIPLE  2

#define ALLOC_METHOD_DEFAULT   0
#define ALLOC_METHOD_NECRO     1
#define ALLOC_METHOD_RANDOM    2

#define DIVIDE_METHOD_OFFSPRING 0
#define DIVIDE_METHOD_SPLIT     1
#define DIVIDE_METHOD_BIRTH     2

#define GENERATION_INC_OFFSPRING 0
#define GENERATION_INC_BOTH      1

#define TASK_MERIT_OFF         0
#define TASK_MERIT_EXPONENTIAL 1
#define TASK_MERIT_LINEAR      2

#define SIZE_MERIT_OFF         0
#define SIZE_MERIT_COPIED      1
#define SIZE_MERIT_EXECUTED    2
#define SIZE_MERIT_FULL        3
#define SIZE_MERIT_LEAST       4
#define SIZE_MERIT_SQRT_LEAST  5


#endif
