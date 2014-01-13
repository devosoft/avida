##############################################################################
## Copyright (C) 1999-2005 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

"""
GCCXMLTool

Tool-specific initialization for gccxml.

Provides configuration variables used to call gccxml. gccxml is used by
Boost.Python's Pyste to help generate C++ files interfacing C++
libraries to Python.
"""

import SCons
import os.path

def _checkKeySubst(env, key):
  if env.Dictionary().has_key(key) and (None != env[key]): return env.subst('$%s' % key)

def find(env):
  """
  Searches for gccxml executable along PATH or PATHEXT environment variable.
  """
  gccxml_command = _checkKeySubst(env, 'GccXmlCommand')
  if not gccxml_command or not os.path.exists(gccxml_command):
    env.Append(
      GCCXML_TOOL_ERR = """
GCCXMLTool error:
  Couldn't find gccxml executable. Is command-line option
  "GccXmlCommand" set correctly?
"""
    )


def generate(env):
  """
  Adds construction variables for gccxml to an Environment.
  """
  env.SetDefault(
    GCCXML_TOOL_ERR = '',
    GCCXML_CMD = '$GccXmlCommand',
  )
  if env.subst('$runConfTests') in ['yes', '1']:
    find(env)

def exists(env):
  return find(env)


