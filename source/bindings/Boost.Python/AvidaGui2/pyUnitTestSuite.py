
from py_recursive_tests_support import *
from pyTestCase import pyTestCase

class pyUnitTestSuite(pyTestCase):
  adoptUnitTestCase = pyTestCase.adoptTestCase
  def updateNamespace(self, additional_namespace):
    self.m_namespace.update(additional_namespace)
  def __init__(self, cycle_check, namespace):
    pyTestCase.__init__(self)
    self.m_namespace = namespace
    self.m_cycle_check = cycle_check
  def construct(self):
    self.adoptUnitTests()
    return self
  def adoptUnitTestSuite(self, obj):
    class_name = extractClassName(obj)
    test_suite_class = locateClassWithPrefix(class_name, ("pyUnitTestSuite_", "cUnitTestSuite_"), self.m_namespace)
    if self.m_cycle_check.recurse() and self.m_cycle_check.ckStr(class_name):
      self.adoptTestCase(test_suite_class(self.m_cycle_check, self.m_namespace).construct())
    return self
