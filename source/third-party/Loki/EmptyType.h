/////////////////////////////////
// Generated header: EmptyType.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
/////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/EmptyType.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/EmptyType.h"
#	elif (__MWERKS__)
#		include "Reference/EmptyType.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/EmptyType.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/EmptyType.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/EmptyType.h"
#	else
#		include "Reference/EmptyType.h"
#	endif
#endif
