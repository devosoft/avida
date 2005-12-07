#ifndef WIN32_MKDIR_HACK_HH
#define WIN32_MKDIR_HACK_HH

/*
FIXME:  mkdir undefined in win32, need a cleaner workaround.
any prefers where to put this?
-- kgn
*/
#ifdef WIN32
# include <direct.h>
# ifndef ACCESSPERMS
#  define ACCESSPERMS 0
# endif
# ifndef mkdir
#  define mkdir(path, ignored_mode) _mkdir(path)
# endif
# ifndef mode_t
#  define mode_t unsigned int
# endif
#endif


#endif
