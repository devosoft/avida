
import inspect
import types

def extractClassName(obj):
  class_name = None
  if type(obj) == types.StringType: class_name = obj
  elif type(obj) == types.ClassType: class_name = obj.__name__
  elif type(obj) == types.InstanceType: class_name = obj.__class__.__name__
  else: raise TypeError(str(type(obj)))
  return class_name

def locateClassWithPrefix(class_name, prefixes, namespace):
  # check that this works with both locally and globally defined test suite classes.
    # it doesn't work with locals. using locals() causes a memory leak apparently lasting the lifetime of this module.
  class_name_stack = [class_name]
  while 0 < len(class_name_stack):
    name = class_name_stack.pop()
    for prefix in prefixes:
      name_with_prefix = "%s%s" % (prefix, name)
      if namespace.has_key(name_with_prefix):
        class_with_prefix = namespace[name_with_prefix]
        if inspect.isclass(class_with_prefix):
          return class_with_prefix
    if namespace.has_key(name):
      klass = namespace[name]
      if type(klass) == types.ClassType:
        class_name_stack.extend([base.__name__ for base in klass.__bases__])
  raise NameError("Couldn't find class with prefix for class name '%s'." % class_name)
