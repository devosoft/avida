///////////////////////////////
// Generated header: Functor.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Functor.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Functor.h"
#	elif (__MWERKS__)
#		include "Reference/Functor.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Functor.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Functor.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Functor.h"
#	else
#		include "Reference/Functor.h"
#	endif
#endif
