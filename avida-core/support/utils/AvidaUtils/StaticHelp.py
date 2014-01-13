##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

def GenerateStaticHelpText(env):
  env.Help("""
*** Help for Avida builds

SCons is used as the build engine and is included in the subdirectory
'support/scons'.

For more about SCons see http://www.scons.org or read the file:
  support/third-party/scons/scons-user.html

SCons reads the file 'SConstruct' to figure out what to build.
""")
#  env.Help("""
#*** Interesting build targets:
#
#  c++ unit tests:
#    'dumb_tPtr_test'
#    'shared_tPtr_test'
#    'extended_shared_tPtr_test'
#    'shared_tPtr_basic_test'
#    'shared_tPtr_from_this_test'
#    'tPtr_unit_tests' - runs all c++ unit tests of tPtr.
#    'dumb_tList_test'
#    'shared_tList_test'
#    'tList_unit_tests' - runs all c++ unit tests of tList.
#
#  Python unit tests (these mostly just test instantiation):
#    'unit_test_cFile'
#    'unit_test_cInitFile'
#    'unit_test_cRandom'
#    'unit_test_cString'
#    'unit_test_cStringIterator'
#    'unit_test_cStringList'
#    'unit_test_cStringUtil'
#    'combined_core_unit_tests' - runs all the above Python unit tests.
#
#  XXX I guess it's obvious, but the options options 'enableTestCode' and
#  'enablePyPkg' must be set to '1' or 'yes' to make these targets
#  available... @kgn
#
#  XXX These tests require Boost, so Boost is unavailable (if
#  $boostIncludeDir is unset), these tests won't be built or run.  @kgn
#
#  XXX At the moment, there's no real application to build...  @kgn
#""")
  env.Help("""
*** Setting build options:

Set build options at the command line like this:
  ./build_avida Foo=foo Bar=bar

You can also place build options into the file named
  '%s',
one option per line, in the form
  Foo = 'foo'
  Bar = 'bar'
Put this file in the top level Avida source code directory. Command line
build options override the options in this file.
""" % env.subst('$default_custom_options_filename'))
  env.Help("""
*** Available build options:
""")
