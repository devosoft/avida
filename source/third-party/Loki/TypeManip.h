/////////////////////////////////
// Generated header: TypeManip.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
/////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/TypeManip.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/TypeManip.h"
#	elif (__MWERKS__)
#		include "Reference/TypeManip.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/TypeManip.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/TypeManip.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/TypeManip.h"
#	else
#		include "Reference/TypeManip.h"
#	endif
#endif
