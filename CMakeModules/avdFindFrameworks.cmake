
#
# On Apple platforms this module provides a macro AVD_FIND_FRAMEWORKS, adapted from
# ${CMAKE_ROOT}/Modules/CMakeFindFrameworks.cmake, searches first on the given paths <search_paths>, and then along standard
# paths, for frameworks of the given name <fwk>, and sets <fwk>_FRAMEWORKS to a list of frameworks found.
#
# On non-Apple platforms, AVD_FIND_FRAMEWORKS does nothing (except set <fwk>_FRAMEWORKS to empty).
#

MACRO(AVD_FIND_FRAMEWORKS fwk search_paths)
  SET(${fwk}_FRAMEWORKS)
  SET(${fwk}_FRAMEWORK_PATHS)
  IF(APPLE)
    SET(STD_APPLE_FRAMEWORK_SEARCH_PATHS
      ~/Library/Frameworks
      /Library/Frameworks
      /System/Library/Frameworks
      /Network/Library/Frameworks
    )
    FOREACH(dir ${search_paths} ${STD_APPLE_FRAMEWORK_SEARCH_PATHS})
      IF(EXISTS ${dir}/${fwk}.framework)
        SET(${fwk}_FRAMEWORKS ${${fwk}_FRAMEWORKS} ${dir}/${fwk}.framework)
        SET(${fwk}_FRAMEWORK_PATHS ${${fwk}_FRAMEWORK_PATHS} ${dir})
      ENDIF(EXISTS ${dir}/${fwk}.framework)
    ENDFOREACH(dir)
  ENDIF(APPLE)
ENDMACRO(AVD_FIND_FRAMEWORKS)
