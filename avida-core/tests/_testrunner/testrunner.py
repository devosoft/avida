#!/usr/bin/python

#  Copyright 2007-2011 David Michael Bryson, all rights reserved.
#  http://programerror.com/software/testrunner
#
#  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
#  following conditions are met:
#
#  1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the
#      following disclaimer.
#  2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
#      following disclaimer in the documentation and/or other materials provided with the distribution.
#  3.  Neither the name of David Michael Bryson, nor the names of contributors may be used to endorse or promote
#      products derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY DAVID MICHAEL BRYSON AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
#  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL DAVID MICHAEL BRYSON OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
#  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  Authors: David M. Bryson <david@programerror.com>, Brian Baer <baerb@msu.edu>
#


import ConfigParser
import difflib
import dircache
import fnmatch
import getopt
import os
import shutil
import string
import subprocess
import sys
import tempfile
import threading
import time
import xml.dom.minidom


# This file has been formatted for editors/terminals 125 characters wide


# Global Constants
# ---------------------------------------------------------------------------------------------------------------------------
TESTRUNNER_VERSION = "2.0"
TESTRUNNER_COPYRIGHT = "2007-2011"

TRUE_STRINGS = ("y", "Y", "yes", "Yes", "true", "True", "1")
RESAVAIL = True

# -- Directory/Filename Definitions
CONFIGDIR = "config"
EXPECTDIR = "expected"
PERFDIR = "perf~"  # subversion, by default, ignores files/dirs with ~ at the end
TEST_LIST = "test_list"
PERF_BASE = "baseline"
EXPECTED_IGNORE = (".gitignore",)


# Global Variables
# ---------------------------------------------------------------------------------------------------------------------------
settings = {}   # {string:string}
tmpdir = None   # string


# Conditional Imports
# ---------------------------------------------------------------------------------------------------------------------------
try:
    import resource
except ImportError:
    RESAVAIL = False


# Global Helper Functions
# ---------------------------------------------------------------------------------------------------------------------------

# void usage() {
def usage():
    global settings
    usagestr = """
Usage: %(_testrunner_name)s [options] [testname ...]

  Runs the specified tests.  Test names can be unix-style globbing patterns.
  If no tests are specified all available tests will be run and new expected
  results generated, where applicable.

  Options:
    --builddir=dir [%(builddir)s]
      Set the path to the build directory.

    -f | --force-perf
      Force active tests to be treated as peformance tests, regardless of
      individual test configuration.

    -h | --help
      Display this message

    --help-test-cfg
      Display a sample test configuration file

    --ignore-consistency
      Ignore consistency tests altogether.  Performance results are not saved.
      Valid only when used in conjunction with -p or --run-perf-tests.

    -j number [%(cpus)d]
      Set the number of concurrent tests to run. i.e. - the number of CPUs
      that are available.

    -l | --list-tests
      List all available tests and exits.  Tests that will require new
      expected results will have an asterisk preceeding the name.

    --long-tests
      Run tests that have been marked as 'long' running.

    --mode=option [%(mode)s]
      Set the test runner mode.  Options are 'local' and 'slave'.

      Local mode generates expected results and adds them to the repository,
      if enabled has been found. Slave mode disables expected results generation.

    --scm=option [%(scm)s]
      Set the source code management system.
      Options are 'git', 'svn', and 'none'.

      None disables all automatic source code management operations.

    -p | --run-perf-tests
      Run available performance tests.

    --reset-expected
      Reset the expected results for active consistency tests

    --reset-perf-base
      Reset performance test baseline results.  Old baseline results are
      saved in the 'perf' directory.

    --show-diff
      Show full file diff for failed consistency tests.

    --diff-max-threshold=option [1536]
      Threshold size, in bytes, at which testrunner will no longer
      calculate detailed diffs. Prevents diff hangups.

    --skip-tests
      Do not run tests. Only generate new results, where applicable.

    -g path | --git=path [%(git)s]
      Set the path to the Git command line utility.

    -s path | --svn=path [%(svn)s]
      Set the path to the Subversion command line utility.

    --svnversion=path [%(svnversion)s]
      Set the path to the Subversion 'svnversion' command line utility.

    --svnmetadir=dir [%(svnmetadir)s]
      Set the name of the Subversion metadata directory.

    --testdir=dir [%(testdir)s]
      Set the path to the directory containing tests.

    -v | --verbose
      Enable verbose output, showing all test output.

    --version
      Show version information.
""" % settings
    print usagestr
# } // End of usage()


# void sample_test_list() {
def sample_test_list():
    global settings, TEST_LIST
    test_list = ";--- Begin Test Configuration File (%s) ---" % TEST_LIST
    test_list += """
[main]
; Command line arguments to pass to the application
args =

app = %(default_app)s            ; Application path to test
nonzeroexit = disallow   ; Exit code handling (disallow, allow, or require)
                         ;  disallow - treat non-zero exit codes as failures
                         ;  allow - all exit codes are acceptable
                         ;  require - treat zero exit codes as failures, useful
                         ;            for creating tests for app error checking
createdby =              ; Who created the test
email =                  ; Email address for the test's creator

[consistency]
enabled = yes            ; Is this test a consistency test?
long = no                ; Is this test a long test?

[performance]
enabled = no             ; Is this test a performance test?
long = no                ; Is this test a long test?

; The following variables can be used in constructing setting values by calling
; them with %(variable_name)s.  For example see 'app' above.
;
"""
    sk = settings.keys()
    sk.sort()
    for set in sk:
        if set[0] != "_":
            test_list += "; %s \n" % set
    test_list += ";--- End Test Configuration File ---"
    print test_list
