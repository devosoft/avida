////////////////////////////////
// Generated header: Typelist.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Typelist.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Typelist.h"
#	elif (__MWERKS__)
#		include "Reference/Typelist.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Typelist.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Typelist.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Typelist.h"
#	else
#		include "Reference/Typelist.h"
#	endif
#endif
