////////////////////////////////////
// Generated header: static_check.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
////////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/static_check.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/static_check.h"
#	elif (__MWERKS__)
#		include "Reference/static_check.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/static_check.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/static_check.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/static_check.h"
#	else
#		include "Reference/static_check.h"
#	endif
#endif
