///////////////////////////////
// Generated header: Factory.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Factory.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Factory.h"
#	elif (__MWERKS__)
#		include "Reference/Factory.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Factory.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Factory.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Factory.h"
#	else
#		include "Reference/Factory.h"
#	endif
#endif
