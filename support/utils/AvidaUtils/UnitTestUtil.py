
def PhonyTarget(alias, action, depends = None):
  """Returns an alias to a command that performs the action.
  Implementated by a Command with a nonexistant file target.  This command will
  run on every build, and will never be considered 'up to date'. Acts like a
  'phony' target in make.
  """
  phony_file = os.path.normpath(tempfile.mktemp(prefix="phony_%s_" % alias, dir="."))
  Depends(phony_file, depends)
  return Alias(alias, Command(target=phony_file, source=None, action=action))

def Test(env, prg):
  """Creates unit test from given program.
  When unit test passes, a file stamp is made. If it ran successfully and there
  is nothing changed, the unit test can be skipped next time.

  Makes three alii: the name of program, force-test, and test-changed:
  - First can be used to run individual unit test.
  - Second is used to run changed unit tests.
  - Third forces all unit tests to run.
  """
  name = str(prg[0])
  stamp = name + '.passed'
  env.UnitTest(stamp, prg)
  alias = PhonyTarget(name, [prg], stamp)
  env.Alias('force-test', name)
  env.Alias('test-changed', stamp)
    
def TestList(env, name, list, *args, **kw):
  """Creates a set of unit tests from given list of filename bases.
    
  For each filename base 'blah', tries to make unit test using filename
  'blah.cc'.  Creates alias, of given name, to all tests in this set.
  """
  for i in list:
    Test(e, e.Program(i, i + '.cc', *args, **kw))
  Alias(name, list)

