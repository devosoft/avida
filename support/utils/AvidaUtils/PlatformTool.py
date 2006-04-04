##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

"""
PlatformTool

Platform-specific initialization for Avida build system.
"""

import SCons
import os
import sys

CLVar = SCons.Util.CLVar

def _posix_defaults(env):
  env.SetDefault(
    PLATFORM_CPPPATH = CLVar('/usr/include /usr/local/include /opt/local/include' '$CPPPATH'),
    PLATFORM_LIBPATH = CLVar('/usr/lib /usr/local/lib /opt/local/lib' '$LIBPATH'),
    _PLATFORM_DEBUG_BUILD_FLAGS = "-g $COMPILER_WARNING_FLAGS -pedantic -DDEBUG",
    _PLATFORM_RELEASE_DEBUG_BUILD_FLAGS = "-O2 -ffast-math -g $COMPILER_WARNING_FLAGS -pedantic -DDEBUG",
    _PLATFORM_RELEASE_BUILD_FLAGS = "-O3 -ffast-math -DNDEBUG",
    _PLATFORM_MINIMUM_RELEASE_BUILD_FLAGS = "-Os -DNDEBUG",
  )

def darwin_generate(env):
  _posix_defaults(env)
  env.Append(PLATFORM_CPPPATH = CLVar('/sw/include'))
  env.Append(PLATFORM_LIBPATH = CLVar('/sw/lib'))
  env['ENV']['MACOSX_DEPLOYMENT_TARGET'] = '10.4'
  env['ENV']['GCCXML_COMPILER'] = 'c++-3.3'

  env.SetDefault(
    COMPILER_WARNING_FLAGS = "-Wall -Wno-long-double -Wno-long-long",
    plat_default_enableTestCode = 0,
    plat_default_enablePyPkg = 0,
    plat_default_enableSharedPtr = 0,
    plat_default_pythonCommand = sys.executable,
    plat_default_boostIncludeDir = None,
    plat_default_boostPythonLibDir = None,
    plat_default_boostPythonLib = None,
    plat_default_GccXmlCommand = None,
    plat_default_pysteScriptPath = os.path.join(sys.path[0], '${support_utils_rel_dir}', 'AvidaPyste.py'),
    plat_default_pysteScriptCommand = SCons.Util.CLVar('$pythonCommand $pysteScriptPath'),
  )

def linux_generate(env):
  pass

def cygwin_generate(env):
  pass

def win32_generate(env):
  pass

platform_generators = {
  'darwin': darwin_generate,
  #'linux': linux_generate,
  #'cygwin': cygwin_generate,
  #'win32': win32_generate,
}

def generate(env):
  env.SetDefault(
    PLATFORM_TOOL_ERR = '',
  )
  if env.Dictionary().has_key('PLATFORM'):
    if platform_generators.has_key(env['PLATFORM']):
      platform_generators[env['PLATFORM']](env)
    else:
      env.Append(
        PLATFORM_TOOL_ERR = """
PlatformTool error:
  Platform $$PLATFORM == '%s'
  isn't supported yet.
""" % env['PLATFORM']
      )
  else:
    env.Append(
      PLATFORM_TOOL_ERR = """
PlatformTool error:
  Couldn't obtain $$PLATFORM from environment.
"""
    )
    

def exists(env):
  return True
