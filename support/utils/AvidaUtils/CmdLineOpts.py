##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

import SCons
import os
import sys
import shutil

def Update(opts, env):
  opts.Add(
    SCons.Options.EnumOption(
      'buildType',
    """Selects a set of compiler options:
    - Debug: many compiler warnings; debugging code enabled; no optimization.
    - ReleaseDebug: all options from Debug set, with some speed optimization.
    - Release: few warnings; debugging code disabled; speed optimization.
    - MinimumRelease: few warnings; debugging code disabled; size optimization.
    - None: no new compiler options are added to the system defaults.
   """,
      'Release',
      allowed_values = (
        'Debug',
        'ReleaseDebug',
        'Release',
        'MinimumRelease',
        'None',
      )
  ) )
  opts.Add(
    SCons.Options.BoolOption(
      'enablePrimitive',
      """Set to "1" to enable building interfaceless Avida (fastest version).""",
      1,
  ) )
  opts.Add(
    SCons.Options.BoolOption(
      'enableTCMalloc',
      """Set to "1" to enable TCMalloc""",
       '$plat_default_enableTCMalloc',
  ) )
  opts.Add(
    SCons.Options.BoolOption(
      'enableGuiNcurses',
      """Set to "1" to enable building Avida console interface.""",
      0,
  ) )
  opts.Add(
    SCons.Options.BoolOption(
      'enableSerialization',
      """Set to "1" to build and run test code.""",
      '$plat_default_enableSerialization',
  ) )
  opts.Add(
    SCons.Options.BoolOption(
      'enableMemTracking',
      """Set to "1" to track creation and deletion of some c++ objects.""",
      '$plat_default_enableMemTracking',
  ) )
  opts.Add(
    SCons.Options.BoolOption(
      'enablePyPkg',
    """Set to "1" to wrap c++ interfaces for use in Python scripts.""",
      0
#      '$plat_default_enablePyPkg',
  ) )

  #opts.Add(
  #  SCons.Options.BoolOption(
  #    'enableSharedPtr',
  #  """Set to "1" to use tPtr in shared mode (currently required for
  #  Python wrappers, and for saving and reloading runs; NOT required to
  #  use Avida in primitive mode, in which case Avida will run faster).""",
  #    '$plat_default_enableSharedPtr',
  #) )

  opts.Add(
    'extrasDir',
    'Path to the avida-extras subdirectory.',
    '$plat_default_extrasDir',
  )

  # Store option names and values in the construction env so that any
  # user overrides from the command line can be retrieved.
  #
  opts.Update(env)

  # Set build flags per-platform using user-selected buildType.
  #
  if env.subst('$buildType') is not 'None':
    build_type_options = {
      'Debug':{
        'CXXFLAGS':"$_PLATFORM_DEBUG_BUILD_FLAGS",
        'SHCXXFLAGS':"$_PLATFORM_DEBUG_BUILD_FLAGS",
        'CCFLAGS':"$_PLATFORM_DEBUG_BUILD_FLAGS",
        'SHCCFLAGS':"$_PLATFORM_DEBUG_BUILD_FLAGS",
      },
      'ReleaseDebug':{
        'CXXFLAGS':"$_PLATFORM_RELEASE_DEBUG_BUILD_FLAGS",
        'SHCXXFLAGS':"$_PLATFORM_RELEASE_DEBUG_BUILD_FLAGS",
        'CCFLAGS':"$_PLATFORM_RELEASE_DEBUG_BUILD_FLAGS",
        'SHCCFLAGS':"$_PLATFORM_RELEASE_DEBUG_BUILD_FLAGS",
      },
      'Release':{
        'CXXFLAGS':"$_PLATFORM_RELEASE_BUILD_FLAGS",
        'SHCXXFLAGS':"$_PLATFORM_RELEASE_BUILD_FLAGS",
        'CCFLAGS':"$_PLATFORM_RELEASE_BUILD_FLAGS",
        'SHCCFLAGS':"$_PLATFORM_RELEASE_BUILD_FLAGS",
      },
      'MinimumRelease':{
        'CXXFLAGS':"$_PLATFORM_MINIMUM_RELEASE_BUILD_FLAGS",
        'SHCXXFLAGS':"$_PLATFORM_MINIMUM_RELEASE_BUILD_FLAGS",
        'CCFLAGS':"$_PLATFORM_MINIMUM_RELEASE_BUILD_FLAGS",
        'SHCCFLAGS':"$_PLATFORM_MINIMUM_RELEASE_BUILD_FLAGS",
      },
    }
    build_type = env.subst('$buildType')
    if build_type_options.has_key(build_type):
      for item in build_type_options[build_type].items():
        env[item[0]] = item[1]


  # Make a default build directory name based on values of options 'buildType'
  # and 'enableMemTracking'.
  #
  default_build_dir = 'build'
  if env.subst('$buildType') is not 'None':
    default_build_dir += '-' + env.subst('$buildType')
  if env.subst('$enableMemTracking') in ['yes', '1']:
    default_build_dir += '-MemTrack'
  if env.subst('$enableSerialization') in ['yes', '1']:
    default_build_dir += '-Ser'
  # XXX reenable when we have code using its value.
  #if env.subst('$enablePyPkg') in ['yes', '1']:
  #  default_build_dir += '-Py'
  #elif env.subst('$enableSharedPtr') in ['yes', '1']:
  #  default_build_dir += '-ShPtr'

  extras_build_dir = None
  if env.subst('$extrasDir') not in ['None', 'none', '']:
    extras_build_dir = os.path.join(env.subst('$extrasDir'), default_build_dir)
    opts.Add(
      'extrasBuildDir',
      """Where to place derived build files for avida-extras.
      - Default is derived from build options.
      """,
      extras_build_dir
    )

  opts.Add(
    'buildDir',
    """Where to place derived build files.
    - Default is derived from build options.
    """,
    default_build_dir
  )
  
  opts.Add(
    'execPrefix',
    """Installation prefix for executables.
    - #/$$buildDir/work by default.""",
    os.path.join('#/${buildDir}', 'work')
  )

  opts.Add(
    'configPrefix',
    """Installation prefix for configuration files.
    - $$execPrefix by default.""",
    '$execPrefix'
  )

  #opts.Add(
  #  SCons.Options.BoolOption(
  #    'runConfTests',
  #  """
  #  Set to '1' to have build system try to verify presence of required
  #  libraries and header files.
  #  """,
  #    0
  #  )
  #)
  #
  opts.Add(
    'pythonCommand',
    'Command used to invoke Python interpreter.',
    '$plat_default_pythonCommand',
  )
  opts.Add(
    'boostIncludeDir',
    'Path to the header directory for Boost.',
    '$plat_default_boostIncludeDir',
  )
  opts.Add(
    'boostPythonLibDir',
    'Directory containing Boost Python library.',
    '$plat_default_boostPythonLibDir',
  )
  opts.Add(
    'boostPythonLib',
    """Name of the Boost Python library.
    - Note : name is sans prefix and suffix; e.g., boost_python-1_33.""",
    '$plat_default_boostPythonLib',
  )
  opts.Add(
    'GccXmlCommand',
    'Command used to call gccxml.',
    '$plat_default_GccXmlCommand',
  )
  opts.Add(
    'pysteScriptPath',
    'Full path to Pyste script.',
    '$plat_default_pysteScriptPath',
  )
  opts.Add(
    'pysteScriptCommand',
    'Command used to call Pyste script.',
    '$plat_default_pysteScriptCommand',
  )

  # FIXME : Move me to support/utils/PysteTool.py, into a conftest
  # therein. @kgn
  env.Append(PYSTE_CMD = '$pysteScriptCommand')

  #opts.Add(
  #  SCons.Options.BoolOption(
  #    'pysteDebug',
  #    """Set to '1' to enable debugging Pyste builders.
  # """,
  #    0
  #  )
  #)
  #env.Append(PYSTE_DEBUG = '$pysteDebug')

#  def customOptionsPathValidator(key, val, env):
#    if val not in ['None', 'none']:
#      if not os.path.exists(val):
#        print """
#... Creating "%s" since it doesn't exist.
#""" % env.subst('$default_custom_options_filename')
#        shutil.copyfile(
#          os.path.join(
#            env.subst('$AvidaUtils_path'),
#            env.subst('$default_custom_options_filename')
#          ),
#          val
#        )

  opts.AddOptions(
    SCons.Options.PathOption(
      'customOptions',
    """Path to custom build configuration file.
    - This file will be created if it doesn't exist.
    """,
      '$default_custom_options_filename',
      #customOptionsPathValidator
    ),
  )
  opts.Update(env)

