////////////////////////////////
// Generated header: TypeInfo.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/TypeInfo.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/TypeInfo.h"
#	elif (__MWERKS__)
#		include "Reference/TypeInfo.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/TypeInfo.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/TypeInfo.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/TypeInfo.h"
#	else
#		include "Reference/TypeInfo.h"
#	endif
#endif
