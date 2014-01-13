##############################################################################
## Copyright (C) 1999-2005 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

"""
BoostPythonTool

Tool-specific initialization for Boost.Python.

Provides builders and configuration variables for building Python
extension modules to interface with C++ libraries.
"""

import SCons
import os
import re

Split = SCons.Util.Split

def _checkKey(env, key):
  if env.Dictionary().has_key(key) and (None != env[key]): return env[key]

def _checkKeySubst(env, key):
  if env.Dictionary().has_key(key) and (None != env[key]): return env.subst('$%s' % key)

def CheckBoostPython(context):
  """
  Custom test to verify Boost.Python functionality. Tries building a
  small Boost.Python module using provided Boost.Python library, library
  path, and include directory.
  """
  # FIXME: when we get the chance, write some code to verify that the
  # built python module works. @kgn

  # Try building...
  context.Message("Checking Boost.Python ... ")
  result = context.TryBuild(
    context.env.BoostPythonModule,
    """
#include <boost/python.hpp>
#include <boost/cstdint.hpp>
class boost_python_conftest
{ public: boost_python_conftest(){} };
using namespace boost::python;
BOOST_PYTHON_MODULE(boost_python_conftest)
{ class_< boost_python_conftest, boost::noncopyable >("boost_python_conftest", init<  >()); }
int main(int argc, char **argv){
  init_module_boost_python_conftest();
  return 0;
}
""",
    context.env['BOOST_PYTHON_CXX_SUFFIX']
  )
  context.Result(result)
  return result

def find(env):
  boost_python_lib = _checkKeySubst(env, 'boostPythonLib')
  boost_python_lib_dir = _checkKeySubst(env, 'boostPythonLibDir')
  boost_include_dir = _checkKeySubst(env, 'boostIncludeDir')
  env2 = env.Copy()
  conf = env2.Configure(custom_tests = {'CheckBoostPython' : CheckBoostPython})
  result = conf.CheckBoostPython()
  conf.Finish()
  if not result:
    env.Append(
      BOOST_PYTHON_TOOL_ERR = """
BoostPythonTool error:
  Boost.Python not functional: I tried and failed to use Boost.Python to
  make a small Python module. Are command-line options "boostPythonLib",
  "boostPythonLibDir", "boostIncludeDir" set correctly?
"""
    )


ShCXXAction = SCons.Action.Action("$BOOST_PYTHON_SHCXXCOM", "$BOOST_PYTHON_SHCXXCOMSTR")
LdModuleLinkAction = SCons.Action.Action("$BOOST_PYTHON_LDMODULECOM", "$BOOST_PYTHON_LDMODULECOMSTR")

def generate(env):
  """
  Adds builders and construction variables for Boost.Python to an
  Environment.
  """
  env.SetDefault(
    BOOST_PYTHON_TOOL_ERR = '',
    BOOST_PYTHON_CPPFLAGS = ['$PYTHON_BASECFLAGS'],
    BOOST_PYTHON_CPPDEFINES = [],
    BOOST_PYTHON_CPPPATH = ['$boostIncludeDir', '$PYTHON_INCLUDEPY'],
    BOOST_PYTHON_CXX_SUFFIX = '.boost.python.cpp',
    BOOST_PYTHON_LIBPATH = ['$boostPythonLibDir'],
    BOOST_PYTHON_LIBS = ['$boostPythonLib'],
    BOOST_PYTHON_SHLINK = '$PYTHON_LDSHARED',
    BOOST_PYTHON_LDMODULE = '$BOOST_PYTHON_SHLINK',
    BOOST_PYTHON_LDMODULEFLAGS = '',
    _BOOST_PYTHON_CPPINCFLAGS = '$( ${_concat(INCPREFIX, BOOST_PYTHON_CPPPATH, INCSUFFIX, __env__, RDirs, TARGET)} $)',
    _BOOST_PYTHON_CPPDEFFLAGS = '${_defines(CPPDEFPREFIX, BOOST_PYTHON_CPPDEFINES, CPPDEFSUFFIX, __env__)}',
    _BOOST_PYTHON_LIBFLAGS = '${_stripixes(LIBLINKPREFIX, BOOST_PYTHON_LIBS, LIBLINKSUFFIX, LIBPREFIX, LIBSUFFIX, __env__)}',
    _BOOST_PYTHON_LIBDIRFLAGS = '$( ${_concat(LIBDIRPREFIX, BOOST_PYTHON_LIBPATH, LIBDIRSUFFIX, __env__, RDirs, TARGET)} $)',
    BOOST_PYTHON_SHCXXCOM = '$SHCXX $SHCXXFLAGS $CPPFLAGS $BOOST_PYTHON_CPPFLAGS $_CPPDEFFLAGS $_BOOST_PYTHON_CPPDEFFLAGS $_CPPINCFLAGS $_BOOST_PYTHON_CPPINCFLAGS -c -o $TARGET $SOURCES',
    BOOST_PYTHON_LDMODULECOM = '$BOOST_PYTHON_LDMODULE $BOOST_PYTHON_LDMODULEFLAGS -o ${TARGET} $SOURCES $_LIBDIRFLAGS $_BOOST_PYTHON_LIBDIRFLAGS $_LIBFLAGS $_BOOST_PYTHON_LIBFLAGS',
  )

  boost_python_ld_module_link_action = SCons.Action.Action("$BOOST_PYTHON_LDMODULECOM", "$BOOST_PYTHON_LDMODULECOMSTR")
  boost_python_shared_object_builder = SCons.Builder.Builder(
    action = [ ShCXXAction ],
    prefix = '$SHOBJPREFIX',
    suffix = '$SHOBJSUFFIX',
    src_suffix = '$BOOST_PYTHON_CXX_SUFFIX',
    source_scanner = SCons.Tool.SourceFileScanner,
    single_source = True
  )
  boost_python_module_builder = SCons.Builder.Builder(
    action = [ LdModuleLinkAction ],
    prefix = '',
    suffix = '$PYTHON_SO',
    target_scanner = SCons.Tool.ProgramScanner,
    src_suffix = '$SHOBJSUFFIX',
    src_builder = 'BoostPythonSharedObject',
    single_source = True
  )
  env.AppendUnique(BUILDERS = {'BoostPythonSharedObject' : boost_python_shared_object_builder})
  env.AppendUnique(BUILDERS = {'BoostPythonModule' : boost_python_module_builder})

  if env.subst('$runConfTests') in ['yes', '1']:
    find(env)

def exists(env):
  return find(env)

