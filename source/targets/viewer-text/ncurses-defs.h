#ifndef ncurses_defs_h
#define ncurses_defs_h

#ifdef HAVE_NCURSES_H
#define NCURSES_OPAQUE 0
#include <ncurses.h>
#undef OK
#else
#define NCURSES_OPAQUE 0
#include <curses.h>
#undef OK
#endif

#ifndef platform_h
#include "Platform.h"
#endif

// All colors are defined except black on black:
#define COLOR_OFF    8

// Define special characters

#define CHAR_TTEE     ACS_TTEE
#define CHAR_BTEE     ACS_BTEE
#define CHAR_LTEE     ACS_LTEE
#define CHAR_RTEE     ACS_RTEE
#define CHAR_PLUS     ACS_PLUS
#define CHAR_HLINE    ACS_HLINE
#define CHAR_VLINE    ACS_VLINE
#define CHAR_ULCORNER ACS_ULCORNER
#define CHAR_URCORNER ACS_URCORNER
#define CHAR_LLCORNER ACS_LLCORNER
#define CHAR_LRCORNER ACS_LRCORNER
#define CHAR_BULLET   '#'   // ACS_BULLET


#endif
