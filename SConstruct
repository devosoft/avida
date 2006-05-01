##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

import AvidaUtils

environment = Environment()
environment.Export('environment')

# Parse cmd line, set compiler options, generate help text.
#
# Per-platform build options are defined in
# support/utils/AvidaUtils/PlatformTool.py
#
# Command-line build options are defined in
# support/utils/AvidaUtils/CmdLineOpts.py
#
# Options for SCons are defined in
# support/utils/AvidaUtils/SConsOpts.py
#
# Static help text is defined in
# support/utils/AvidaUtils/StaticHelp.py
#
AvidaUtils.Configure(ARGUMENTS, environment)

if environment.subst('$enableTestCode') in ['1', 'yes']:
  environment.SetDefault(enableSharedPtr = 1)
  environment.Append(CPPDEFINES = ['USE_tMemTrack=1'])

environment.Append(
  CPPPATH = [
    '#/source',
    '#/source/actions',
    '#/source/analyze',
    '#/source/archive',
    '#/source/classification',
    '#/source/cpu',
    '#/source/drivers',
    '#/source/event',
    '#/source/main',
    '#/source/third-party/boost',
    '#/source/tools',
  ],
  LIBPATH = [
    '#$buildDir/actions',
    '#$buildDir/analyze',
    '#$buildDir/archive',
    '#$buildDir/classification',
    '#$buildDir/cpu',
    '#$buildDir/drivers',
    '#$buildDir/event',
    '#$buildDir/main',
    '#$buildDir/third-party/boost/serialization',
    '#$buildDir/tools',
  ],
)

# Tell SCons where to find its subscripts.
environment.SConscript('source/SConscript', build_dir = "$buildDir")
environment.SConscript('support/config/SConscript')


# Vim modeline to tell Vim that this is a Python script.
# vim: set ft=python:
