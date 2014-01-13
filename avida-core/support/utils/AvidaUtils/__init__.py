##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

import sys
import SCons
import CmdLineOpts, SConsOpts, StaticHelp, TestUtil

def Configure(args, env):
  env.Replace(AvidaUtils_path = __path__)

  # Load platform-specific configuration and default options.
  env.Tool('PlatformTool', toolpath = __path__)

  # Load custom options file: if user specified the customOptions
  # option, figure out what custom options file they want to use,
  # otherwise use the default one.
  #
  # This step uses platform-specific defaults defined in the
  # 'PlatformTool', so must run after the 'PlatformTool' has been
  # loaded.
  #
  env.Replace(default_custom_options_filename = 'my_avida_build_options.py')
  custom_options = args.get('customOptions', env.subst('$default_custom_options_filename'))
  if custom_options not in ['None', 'none']:
    print "Reading custom options from file '%s' ..." % custom_options

  # Load command-line arguments into Options parser.
  opts = SCons.Options.Options([custom_options], args)

  # Parses Avida-specific command-line arguments, loads default values
  # for options not specified at the command line, and creates help text
  # for available options.
  #
  CmdLineOpts.Update(opts, env)
  SConsOpts.Update(env)
  
  # Load various customized build tools.
  #
  # Some of these can be tweaked by our custom command-line options, so
  # must be loaded after command-line arguments are parsed.
  #
  if env['enablePyPkg'] in ('True', '1', 1):
    env.Tool('GCCXMLTool', toolpath = __path__)
    env.Tool('PythonTool', toolpath = __path__)
    env.Tool('BoostPythonTool', toolpath = __path__)
    env.Tool('PysteTool', toolpath = __path__)
  env.Tool('UnitTestTool', toolpath = __path__)

  # Provide help text.
  StaticHelp.GenerateStaticHelpText(env)
  env.Help(opts.GenerateHelpText(env))
