"""
--------------------------------------------------------------------------------
Functional-tests of AvidaGui recursive unit testing classes.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


#import pyEduMainCtrl; reload(pyEduMainCtrl)
#pyEduMainCtrl = pyEduMainCtrl.pyEduMainCtrl

import pyMdl; reload(pyMdl)
pyMdl = pyMdl.pyMdl

import unittest
from pmock import *
import qt


class TestBase(unittest.TestCase):
  def crashDummy(self):
    self.assert_(True)
    #self.assert_(False)
    print """
hi."""
    mdl = pyMdl()
    mdl.foo = 23
    mock = Mock()
    mock.expects(once()).method('__getattributex__').with(eq('bar')).will(return_value(mdl))
    print "made mock '", mock, "'."
    mdl_prime = mock.proxy().bar
    print "got mdl_prime:", mdl_prime, "compared to mdl:", mdl
    try:
      print "mock.verify():", mock.verify()
    except:
      pass

class MockTestCase(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

class RealTestCase(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass


def suite():
  test_suite = unittest.TestSuite()
  #test_cases = [MockTestCase, RealTestCase]
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
  return test_suite
