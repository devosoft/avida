/////////////////////////////////
// Generated header: Singleton.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
/////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Singleton.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Singleton.h"
#	elif (__MWERKS__)
#		include "Reference/Singleton.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Singleton.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Singleton.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Singleton.h"
#	else
#		include "Reference/Singleton.h"
#	endif
#endif
