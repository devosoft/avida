///////////////////////////////////
// Generated header: Singleton.cpp
// Forwards to the appropriate code
// that works on the detected compiler
// Generated on Sun Sep 15 15:31:17 2002
///////////////////////////////////

#ifdef LOKI_USE_REFERENCE
#	include "Reference/Singleton.cpp"
#else
#	if (__INTEL_COMPILER)
#		include "Reference/Singleton.cpp"
#	elif (__MWERKS__)
#		include "Reference/Singleton.cpp"
#	elif (__BORLANDC__ >= 0x560)
#		include "Borland/Singleton.cpp"
#	elif (_MSC_VER >= 1300)
#		include "MSVC/1300/Singleton.cpp"
#	elif (_MSC_VER >= 1200)
#		include "MSVC/1200/Singleton.cpp"
#	else
#		include "Reference/Singleton.cpp"
#	endif
#endif
