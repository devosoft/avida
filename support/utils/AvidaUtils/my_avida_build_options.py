##############################################################################
## Copyright (C) 1999-2005 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

# For convenience you can use this file to specify build options to the
# Avida build system.  This file must be in the top level Avida source
# code directory for the build system to find it. Add options one per
# line in the form
#
#   Foo = 'foo'
#   Bar = 'bar'
#
# Build options specified at the command line override the options in
# this file.
#
# For more information about build options, type
#
#   ./build_avida -h
#
# In the top level Avida source code directory.
#
# Note: this file is also a Python script, so you can do some processing
# here if you like.

buildType = "Release"

enablePyPkg = "no"
enableTestCode = "no"

boostIncludeDir = None
boostPythonLibDir = None
boostPythonLib = None
GccXmlCommand = None

# Vim modeline to tell Vim that this is a Python script.
# vim: set ft=python:
