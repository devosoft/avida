///////////////////////////////////
// Generated header: AssocVector.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/AssocVector.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/AssocVector.h"
#	elif (__MWERKS__)
#		include "Reference/AssocVector.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/AssocVector.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/AssocVector.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/AssocVector.h"
#	else
#		include "Reference/AssocVector.h"
#	endif
#endif
