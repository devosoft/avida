# - Find the native FLTK includes and library
# The following settings are defined
#  FLTK_FLUID_EXECUTABLE, where to find the Fluid tool
#  FLTK_WRAP_UI, This enables the FLTK_WRAP_UI command
#  FLTK_INCLUDE_DIR, where to find include files
#  FLTK_LIBRARIES, list of fltk libraries
#  FLTK_FOUND, Don't use FLTK if false.
# The following settings should not be used in general.
#  FLTK_BASE_LIBRARY   = the full path to fltk.lib
#  FLTK_GL_LIBRARY     = the full path to fltk_gl.lib
#  FLTK_FORMS_LIBRARY  = the full path to fltk_forms.lib
#  FLTK_IMAGES_LIBRARY = the full path to fltk_images.lib

#  Platform dependent libraries required by FLTK
IF(WIN32)
  IF(NOT CYGWIN)
    IF(BORLAND)
      SET( FLTK_PLATFORM_DEPENDENT_LIBS import32 )
    ELSE(BORLAND)
      SET( FLTK_PLATFORM_DEPENDENT_LIBS wsock32 comctl32 )
    ENDIF(BORLAND)
  ENDIF(NOT CYGWIN)
ENDIF(WIN32)

IF(UNIX)
  INCLUDE(${CMAKE_ROOT}/Modules/FindX11.cmake)
  SET( FLTK_PLATFORM_DEPENDENT_LIBS ${X11_LIBRARIES} -lm)
ENDIF(UNIX)

IF(APPLE)
  SET( FLTK_PLATFORM_DEPENDENT_LIBS  "-framework Carbon -framework Cocoa -framework ApplicationServices -lz")
ENDIF(APPLE)

IF(CYGWIN)
  SET( FLTK_PLATFORM_DEPENDENT_LIBS ole32 uuid comctl32 wsock32 supc++ -lm -lgdi32)
ENDIF(CYGWIN)


FIND_PATH(FLTK_INCLUDE_PATH FL
  # Look for an environment variable FLTK_DIR.
  $ENV{FLTK_DIR}
  
  # Look in places relative to the system executable search path.
  ${FLTK_DIR_SEARCH}
  
  # Look in standard UNIX install locations.
  /usr/local/include
  /usr/include
  /usr/local/fltk
  /usr/X11R6/include
  
  # Help the user find it if we cannot.
  DOC "The ${FLTK_DIR_STRING}"
  )

FIND_LIBRARY(FLTK_BASE_LIBRARY
  NAMES fltk
  PATHS /usr/local/lib /usr/lib /lib
  )
FIND_LIBRARY(FLTK_IMAGE_LIBRARY
  NAMES fltk_images
  PATHS /usr/local/lib /usr/lib /lib
  )
FIND_LIBRARY(FLTK_JPEG_LIBRARY
  NAMES fltk_jpeg
  PATHS /usr/local/lib /usr/lib /lib
  )
FIND_LIBRARY(XEXT_LIBRARY
  NAMES Xext
  PATHS /usr/local/lib /usr/lib /lib /usr/X11R6/lib
  )
