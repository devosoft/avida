##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

import os
import sys
sys.path.append('support/utils')
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
  environment.Append(CPPDEFINES = ['USE_tMemTrack=1', 'ENABLE_UNIT_TESTS=1'])

if environment['enableTCMalloc'] in ('True', '1', 1):
  environment.Append(
    CPPPATH = [ '#/source/platform/tcmalloc', ],
    LIBPATH = [ '#$buildDir/platform/tcmalloc', ],
  )

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
environment.SConscript('source/SConscript', build_dir = '$buildDir')
environment.SConscript('support/config/SConscript')

# XXX beginnings of consistency tests. @kgn
environment.SConscript('consistencytests/SConscript', build_dir = 'consistencytest_output')

if environment['PLATFORM'] == 'win32':
  script_to_build_avida = environment.File(
    os.path.join('#', os.path.basename(sys.argv[0]))
  ).abspath
  environment.MSVSProject(
    target = 'Avida' + environment['MSVSPROJECTSUFFIX'],
    srcs = environment['avida_msvs_project_srcs'],
    incs = environment['avida_msvs_project_incs'],
    misc = environment['avida_msvs_project_misc'],
    variant = 'Release',
    #runfile = avida[0].abspath,
    MSVSSCONS = '"%s" "%s"' % (sys.executable, script_to_build_avida),
    MSVSSCONSFLAGS = '-C "${MSVSSCONSCRIPT.dir.abspath}" -f "${MSVSSCONSCRIPT.name}"'
  )

# Vim modeline to tell Vim that this is a Python script.
# vim: set ft=python:
