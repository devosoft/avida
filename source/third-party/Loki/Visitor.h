///////////////////////////////
// Generated header: Visitor.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Visitor.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Visitor.h"
#	elif (__MWERKS__)
#		include "Reference/Visitor.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Visitor.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Visitor.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Visitor.h"
#	else
#		include "Reference/Visitor.h"
#	endif
#endif
