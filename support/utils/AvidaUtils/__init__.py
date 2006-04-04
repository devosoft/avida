##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

import sys
import SCons
import CmdLineOpts, SConsOpts, StaticHelp

def Configure(args, env):
  env.Replace(AvidaUtils_path = __path__)

  # Load custom options file: if user specified the customOptions
  # option, figure out what custom options file they want to use,
  # otherwise use the default one.
  #
  env.Replace(default_custom_options_filename = 'my_avida_build_options.py')
  custom_options = args.get('customOptions', env.subst('$default_custom_options_filename'))
  if custom_options not in ['None', 'none']:
    print "Reading custom options from file '%s' ..." % custom_options

  # Load command-line arguments into Options parser.
  opts = SCons.Options.Options([custom_options], args)
  
  # Load platform-specific configuration and default options.
  env.Tool('PlatformTool', toolpath = __path__)

  # Parses Avida-specific command-line arguments, loads default values
  # for options not specified at the command line, and creates help text
  # for available options.
  #
  CmdLineOpts.Update(opts, env)
  SConsOpts.Update(env)
  StaticHelp.GenerateStaticHelpText(env)
  env.Help(opts.GenerateHelpText(env))
