import SCons
import os

def unit_test_action(target, source, env):
  app = str(source[0].abspath)
  if os.system(app)==0: open(str(target[0]), 'w').write("PASSED\n")
  else: return 1

def generate(env):
  unit_test_builder = SCons.Builder.Builder(action = unit_test_action)
  env.AppendUnique(BUILDERS = {'UnitTest' : unit_test_builder})

def exists(env):
  return True
