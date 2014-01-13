##############################################################################
## Copyright (C) 1999-2005 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

"""
PythonTool

Tool-specific initialization for Python.

Provides configuration variables used to build Python extension modules.
"""

## Values on my PowerBook:
#
#  env['PYTHON']            '/usr/local/bin/python'
#  env['PYTHON_CC']         'gcc'
#  env['PYTHON_CXX']        'c++'
#  env['PYTHON_BASECFLAGS'] '-fno-strict-aliasing -Wno-long-double -no-cpp-precomp -mno-fused-madd -fno-common -dynamic'
#  env['PYTHON_OPT']        '-DNDEBUG -g -O3 -Wall -Wstrict-prototypes'
#  env['PYTHON_INCLUDEPY']  '/Library/Frameworks/Python.framework/Versions/2.4/include/python2.4'
#  env['PYTHON_CCSHARED']   ''
#  env['PYTHON_LDSHARED']   'g++  -bundle -Wl,-F/Library/Frameworks -framework Python'
#  env['PYTHON_SO']         '.so'

import SCons
import os
import re

none_re = re.compile('None')
trailing_whitespace_re = re.compile(r'^\s*(.*)\s*$')

def queryPython(python_command, sysconfig_variable):
  query_command = """ -c "import distutils.sysconfig; print distutils.sysconfig.get_config_var('%s')," """
  line = os.popen(python_command + query_command % sysconfig_variable).readline()
  line = trailing_whitespace_re.sub(r'\1', line)
  if not none_re.match(line): return line
  else: return None

def checkPython(python_command):
  check_command = """ -c "print 'hello'," """
  print "python_command + check_command", python_command + check_command
  the_file = os.popen(python_command + check_command)
  the_file.read()
  return the_file.close()

def find(env):
  if env['PYTHON'] is None:
    env.Append(
      PYTHON_TOOL_ERR = """
PythonTool error:
  PYTHON has been set to None
  since no Python executable
  $$pythonCommand == '%s'
  was found.
""" % env.subst('$pythonCommand')
    )
    
  pycmd = env.subst('$PYTHON')
  if checkPython(pycmd) is not None:
    env.Append(
      PYTHON_TOOL_ERR = """
PythonTool error:
  Python command doesn't seem to work. Is command-line option
  "pythonCommand" set correctly?
""" % pycmd
    )

def generate(env):
  """
  Adds construction variables for Python to an Environment.
  """
  env.SetDefault(
    PYTHON = env.Detect(env.subst('$pythonCommand')),
    PYTHON_TOOL_ERR = '',
  )
  pycmd = env.subst('$PYTHON')

  # Ask Python about its preferred build tools.
  #
  # Preferred C compiler.
  python_cc =         queryPython(pycmd, 'CC')
  # Preferred C++ compiler.
  python_cxx =        queryPython(pycmd, 'CXX')
  # Preferred complier flags.
  python_basecflags = queryPython(pycmd, 'BASECFLAGS')
  # Additional compiler flags to use when optimizing.
  python_opt =        queryPython(pycmd, 'OPT')
  # Path to Python header files.
  python_includepy =  queryPython(pycmd, 'INCLUDEPY')
  # Command to compile shared object used to build python module.
  python_ccshared =   queryPython(pycmd, 'CCSHARED')
  # Command used to link python module.
  python_ldshared =   queryPython(pycmd, 'LDSHARED')
  #   Fixup -- typical link command is for C, not C++.
  python_ldshared = re.sub(python_cc, python_cxx, python_ldshared)
  # FIXME: osx-specific fixup -- @kgn
  python_framework =   queryPython(pycmd, 'PYTHONFRAMEWORK')
  python_framework_prefix =   queryPython(pycmd, 'PYTHONFRAMEWORKPREFIX')
  python_framework_dir =   queryPython(pycmd, 'PYTHONFRAMEWORKDIR')
  python_ldshared += ' -F%s -framework %s' % (python_framework_prefix, python_framework)
  # Suffix for python modules.
  python_so =         queryPython(pycmd, 'SO')
  env.SetDefault(
    PYTHON_CC =         SCons.Util.CLVar(python_cc),
    PYTHON_CXX =        SCons.Util.CLVar(python_cxx),
    PYTHON_BASECFLAGS = SCons.Util.CLVar(python_basecflags),
    PYTHON_OPT =        SCons.Util.CLVar(python_opt),
    PYTHON_INCLUDEPY =  SCons.Util.CLVar(python_includepy),
    PYTHON_CCSHARED =   SCons.Util.CLVar(python_ccshared),
    PYTHON_LDSHARED =   SCons.Util.CLVar(python_ldshared),
    PYTHON_SO =         SCons.Util.CLVar(python_so),
  )

  if env.subst('$runConfTests') in ['yes', '1']:
    find(env)

def exists(env):
  return find(env)
  #return env.Detect(env.subst('$pythonCommand'))
