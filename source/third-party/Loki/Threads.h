///////////////////////////////
// Generated header: Threads.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Threads.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Threads.h"
#	elif (__MWERKS__)
#		include "Reference/Threads.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Threads.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Threads.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Threads.h"
#	else
#		include "Reference/Threads.h"
#	endif
#endif
