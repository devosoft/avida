import qt

class pyMdtr(qt.QObject):
  def orphan(self):
    if self.parent(): self.parent().removeChild(self)


# Unit tests.

from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pyMdtr(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__
    # Dummy test case.
    class crashDummy(pyTestCase):
      def test(self):
        self.test_is_true(True)

    class qtSignalsChecks(pyTestCase):
      def test(self):
        # Helper function to determine whether mock object verifies.
        def checkMock(mock):
          try:
            mock.verify()
            return True
          except:
            return False

        class signalDetect(qt.QObject):
          def __init__(self):
            qt.QObject.__init__(self)
          def setSignalEndotest(self, endotest):
            self.m_endotest = endotest
          def noArgSlot(self):
            self.m_endotest.noArgSlot()
          def oneArgSlot(self, arg):
            self.m_endotest.oneArgSlot(arg)
          def twoArgSlot(self, arg1, arg2):
            self.m_endotest.twoArgSlot(arg1, arg2)
          def varArgSlot(self, *args):
            self.m_endotest.varArgSlot(*args)

        mock = Mock()
        mdtr = pyMdtr()
        signals_detect = signalDetect()
        signals_detect.setSignalEndotest(mock)

        mdtr.connect(mdtr, qt.PYSIGNAL("noArgSig"), signals_detect.noArgSlot)
        mdtr.connect(mdtr, qt.PYSIGNAL("oneArgSig"), signals_detect.oneArgSlot)
        mdtr.connect(mdtr, qt.PYSIGNAL("twoArgSig"), signals_detect.twoArgSlot)
        mdtr.connect(mdtr, qt.PYSIGNAL("varArgSig"), signals_detect.varArgSlot)

        mock.expects(once()).noArgSlot()
        mock.expects(once()).oneArgSlot(eq(1)).after("noArgSlot")
        mock.expects(once()).twoArgSlot(eq(1), eq(2)).after("oneArgSlot")
        mock.expects(once()).method("varArgSlot").with(eq("foo"), eq("bar")).after("twoArgSlot")

        mdtr.emit(qt.PYSIGNAL("noArgSig"), ())
        mdtr.emit(qt.PYSIGNAL("oneArgSig"), (1, ))
        mdtr.emit(qt.PYSIGNAL("twoArgSig"), (1, 2,))
        mdtr.emit(qt.PYSIGNAL("varArgSig"), ("foo", "bar",))

        self.test_is_true(checkMock(mock))

    self.adoptUnitTestCase(crashDummy())
    self.adoptUnitTestCase(qtSignalsChecks())
