///////////////////////////////////////////
// Generated header: HierarchyGenerators.h
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/HierarchyGenerators.h"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/HierarchyGenerators.h"
#	elif (__MWERKS__)
#		include "Reference/HierarchyGenerators.h"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/HierarchyGenerators.h"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/HierarchyGenerators.h"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/HierarchyGenerators.h"
#	else
#		include "Reference/HierarchyGenerators.h"
#	endif
#endif
