//////////////////////////////////
// Generated header: TypeTraits.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
//////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/TypeTraits.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/TypeTraits.h"
#	elif (__MWERKS__)
#		include "Reference/TypeTraits.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/TypeTraits.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/TypeTraits.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/TypeTraits.h"
#	else
#		include "Reference/TypeTraits.h"
#	endif
#endif