# } // End of sample_test_list()


# void version() {
def version():
    global TESTRUNNER_VERSION, TESTRUNNER_COPYRIGHT
    versionstr = """
TestRunner v%s
Copyright %s David Michael Bryson, all rights reserved.

This software is open source, subject to certain conditions.
See the supplied license for details.

http://www.programerror.com/software/testrunner
""" % (TESTRUNNER_VERSION, TESTRUNNER_COPYRIGHT)
    print versionstr
# } // End of version()


# // Calculate the median of a sequence
# int med(int[] seq) {
def med(seq):
    seq.sort()
    idx = len(seq) / 2
    if len(seq) % 2 == 1:
        return seq[idx]
    else:
        return (seq[idx] + seq[idx - 1]) / 2
# } // End of med()


class SCMWrapper_Git:

    def __init__(self):
        global settings
        self.cmd = settings["git"]
        self.submodule = settings["git_submodule"]
        print self.submodule

    def goSubmodule(self):
        if self.submodule != "":
            os.chdir(self.submodule)

    def retSubmodule(self):
        if self.submodule != "":
            os.chdir("..")

    def getVersionString(self, path):
        self.goSubmodule()
        rev = "exported"
        try:
            gverp = os.popen("%s describe" % (self.cmd))
            rev = gverp.readline().strip()
            gverp.close()
            if rev == "":
                rev = "exported"
        except (IOError, OSError):
            pass
        self.retSubmodule()
        return rev

    def deleteMetadata(self, path): pass

    def removeMetadataFromDirList(self, dirs): pass

    def addDirectory(self, dir):
        self.goSubmodule()
        open(os.path.join(dir, ".gitignore"), "a").close()
        ecode = os.spawnlp(os.P_WAIT, self.cmd, self.cmd, "add", dir)
        if ecode != 0:
            self.retSubmodule()
            return False

        self.retSubmodule()
        return True

    def removeDirectory(self, dir):
        self.goSubmodule()
        ecode = os.spawnlp(os.P_WAIT, self.cmd, self.cmd, "rm", "-r", dir)
        if ecode != 0:
            self.retSubmodule()
            return False
        if os.path.exists(dir):
            self.retSubmodule()
            return False
        self.retSubmodule()
        return True


class SCMWrapper_SVN:

    def __init__(self):
        global settings
        self.cmd = settings["svn"]
        self.version = settings["svnversion"]
        self.metadir = settings["svnmetadir"]

        if not os.path.exists(self.metadir):
            print "Warning: Current directory does not appear to be a SVN working copy"

    def getVersionString(self, path):
        rev = "exported"
        try:
            sverp = os.popen("cd %s; %s" % (path, self.version))
            rev = sverp.readline().strip()
            sverp.close()
            if rev == "":
                rev = "exported"
        except (IOError, OSError):
            pass
        return rev

    def deleteMetadata(self, path):
        # Remove copied svn metadata directories
        for root, dirs, files in os.walk(path):
            if svnmetadir in dirs:
                dirs.remove(self.metadir)
            try:
                shutil.rmtree(os.path.join(root, self.metadir))
            except (IOError, OSError):
                pass

    def removeMetadataFromDirList(self, dirs):
        if self.metadir in dirs:
            dirs.remove(self.metadir)

    def addDirectory(self, dir):
        ecode = os.spawnlp(os.P_WAIT, self.cmd, self.cmd, "add", dir)
        if ecode != 0:
            return False
        return True

    def removeDirectory(self, dir):
        print "Error: SVN does not currently support directory removal"
        return False


class SCMWrapper_None:

    def __init__(self): pass

    def getVersionString(self, path): return "exported"

    def deleteMetadata(self, path): pass

    def removeMetadataFromDirList(self, dirs): pass

    def addDirectory(self, dir): return True

    def removeDirectory(self, dir):
        try:
            shutil.rmtree(dir, True)
        except (IOError, OSError):
            return False
        return True


# Main Test Class - does the actual work for performing individual tests and reporting results
# ---------------------------------------------------------------------------------------------------------------------------

