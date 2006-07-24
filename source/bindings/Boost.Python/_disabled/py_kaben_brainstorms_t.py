
import sys
import unittest
#import getting_started1
#import getting_started2

class kabenBrainstormsBase(unittest.TestCase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

# Test class.
class kabenBrainstorm(kabenBrainstormsBase):
  def test(self):
    """
    Experimenting...
    """
    self.assert_(True)

    #print "  cTraverser instantiation..."
    #self.m_traverser = Avida.cTraverser()
    #print "  tVoidFunctorVoid factory call..."
    #self.m_functor = self.m_traverser.getVoidFunctorVoid()
    #print "  indirect functor call..."
    #self.m_traverser.callFunctor(self.m_functor)
    #print "  direct functor call..."
    #self.m_functor()
    #print "  deleting functor..."
    #del(self.m_functor)
    #print "  deleting traverser..."
    #del(self.m_traverser)


def suite():
  print """

blah.

----------------------------------------
Unit-testing some random brainstorms in python.
----------------------------------------

  """
  kabenBrainstormTestSuite = unittest.TestSuite()
  kabenBrainstormTestSuite.addTest(unittest.makeSuite(kabenBrainstorm))
  return kabenBrainstormTestSuite



