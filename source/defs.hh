//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef DEFS_HH
#define DEFS_HH

#define DEFAULT_DIR "../work/"

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
//#define static_cast(type,expr)      (type)(expr)
//#define const_cast(type,expr)       (type)(expr)
//#define reinterpret_cast(type,expr) (type)(expr)
//#define LONGINT int

// snazzy compiler //
#define static_cast(type,expr)      static_cast<type>(expr)
#define const_cast(type,expr)       const_cast<type>(expr)
#define reinterpret_cast(type,expr) reinterpret_cast<type>(expr)
#define LONGINT long int


////// Don't change anything below this line. /////
// the macro VERSION is defined by automake (in the file 'configure.in' in
// the top-level directory).
#ifndef VERSION
 #define VERSION "2.3.1"
#endif
#define AVIDA_VERSION "2.3.1 (Padawan)"

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

#endif