# class cTest {
class cTest:
    NOTFOUND = "file not found"
    DONOTMATCH = "content does not match"

    # cTest::cTest(string name, string tdir) {
    def __init__(self, name, tdir):
        global settings, TRUE_STRINGS, RESAVAIL, EXPECTDIR, PERFDIR, TEST_LIST, PERF_BASE
        self.name = name
        self.tdir = tdir
        self.scm = settings["scm"]

        if settings.has_key("skip-tests"):
            self.skip = True
        else:
            self.skip = False

        self.cfg = ConfigParser.ConfigParser(settings)
        self.cfg.read([os.path.join(tdir, TEST_LIST)])

        expectdir = os.path.join(tdir, EXPECTDIR)
        if os.path.exists(expectdir) and os.path.isdir(expectdir):
            self.has_expected = True
        else:
            self.has_expected = False

        perfdir = os.path.join(tdir, PERFDIR)
        if os.path.exists(perfdir) and os.path.isdir(perfdir) and os.path.isfile(os.path.join(perfdir, PERF_BASE)):
            self.has_perf_base = True
        else:
            self.has_perf_base = False

        if self.has_perf_base and settings.has_key("_reset_perf_base"):
            try:
                rev = self.scm.getVersionString(self.tdir)
                oname = "perf-%s-reset-%s" % (
                    time.strftime("%Y-%m-%d-%H.%M.%S"), rev)

                shutil.move(os.path.join(perfdir, PERF_BASE),
                            os.path.join(perfdir, oname))
                print "%s : performance baseline reset" % name
            except (IOError, OSError, shutil.Error):
                pass

        # Load the App for the test and check that it exists
        try:
            self.app = self.cfg.get('main', 'app', False, settings)
        except:
            self.app = settings['default_app']
        self.app = os.path.abspath(self.app)
        if not os.path.exists(self.app):
            print "Error: Application (%s) not found" % self.app
            sys.exit(-1)
        if not os.path.isfile(self.app):
            print "Error: Application (%s) is not a file" % self.app
            sys.exit(-1)

        self.args = self.getConfig("main", "args", "")

        if self.getConfig("consistency", "enabled", "yes") in TRUE_STRINGS:
            self.consistency_enabled = True
        else:
            self.consistency_enabled = False
        if self.getConfig("performance", "enabled", "no") in TRUE_STRINGS and RESAVAIL:
            self.performance_enabled = True
        else:
            self.performance_enabled = False

        self.success = True
        self.result = "passed"
        self.disabled = False
        self.exitcode = 0
        self.errors = []

        self.psuccess = True
        self.pdisabled = False
        self.presult = "passed"
    # } // End of cTest::cTest()

    # string cTest::getConfig(string sect, string opt, string default)
    def getConfig(self, sect, opt, default):
        global settings
        try:
            return self.cfg.get(sect, opt, False, settings)
        except:
            return default
    # } // End of cTest::getConfig()

    def getName(self): return self.name

    # bool cTest::isConsistencyTest() {
    def isConsistencyTest(self): return self.consistency_enabled
    # } // End of isConsistencyTest()

    # bool cTest::wasConsistencySkipped() {
    def wasConsistencySkipped(self): return self.disabled
    # } // End of wasConsistencySkipped()

    # bool cTest::isPerformanceTest() {
    def isPerformanceTest(self): return self.performance_enabled
    # } // End of isPerformanceTest()

    # bool cTest::wasPerformanceSkipped() {
    def wasPerformanceSkipped(self): return self.pdisabled
    # } // End of wasPerformanceSkipped()

    # void cTest::runConsistencyTest() {
    def runConsistencyTest(self, dolongtest):
        global settings, tmpdir, CONFIGDIR, EXPECTDIR

        confdir = os.path.join(self.tdir, CONFIGDIR)
        rundir = os.path.join(tmpdir, self.name)
        expectdir = os.path.join(self.tdir, EXPECTDIR)

        if not self.isConsistencyTest():
            self.result = "skipped (not a consistency test)"
            self.disabled = True
            return

        # If no expected results exist and in slave mode
        if not self.has_expected and settings["mode"] == "slave":
            self.result = "skipped (no expected results)"
            self.disabled = True
            return

        if settings.has_key("_reset_expected"):
            if not self.scm.removeDirectory(expectdir):
                print "Error: unable to remove expected directory for reset"
                self.success = False
                return
            self.has_expected = False

        if self.has_expected and self.skip:
            self.result = "skipped"
            self.disabled = True
            return

        if self.getConfig("consistency", "long", "no") in TRUE_STRINGS and not dolongtest:
            self.result = "skipped (long)"
            self.disabled = True
            return

        # Create test directory and populate with config
        try:
            shutil.copytree(confdir, rundir)
        except (IOError, OSError), e:
            print "Error: unable to create run dir"
            print "  -- root cause: %s" % e
            self.success = False
            return

        self.scm.deleteMetadata(rundir)

        # Run test app, capturing output and exitcode
        p = subprocess.Popen("cd %s; %s %s" % (rundir, self.app, self.args), shell=True,
                             stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, close_fds=True)

        # Process output from app
        # Note: must at least swallow app output so that the process output
        # buffer does not fill and block execution
        if settings.has_key("_verbose"):
            print
        for line in p.stdout:
            if settings.has_key("_verbose"):
                sys.stdout.write("%s output: %s" % (self.name, line))
                sys.stdout.flush()

        self.exitcode = p.wait()

        # Check exit code, depending on mode setting
        nz = self.getConfig("main", "nonzeroexit", "disallow")
        if (nz == "disallow" and self.exitcode != 0) or (nz == "require" and self.exitcode == 0):
            self.success = False
            try:
                shutil.rmtree(rundir, True)  # Clean up test directory
            except (IOError, OSError):
                pass
            return

        # Build dictionary of config structure
        confstruct = {}
        for root, dirs, files in os.walk(confdir):
            self.scm.removeMetadataFromDirList(dirs)
            for file in files:
                path = os.path.abspath(os.path.join(root, file))
                key = path[len(confdir) + 1:]  # remove confdir from path
                confstruct[key] = path

        # If no expected results exist, defer processing of new expected
        # results to results phase
        if not self.has_expected:
            self.confstruct = confstruct
            return

        # Build dicitonary of expected structure
        expectstruct = {}
        for root, dirs, files in os.walk(expectdir):
            self.scm.removeMetadataFromDirList(dirs)
            for ifile in EXPECTED_IGNORE:
                if ifile in files:
                    files.remove(ifile)
            for file in files:
                path = os.path.abspath(os.path.join(root, file))
                key = path[len(expectdir) + 1:]  # remove confdir from path
                expectstruct[key] = [path, False, cTest.NOTFOUND]

        # Traverse results, comparing with expected
        for root, dirs, files in os.walk(rundir):
            for file in files:
                path = os.path.abspath(os.path.join(root, file))
                key = path[len(rundir) + 1:]  # remove confdir from path
                if expectstruct.has_key(key):
                    # string[] getStippedLines(string filename) {
                    def getStrippedLines(filename):
                        fp = open(filename, "U")
                        filelines = fp.readlines()
                        fp.close()

                        retlines = []
                        for line in filelines:
                            line = string.lstrip(line)
                            if len(line) != 0 and line[0] != "#":
                                retlines.append(line)
                        return retlines
                    # } // End of getStrippedLines()

                    match = True

                    if os.path.getsize(expectstruct[key][0]) < settings['diff-max-threshold']:
                        # Generate the diff between the two files, ignoring
                        # comments and blank lines
                        differ = difflib.Differ()
                        elines = getStrippedLines(path)
                        tlines = getStrippedLines(expectstruct[key][0])
                        diff = list(differ.compare(tlines, elines))

                        for line in diff:
                            if line[0] != ' ':
                                expectstruct[key][2] = cTest.DONOTMATCH
                                if settings.has_key("show-diff"):
                                    expectstruct[key][2] += "\n\n"
                                    for l in diff:
                                        expectstruct[key][2] += l
                                    expectstruct[key][2] += "\n"
                                match = False
                                break
                    else:
                        elines = getStrippedLines(path)
                        tlines = getStrippedLines(expectstruct[key][0])
                        for i in range(len(elines)):
                            if elines[i] != tlines[i]:
                                expectstruct[key][2] = cTest.DONOTMATCH
                                match = False
                                break

                    expectstruct[key][1] = match

        for key in expectstruct.keys():
            entry = expectstruct[key]
            if not entry[1]:
                self.errors.append("%s : %s" % (key, entry[2]))
                self.success = False

        # Clean up test directory
        try:
            shutil.rmtree(rundir, True)
        except (IOError, OSError):
            pass
    # } // End of cTest::runConsistencyTest()

    # void cTest::runPerformanceTest() {
    def runPerformanceTest(self, dolongtest, saveresults):
        global settings, tmpdir, CONFIGDIR, PERFDIR, TRUE_STRINGS, PERF_BASE

        if self.has_perf_base and self.skip:
            self.presult = "skipped"
            self.pdisabled = True
            return

        if self.getConfig("performance", "long", "no") in TRUE_STRINGS and not dolongtest:
            self.presult = "skipped (long)"
            self.pdisabled = True
            return

        confdir = os.path.join(self.tdir, CONFIGDIR)
        rundir = os.path.join(tmpdir, self.name)
        perfdir = os.path.join(self.tdir, PERFDIR)

        # Create test directory and populate with config
        try:
            shutil.copytree(confdir, rundir)
        except (IOError, OSError):
            self.psuccess = False
            self.presult = "error occurred creating run directory"
            return

        self.scm.deleteMetadata(rundir)

        # Run test X times, take min value
        nz = self.getConfig("main", "nonzeroexit", "disallow")
        r_times = []
        t_times = []
        for i in range(settings["perf_repeat"]):
            t_start = time.time()
            res_start = resource.getrusage(resource.RUSAGE_CHILDREN)

            # Run test app, capturing output and exitcode
            p = subprocess.Popen("cd %s; %s %s" % (rundir, self.app, self.args), shell=True,
                                 stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, close_fds=True)
            for line in p.stdout:
                pass
            exitcode = p.wait()

            res_end = resource.getrusage(resource.RUSAGE_CHILDREN)
            t_end = time.time()

            # Check exit code
            if (nz == "disallow" and exitcode != 0) or (nz == "require" and exitcode == 0):
                try:
                    shutil.rmtree(rundir, True)  # Clean up test directory
                except (IOError, OSError):
                    pass
                self.psuccess = False
                self.presult = "test app returned non-zero exit code"
                return

            r_times.append(res_end.ru_utime - res_start.ru_utime)
            t_times.append(t_end - t_start)

        # Load baseline results
        r_base = 0.0
        t_base = 0.0
        basepath = os.path.join(perfdir, PERF_BASE)
        if self.has_perf_base:
            try:
                fp = open(basepath, "r")
                line = fp.readline()
                vals = line.split(',')
                r_base = float(vals[0].strip())
                t_base = float(vals[4].strip())
                fp.close()
            except (IOError):
                self.has_perf_base = False

        r_min = min(r_times)
        r_max = max(r_times)
        r_ave = sum(r_times) / len(r_times)
        r_med = med(r_times)
        t_min = min(t_times)
        t_max = max(t_times)
        t_ave = sum(t_times) / len(t_times)
        t_med = med(t_times)

        # If no baseline results exist, write out results
        if not self.has_perf_base:
            if saveresults:
                try:
                    if not os.path.exists(perfdir):
                        os.mkdir(perfdir)
                    if not os.path.isdir(perfdir):
                        try:
                            # Clean up test directory
                            shutil.rmtree(rundir, True)
                        except (IOError, OSError):
                            pass
                        self.psuccess = False
                        self.presult = "unable to write out baseline, file exists"
                        return

                    fp = open(basepath, "w")
                    fp.write("%f,%f,%f,%f,%f,%f,%f,%f\n" % (
                        r_min, r_max, r_ave, r_med, t_min, t_max, t_ave, t_med))
                    fp.flush()
                    fp.close()
                except (IOError):
                    try:
                        shutil.rmtree(rundir, True)  # Clean up test directory
                    except (IOError, OSError):
                        pass
                    self.psuccess = False
                    self.presult = "error occurred writing baseline results"
                    return
                self.presult = "new baseline - wall time: %3.4f user time: %3.4f" % (
                    t_min, r_min)
            else:
                self.presult = "*unsaved* baseline - wall time: %3.4f user time: %3.4f" % (
                    t_min, r_min)

            try:
                shutil.rmtree(rundir, True)  # Clean up test directory
            except (IOError, OSError):
                pass
            return

        # Compare results with baseline
        r_margin = settings["perf_user_margin"] * r_base
        r_umargin = r_base + r_margin
        r_lmargin = r_base - r_margin
        r_ratio = r_min / r_base

        t_margin = settings["perf_wall_margin"] * t_base
        t_umargin = t_base + t_margin
        t_lmargin = t_base - t_margin
        t_ratio = t_min / t_base

        if r_min > r_umargin or t_min > t_umargin:
            self.psuccess = False
            self.presult = "failed"
        elif r_min < r_lmargin or t_min < t_lmargin:
            if saveresults:
                # new baseline, move old baseline and write out new results
                try:
                    rev = self.scm.getVersionString(self.tdir)
                    oname = "perf-%s-prev-%s" % (
                        time.strftime("%Y-%m-%d-%H.%M.%S"), rev)

                    shutil.move(basepath, os.path.join(perfdir, oname))

                    fp = open(basepath, "w")
                    fp.write("%f,%f,%f,%f,%f,%f,%f,%f\n" % (
                        r_min, r_max, r_ave, r_med, t_min, t_max, t_ave, t_med))
                    fp.flush()
                    fp.close()
                except (IOError, OSError, shutil.Error):
                    print "Warning: error updating '%s' performance baseline" % self.name
            self.presult = "exceeded"

        # Print output on all tests
        self.presult += "\n - wall: %2.2f  base = %3.4f  test = %3.4f" % (
            t_ratio, t_base, t_min)
        self.presult += "\n - user: %2.2f  base = %3.4f  test = %3.4f" % (
            r_ratio, r_base, r_min)

        # Clean up test directory
        try:
            shutil.rmtree(rundir, True)
        except (IOError, OSError):
            pass
    # } // End of cTest::runPerformanceTest()

    # bool cTest::handleNewExpected() {
    def handleNewExpected(self):
        global settings, EXPECTDIR

        if settings["mode"] == "slave":
            return True

        rundir = os.path.join(tmpdir, self.name)
        expectdir = os.path.join(self.tdir, EXPECTDIR)

        try:
            shutil.copytree(rundir, expectdir)
        except (IOError, OSError), e:
            return False

        for cfile in self.confstruct.keys():
            try:
                os.remove(os.path.join(expectdir, cfile))
            except OSError, e:
                print "Warning: failed to remove conf file (%s) from expected" % cfile
                print "  -- root cause: %s" % e
        try:
            shutil.rmtree(rundir, True)  # Clean up test directory
        except (IOError, OSError):
            pass

        return self.scm.addDirectory(expectdir)

    # } // End of cTest::handleNewExpected()

    # bool cTest::reportConsistencyResults() {
    def reportConsistencyResults(self):
        global settings
        print "%s :" % self.name,
        if self.success:
            if self.has_expected:
                print self.result
            else:
                if self.handleNewExpected():
                    if settings["mode"] == "slave":
                        print "skipped (no expected results)"
                    else:
                        print "new expected results generated"
                else:
                    print "unable to process new expected results"
                    self.success = False
        else:
            print "failed\n"
            if self.exitcode != 0:
                print "exit code: %d" % os.WEXITSTATUS(self.exitcode)
                print "term signal: %d" % os.WTERMSIG(self.exitcode)
            else:
                print "output variance(s):"
                for err in self.errors:
                    print err
            print "\n"

        return self.success
    # } // End of cTest::reportConsistencyResults()

    # bool cTest::getConsistencyResults() {
    def getConsistencyResults(self):
        global settings
        message = ""
        if self.success:
            message = self.result
        else:
            message = "failed\n"
            if self.exitcode != 0:
                message += "exit code: %d\n" % os.WEXITSTATUS(self.exitcode)
                message += "term signal: %d\n" % os.WTERMSIG(self.exitcode)
            else:
                message += "output variance(s):\n"
                for err in self.errors:
                    message += err + "\n"

        return (self.success, message)
    # } // End of cTest::getConsistencyResults()

    # bool cTest::reportPerformanceResults() {
    def reportPerformanceResults(self):
        print "%s : %s" % (self.name, self.presult)
        return self.psuccess
    # } // End of cTest::reportPerformanceResults()

    # bool cTest::getPerformanceResults() {
    def getPerformanceResults(self):
        return (self.psuccess, self.presult)
    # } // End of cTest::getPerformanceResults()

    # void cTest::describe() {
    def describe(self):
        if self.has_expected:
            print "  ",
        else:
            print " *",
        if self.consistency_enabled:
            print "c",
        else:
            print " ",
        if self.performance_enabled:
            print "p ",
        else:
            print "  ",
        print self.name
    # } // End of cTest::describe()

# } // End of class cTest


# Main Test Running Functions - subroutines called by main to do the testing work
# ---------------------------------------------------------------------------------------------------------------------------

# (int, int) runConsistencyTests(cTest[] tests) {
def runConsistencyTests(alltests, dolongtests):
    global settings, tmpdir

    tests = []
    for test in alltests:
        if test.isConsistencyTest():
            tests.append(test)

    if len(tests) == 0:
        print "No Consistency Tests Available (or Specified)."
        return (0, 0)

    print "\nRunning Consistency Tests:\n"

    # Run Tests
    sem = threading.BoundedSemaphore(settings["cpus"])
    ti = 0
    for test in tests:
        # void runTestWrapper(cTest test, Semaphore sem) {
        def runTestWrapper(test, sem):
            test.runConsistencyTest(dolongtests)
            sem.release()
        # } // End of runTestWrapper()

        sem.acquire()
        ti += 1
        sys.stdout.write("[% 3d of %d] %s\n" %
                         (ti, len(tests), test.name[:65]))
        sys.stdout.flush()
        tthread = threading.Thread(target=runTestWrapper, args=(test, sem))
        tthread.start()

    for i in range(settings["cpus"]):
        sem.acquire()

    sys.stdout.write("\n\n")
    sys.stdout.flush()

    # Report Results
    success = 0
    disabled = 0
    fail = 0
    for test in tests:
        if test.reportConsistencyResults():
            if test.wasConsistencySkipped():
                disabled += 1
            else:
                success += 1
        else:
            fail += 1

    return (success, disabled, fail)
