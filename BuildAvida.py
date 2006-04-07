#! /usr/bin/env python

##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

# Use this script to build Avida. It calls SCons, which reads
# information from the file 'SConstruct'.
#
# For more information about SCons, see
#
#   http://www.scons.org
#
# or read the SCons html documentation in the included file
#
#   support/scons/scons-user.html
#

import os
import sys

# Unless user has asked for silence or help, print a short help message
# offering more help if needed.
if '-h' not in sys.argv \
and '-H' not in sys.argv \
and '-s' not in sys.argv \
and '--silent' not in sys.argv \
and '--quiet' not in sys.argv:
  print """
Note: if you need help, type
%s -h
""" % sys.argv[0]

# Tell Python where to find the main SCons build script, the SCons
# library, and our SCons customizations.
sys.path.append('support/scons')
os.environ["SCONS_LIB_DIR"] = os.path.join(sys.path[0],
  'support/scons/scons-local-0.96.91')
sys.path.append('support/utils')

# Any command line arguments are passed to SCons when it is imported by
# the next line.
import scons


# Vim modeline to tell Vim that this is a Python script.
# vim: set ft=python:
