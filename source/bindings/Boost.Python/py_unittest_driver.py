import sys
import unittest

for library_path in sys.argv[2:]:
  sys.path.append(library_path)

# Does what it says : import the module whose name was given as the
# second command-line argument. The module should contain a function
# 'suite()' that returns a unittests.testsuite().
module_name = sys.argv[1]
# Passing 'globals()' gives the module a copy of, among other things,
# the appended system path, so that it can find the Avida module and
# then import it.
test_module = __import__(module_name, globals(), locals())

# When this script tries to load a package's submodule for testing, the
# __import__ command above returns the top-level package, not the
# submodule. The loop below is used to extract the submodule from the
# package.
for component in module_name.split('.')[1:]:
  test_module = getattr(test_module, component)
  # This permits rewriting and rerunning the unit test module within a
  # Python shell.
  reload(test_module)

# This asks the module for its test suite, the runs it.
test_result = unittest.TextTestRunner(verbosity=2).run(test_module.suite())

# test_result.wasSuccessful() returns true on success, just like you'd
# think it would, but sys.exit() by convention returns 0 to indicate
# success, so we have to invert test_result.wasSuccessful()'s return
# value.
if __name__ == '__main__':
  sys.exit(not test_result.wasSuccessful())

# This script can be used directly from AvidaIPython, following
# executing Kaben's bs.py script, to run a unit test from the AvidaGui
# package, as in the following example:
#
#   run -n /Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/py_unittest_driver.py AvidaGui.UnitTest_pyKabenBrainstorms
