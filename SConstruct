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

environment.Append(
  CPPPATH = [
    '#/source',
    '#/source/analyze',
    '#/source/classification',
    '#/source/cpu',
    '#/source/drivers',
    '#/source/event',
    '#/source/main',
    '#/source/tools',
  ],
  LIBPATH = [
    '#$buildDir/analyze',
    '#$buildDir/classification',
    '#$buildDir/cpu',
    '#$buildDir/drivers',
    '#$buildDir/event',
    '#$buildDir/main',
    '#$buildDir/tools',
  ],
)

# Tell SCons to read script in 'source' subdirectory.
environment.SConscript('source/SConscript', build_dir = "$buildDir")


# Vim modeline to tell Vim that this is a Python script.
# vim: set ft=python:
