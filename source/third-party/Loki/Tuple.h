/////////////////////////////
// Generated header: Tuple.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
/////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Tuple.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Tuple.h"
#	elif (__MWERKS__)
#		include "Reference/Tuple.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Tuple.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Tuple.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Tuple.h"
#	else
#		include "Reference/Tuple.h"
#	endif
#endif