# } // End of runConsistencyTests()


# (int, int) runPerformanceTests(cTest[] tests) {
def runPerformanceTests(alltests, dolongtests, force, saveresults):
    global settings, tmpdir

    tests = []
    if force:
        tests = alltests
    else:
        for test in alltests:
            if test.isPerformanceTest():
                tests.append(test)

    if len(tests) == 0:
        print "No Performance Tests Available (or Specified)."
        return (0, 0)

    print "\nRunning Performance Tests:\n"

    # Run Tests
    ti = 0
    for test in tests:
        ti += 1
        sys.stdout.write("[% 3d of %d] %s\n" %
                         (ti, len(tests), test.name[:65]))
        sys.stdout.flush()
        test.runPerformanceTest(dolongtests, saveresults)

    sys.stdout.write("\n\n")
    sys.stdout.flush()

    # Report Results
    success = 0
    disabled = 0
    fail = 0
    for test in tests:
        if test.reportPerformanceResults():
            if test.wasPerformanceSkipped():
                disabled += 1
            else:
                success += 1
        else:
            fail += 1

    return (success, disabled, fail)
# } // End of runPerformanceTests()


# Main - load configuration and call the workhorse routines
# ---------------------------------------------------------------------------------------------------------------------------

# int main(string[] argv) {
def main(argv):
    global settings, tmpdir, CONFIGDIR

    scriptdir = os.path.abspath(os.path.dirname(argv[0]))

    # Read Configuration File
    cfg = ConfigParser.ConfigParser(settings)
    cfg.read([os.path.join(scriptdir, "testrunner.cfg")])

    # getConfig - embedded function to wrap loading configuration settings w/defaults
    # -------------------------------------------------------------------------------
    # string getConfig(string sect, string opt, string default) {
    def getConfig(sect, opt, default):
        try:
            global settings
            val = cfg.get(sect, opt, False, settings)
            return val
        except:
            return default
    # } // End of getConfig()

    # Setup Global Settings
    #  - settings that begin with an underscore (i.e. _testrunner_name) are for internal use and are not intended for
    #    use as variables in test_list configuration files
    # App is defined later, since values like builddir can be modified by
    # cmdline settings
    settings["default_app"] = ""
    settings["builddir"] = getConfig("testrunner", "builddir", "build")
    settings["mode"] = getConfig("testrunner", "mode", "local")
    settings["scm"] = getConfig("testrunner", "scm", "none")

    settings["git"] = getConfig("git", "cmd", "git")
    settings["git_submodule"] = getConfig("git", "submodule", "")

    settings["svn"] = getConfig("svn", "cmd", "svn")
    settings["svnversion"] = getConfig("svn", "svnversion", "svnversion")
    settings["svnmetadir"] = getConfig("svn", "metadir", ".svn")

    settings["testdir"] = getConfig("testrunner", "testdir", "tests")

    settings["_testrunner_name"] = "testrunner.py"

    settings["perf_user_margin"] = float(
        getConfig("performance", "usermargin", .05))
    settings["perf_wall_margin"] = float(
        getConfig("performance", "wallmargin", .05))
    settings["perf_repeat"] = int(getConfig("performance", "repeat", 5))

    settings["cpus"] = 1

    settings["diff-max-threshold"] = 1536

    # Process Command Line Arguments
    try:
        opts, args = getopt.getopt(argv[1:], "fhj:lm:pg:s:v",
                                   ["diff-max-threshold=", "builddir=", "force-perf", "help", "help-test-cfg", "ignore-consistency", "list-tests", "long-tests",
                                    "mode=", "scm=", "reset-expected", "reset-perf-base", "run-perf-tests", "show-diff", "skip-tests", "git=", "svnmetadir=", "svn=", "svnversion=",
                                    "testdir=", "verbose", "version", "xml-report=", "-testrunner-name="])
    except getopt.GetoptError:
        usage()
        return -1

    # Define Option Flags
    opt_forceperf = False
    opt_ignoreconsistency = False
    opt_listtests = False
    opt_long = False
    opt_runperf = False
    opt_showhelp = False
    opt_showtestcfg = False
    opt_showversion = False

    # Process Supplied Options
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            opt_showhelp = True
        elif opt == "--builddir":
            settings["builddir"] = arg
        elif opt == "--diff-max-threshold":
            settings["diff-max-threshold"] = arg
        elif opt == "--help-test-cfg":
            opt_showtestcfg = True
        elif opt == "-j":
            cpus = int(arg)
            if cpus < 1:
                cpus = 1
            settings["cpus"] = cpus
        elif opt == "--scm":
            settings["scm"] = arg
        elif opt in ("-f", "--force-perf"):
            opt_forceperf = True
        elif opt == "--ignore-consistency":
            opt_ignoreconsistency = True
        elif opt in ("-l", "--list-tests"):
            opt_listtests = True
        elif opt == "--long-tests":
            opt_long = True
        elif opt in ("-m", "--mode"):
            settings["mode"] = arg
        elif opt == "--reset-expected":
            settings["_reset_expected"] = ""
        elif opt == "--reset-perf-base":
            settings["_reset_perf_base"] = ""
        elif opt in ("-p", "--run-perf-tests"):
            opt_runperf = True
        elif opt == "--show-diff":
            settings["show-diff"] = ""
        elif opt == "--skip-tests":
            settings["skip-tests"] = ""
        elif opt in ("-g", "--git"):
            settings["git"] = arg
        elif opt == "--svnmetadir":
            settings["svnmetadir"] = arg
        elif opt in ("-s", "--svn"):
            settings["svn"] = arg
        elif opt == "--svnversion":
            settings["svnversion"] = arg
        elif opt == "--testdir":
            settings["testdir"] = arg
        elif opt in ("-v", "--verbose"):
            settings["_verbose"] = ""
        elif opt == "--version":
            opt_showversion = True
        elif opt == "--xml-report":
            settings["xml_report"] = arg
        elif opt == "---testrunner-name":
            settings["_testrunner_name"] = arg

    # Show help or version and exit, if requested to do so
    if opt_showhelp or opt_showtestcfg or opt_showversion:
        if opt_showversion:
            version()
        if opt_showhelp:
            usage()
        if opt_showtestcfg:
            sample_test_list()
        return 0

    # Get the path to the test directory
    testdir = os.path.abspath(getConfig("main", "testdir", "."))
    settings["testdir"] = testdir

    # Re-read Configuration File with filled settings
    cfg = ConfigParser.ConfigParser(settings)
    cfg.read([os.path.join(scriptdir, "testrunner.cfg")])

    # Load the default app to test
    try:
        settings["default_app"] = os.path.abspath(cfg.get("main", "app"))
    except:
        print "Warning: No default app configured"

    if settings["scm"] == "git":
        settings["scm"] = SCMWrapper_Git()
    elif settings["scm"] == "svn":
        settings["scm"] = SCMWrapper_SVN()
    elif settings["scm"] == "none":
        settings["scm"] = SCMWrapper_None()
    else:
        print "Error: Unsupported SCM '%s'" % (settings["scm"])
        return -1

    # Load in all tests
    print "Reading Test Configurations:\n"
    tests = []

    prefix_filter = ["."]
    dlist = []
    if len(args) != 0:
        for d in dircache.listdir(testdir):
            for a in args:
                if fnmatch.fnmatch(d, a):
                    dlist.append(d)
                    break

    else:
        dlist = dircache.listdir(testdir)
        prefix_filter.append("_")

    dircache.annotate(testdir, dlist)
    for d in dlist:
        # Directories with preceeding underscore or period are ignored, as are
        # files
        if d[0] in prefix_filter or d[len(d) - 1] != "/":
            continue

        name = d[:len(d) - 1]
        curtdir = os.path.join(testdir, name)
        contents = dircache.listdir(curtdir)
        if CONFIGDIR in contents:
            test = cTest(name, curtdir)
            test.describe()
            tests.append(test)

    # If selected, display available tests and exit
    if opt_listtests:
        return 0

    # Make temp directory to hold active tests
    tmpdir = tempfile.mkdtemp("_testrunner")

    success = 0
    disabled = 0
    fail = 0

    try:
        # Run Consistency Tests
        csuccess = 0
        cdisabled = 0
        cfail = 0
        if (not opt_runperf or not opt_ignoreconsistency):
            (csuccess, cdisabled, cfail) = runConsistencyTests(tests, opt_long)
            success += csuccess
            disabled += cdisabled
            fail += cfail

        # Run Performance Tests
        psuccess = 0
        pdisabled = 0
        pfail = 0
        if (opt_ignoreconsistency or fail == 0) and opt_runperf:
            (psuccess, pdisabled, pfail) = runPerformanceTests(
                tests, opt_long, opt_forceperf, not opt_ignoreconsistency)
            success += psuccess
            disabled += pdisabled
            fail += pfail

        if settings.has_key("xml_report"):
            f = open(settings["xml_report"], "w")
            f.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
            f.write("<testsuites tests=\"%d\" failures=\"%d\" disabled=\"%d\" errors=\"0\" time=\"0\" name=\"AllTests\">\n" % (
                success + disabled + fail, fail, disabled))

            f.write("  <testsuite name=\"ConsistencyTests\" tests=\"%d\" failures=\"%d\" disabled=\"%d\" errors=\"0\" time=\"0\">\n" % (
                csuccess + cdisabled + cfail, cfail, cdisabled))
            for test in tests:
                if test.isConsistencyTest():
                    (tsuccess, message) = test.getConsistencyResults()
                    run = "run"
                    if test.wasConsistencySkipped():
                        run = "disabled"
                    if not tsuccess:
                        f.write("    <testcase name=\"%s\" status=\"%s\" time=\"0\" classname=\"ConsistencyTests\">\n" % (
                            test.getName(), run))
                        f.write(
                            "      <failure message=\"%s\" type=\"\"/>\n" % (message))
                        f.write("    </testcase>\n")
                    else:
                        f.write(
                            "    <testcase name=\"%s\" status=\"%s\" time=\"0\" classname=\"ConsistencyTests\"/>\n" % (test.getName(), run))
            f.write("  </testsuite>\n")

            if (opt_ignoreconsistency or fail == 0) and opt_runperf:
                f.write("  <testsuite name=\"PerformanceTests\" tests=\"%d\" failures=\"%d\" disabled=\"%d\" errors=\"0\" time=\"0\">\n" % (
                    psuccess + pdisabled + pfail, pfail, pdisabled))
                for test in tests:
                    if test.isPerformanceTest():
                        (tsuccess, message) = test.getPerformanceResults()
                        run = "run"
                        if test.wasPerformanceSkipped():
                            run = "disabled"
                        if not tsuccess:
                            f.write("    <testcase name=\"%s\" status=\"%s\" time=\"0\" classname=\"ConsistencyTests\">\n" % (
                                test.getName(), run))
                            f.write(
                                "      <failure message=\"%s\" type=\"\"/>\n" % (message))
                            f.write("    </testcase>\n")
                        else:
                            f.write(
                                "    <testcase name=\"%s\" status=\"%s\" time=\"0\" classname=\"ConsistencyTests\"/>\n" % (test.getName(), run))
                f.write("  </testsuite>\n")

            f.write("</testsuites>\n")
            f.close()

        # Clean up test directory
        try:
            shutil.rmtree(tmpdir, True)
        except (IOError, OSError):
            pass

        if fail == 0:
            print "\nAll tests passed."
            return 0
        else:
            if disabled != 0:
                print "\n%d of %d tests failed (%d disabled)." % (fail, fail + disabled + success, disabled)
            else:
                print "\n%d of %d tests failed." % (fail, fail + success)
            return fail
    except (KeyboardInterrupt):
        print "\nInterrupted... Terminanting Tests."

# } // End of main()


# void _main() { // Main entry point when called as standalone script
if __name__ == "__main__":
    sys.exit(main(sys.argv))
# } // End of _main()
