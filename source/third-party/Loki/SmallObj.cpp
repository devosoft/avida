//////////////////////////////////
// Generated header: SmallObj.cpp
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
//////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/SmallObj.cpp"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/SmallObj.cpp"
#	elif (__MWERKS__)
#		include "Reference/SmallObj.cpp"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/SmallObj.cpp"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/SmallObj.cpp"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/SmallObj.cpp"
#	else
#		include "Reference/SmallObj.cpp"
#	endif
#endif
