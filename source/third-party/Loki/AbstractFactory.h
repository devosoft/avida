///////////////////////////////////////
// Generated header: AbstractFactory.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/AbstractFactory.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/AbstractFactory.h"
#	elif (__MWERKS__)
#		include "Reference/AbstractFactory.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/AbstractFactory.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/AbstractFactory.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/AbstractFactory.h"
#	else
#		include "Reference/AbstractFactory.h"
#	endif
#endif
