import pySessionDumbView; reload(pySessionDumbView)
from pySessionDumbView import *

import qt

class pySessionDumbCtrl(qt.QObject):
  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_session_dumb_view = pySessionDumbView()

    self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr.emit(
        qt.PYSIGNAL("doSetupMainWindowMenuBarSig"), (self.m_session_dumb_view,))

    self.connect(self.m_session_dumb_view.m_startpause_avida_pb, qt.SIGNAL("clicked()"), self.startPausePBClickedSlot)
    self.connect(self.m_session_dumb_view.m_update_avida_pb, qt.SIGNAL("clicked()"), self.updatePBClickedSlot)

    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doStartSig"), self.doStart)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doPauseSig"), self.doPause)
    self.connect(self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      qt.PYSIGNAL("doNextUpdateSig"), self.updatePBClickedSlot)

    self.connect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr,
      qt.PYSIGNAL("AvidaUpdatedSig"), self.avidaUpdatedSlot)

    self.m_start_pb_text = "Start..."
    self.m_pause_pb_text = "Pause..."
    self.m_should_update = False
    self.m_session_dumb_view.m_control_menu.setItemVisible(self.m_session_dumb_view.m_next_step_cmi_id, False)
    self.m_session_dumb_view.m_edit_menu.setItemEnabled(self.m_session_dumb_view.m_undo_emi_id, False)
    self.m_session_dumb_view.m_edit_menu.setItemEnabled(self.m_session_dumb_view.m_redo_emi_id, False)
    self.m_session_dumb_view.m_edit_menu.setItemEnabled(self.m_session_dumb_view.m_cut_emi_id, False)
    self.m_session_dumb_view.m_edit_menu.setItemEnabled(self.m_session_dumb_view.m_copy_emi_id, False)
    self.m_session_dumb_view.m_edit_menu.setItemEnabled(self.m_session_dumb_view.m_paste_emi_id, False)
    self.m_session_dumb_view.m_edit_menu.setItemEnabled(self.m_session_dumb_view.m_select_all_emi_id, False)
    self.doPause()
    self.m_session_dumb_view.show()

  def destruct(self):
    print("pySessionDumbCtrl.destruct()...")
    self.disconnect(self.m_session_dumb_view.m_startpause_avida_pb,
      qt.SIGNAL("clicked()"), self.startPausePBClickedSlot)
    print("pySessionDumbCtrl.destruct()...1")
    self.disconnect(self.m_session_dumb_view.m_update_avida_pb,
      qt.SIGNAL("clicked()"), self.updatePBClickedSlot)
    print("pySessionDumbCtrl.destruct()...2")
    self.disconnect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr,
      qt.PYSIGNAL("AvidaUpdatedSig"), self.avidaUpdatedSlot)
    print("pySessionDumbCtrl.destruct()...3")
    del self.m_session_mdl
    print("pySessionDumbCtrl.destruct()...4")
    del self.m_session_dumb_view.m_menu_bar
    del self.m_session_dumb_view
    print("pySessionDumbCtrl.destruct() done.")

  def __del__(self):
    print("pySessionDumbCtrl.__del__()...")
    self.destruct()
    print("pySessionDumbCtrl.__del__() done.")

  def doStart(self):
    self.m_should_update = True
    self.m_session_dumb_view.m_startpause_avida_pb.setText(self.m_pause_pb_text)
    self.m_session_dumb_view.m_update_avida_pb.setEnabled(False)
    self.m_session_dumb_view.m_control_menu.setItemVisible(self.m_session_dumb_view.m_pause_cmi_id, True)
    self.m_session_dumb_view.m_control_menu.setItemVisible(self.m_session_dumb_view.m_start_cmi_id, False)
    self.m_session_dumb_view.m_control_menu.setItemEnabled(self.m_session_dumb_view.m_next_update_cmi_id, False)
    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("doStartAvidaSig"), ())

  def doPause(self):
    self.m_should_update = False
    self.m_session_dumb_view.m_startpause_avida_pb.setText(self.m_start_pb_text)
    self.m_session_dumb_view.m_update_avida_pb.setEnabled(True)
    self.m_session_dumb_view.m_control_menu.setItemVisible(self.m_session_dumb_view.m_pause_cmi_id, False)
    self.m_session_dumb_view.m_control_menu.setItemVisible(self.m_session_dumb_view.m_start_cmi_id, True)
    self.m_session_dumb_view.m_control_menu.setItemEnabled(self.m_session_dumb_view.m_next_update_cmi_id, True)
    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("doPauseAvidaSig"), ())

  def startPausePBClickedSlot(self):
    if True == self.m_should_update:
      self.doPause()
    else:
      self.doStart()
    
  def updatePBClickedSlot(self):
    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("doUpdateAvidaSig"), ())

  def avidaUpdatedSlot(self):
    #stats = self.m_session_mdl.m_avida_threaded_driver.GetPopulation().GetStats()
    stats = self.m_session_mdl.m_population.GetStats()
    self.m_session_dumb_view.m_log_te.append(
      "UD: " + repr(stats.GetUpdate()) +
      "\t Gen: " + repr(stats.SumGeneration().Average()) +
      "\t Fit: " + repr(stats.GetAveFitness()) +
      #"\t Size: " + repr(self.m_session_mdl.m_avida_threaded_driver.GetPopulation().GetNumOrganisms()))
      "\t Size: " + repr(self.m_session_mdl.m_population.GetNumOrganisms()))




# Unit tests.

from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pySessionDumbCtrl(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__
    print """
    FIXME: pySessionDumbCtrl
    pySessionDumbCtrl has no unit tests.
    """
    self.adoptUnitTestSuite("pyMdtr")

    # Dummy test case.
    class crashDummy(pyTestCase):
      def test(self):
        self.test_is_true(True)
    self.adoptUnitTestCase(crashDummy())
