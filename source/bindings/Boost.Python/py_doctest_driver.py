import sys

library_path = sys.argv[1]
sys.path.append(library_path)

# Does what it says : import the module whose name was given as the
# second command-line argument. The module should contain a function
# 'suite()' that returns a unittests.testsuite().
module_name = sys.argv[2]
# Passing 'globals()' gives the module a copy of, among other things,
# the appended system path, so that it can find the Avida module and
# then import it.
test_module = __import__(module_name, globals(), locals())

# This asks the module to run its doctest.
test_result = test_module.run()

# test_result.wasSuccessful() returns true on success, just like you'd
# think it would, but sys.exit() by convention returns 0 to indicate
# success, so we have to invert test_result.wasSuccessful()'s return
# value.
sys.exit(test_result[0])
