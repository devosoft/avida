##############################################################################
## Copyright (C) 1999-2006 Michigan State University                        ##
## Based on work Copyright (C) 1993-2003 California Institute of Technology ##
##                                                                          ##
## Read the COPYING and README files, or contact 'avida@alife.org',         ##
## before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     ##
##############################################################################

def Update(env):
  # MD5 Source File Signatures vs.  Source File Time Stamps
  #
  # By default,  SCons keeps track of whether a source file has changed
  # based on the file's contents, using its MD5 signature, not the
  # modification time. This can be configured explicity by calling the
  # SourceSignatures function:
  #
  #  SourceSignatures('MD5') # default
  #
  # If you wish, you can configure SCons to use the modification time  of
  # source files by calling the function
  #
  #  SourceSignatures('timestamp')
  
  env.SourceSignatures('MD5')
  
  
  # Build Signatures vs.  File Contents
  #
  # By default, SCons tracks whether a target file must be rebuilt  by using
  # a build signature that consists of the combined signatures of all the
  # files that go into making the target file. This default behavior (build
  # signatures) can be configured explicitly using the TargetSignatures
  # function:
  #
  #  TargetSignatures('build') # default
  #
  # Sometimes a source file can be changed in such a way that the contents
  # of the rebuilt target file(s) will be exactly the same as the last time
  # the file was built. If so, then any other target files that depend on
  # such a built-but-not-changed target file actually need not be rebuilt.
  # You can make SCons realize that it does not need to rebuild a dependent
  # target file in this situation using the TargetSignatures function as
  # follows:
  #
  #  TargetSignatures('content')
  #
  # So configured, SCons takes some extra processing time to scan the
  # contents of the target (hello.o) file,  but this may save time if the
  # rebuild that was avoided would have been very time-consuming and
  # expensive.
  
  env.TargetSignatures('content')
  
  
  # Caching Implicit Dependencies
  #
  # Scanning each file for #include lines takes some extra processing time.
  # When you're doing a full build of a large system, the scanning time is
  # usually a very small percentage of the overall time spent on the build.
  # You're most likely to notice the scanning time, however, when you
  # rebuild  all or part of a large system: SCons will likely take some
  # extra time to "think about" what must be built before it issues the
  # first build command (or decides that everything is up to date and
  # nothing must be rebuilt).
  #
  # Having SCons scan files can save time lost to tracking down subtle
  # problems introduced by incorrect dependencies. Nevertheless, the
  # "waiting time" while SCons scans files can be annoying.  SCons lets you
  # cache the implicit dependencies that its scanners find, for use by later
  # builds. You can do this by specifying the --implicit-cache option on the
  # command line.
  #
  # If you don't want to specify --implicit-cache on the command line each
  # time, you can make it the default behavior for your build by setting the
  # implicit_cache option in an SConscript file:
  #
  #  SetOption('implicit_cache', 1)
  #
  # Sometimes you want to "start fresh" and have SCons re-scan the files for
  # which it previously cached the dependencies.  For example, if you have
  # recently installed a new version of external code that you use for
  # compilation, the external header files will have changed  and the
  # previously-cached implicit dependencies  will be out of date. You can
  # update them by running SCons with the --implicit-deps-changed option on
  # the command line.
  
  env.SetOption('implicit_cache', 1)


