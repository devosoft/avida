import pySessionDumbView; reload(pySessionDumbView)
from pySessionDumbView import *

import pyAvida; reload(pyAvida)
from pyAvida import *

from AvidaCore import *
from qt import *

class pySessionDumbCtrl(pySessionDumbView):
  def __init__(self):
    pySessionDumbView.__init__(self)

  def setAvidaSlot(self, avida):
    old_avida = self.m_avida
    self.m_avida = avida
    if(old_avida):
      print "pySessionDumbCtrl.setAvidaSlot(): disconnecting..."
      self.disconnect(
        old_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      self.disconnect(
       self.m_session_mdl.m_session_mdtr, PYSIGNAL("doStartAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doStartAvidaSig"))
      self.disconnect(
        self.m_session_mdl.m_session_mdtr, PYSIGNAL("doPauseAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doPauseAvidaSig"))
      self.disconnect(
        self, PYSIGNAL("doUpdateAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))
      self.disconnect(
        self.m_session_mdl.m_session_mdtr, 
        PYSIGNAL("fromLiveCtrlUpdateAvidaSig"),
        old_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))
      del old_avida
    if(self.m_avida):
      print "pySessionDumbCtrl.setAvidaSlot(): connecting..."
      self.connect(
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("AvidaUpdatedSig"),
        self.avidaUpdatedSlot)
      self.connect(
        self.m_session_mdl.m_session_mdtr, PYSIGNAL("doStartAvidaSig"),
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("doStartAvidaSig"))
      self.connect(
        self.m_session_mdl.m_session_mdtr, PYSIGNAL("doPauseAvidaSig"),
        self.m_avida.m_avida_thread_mdtr,  PYSIGNAL("doPauseAvidaSig"))
      self.connect(
        self, PYSIGNAL("doUpdateAvidaSig"),
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))
      self.connect(
        self.m_session_mdl.m_session_mdtr, 
        PYSIGNAL("fromLiveCtrlUpdateAvidaSig"),
        self.m_avida.m_avida_thread_mdtr, PYSIGNAL("doUpdateAvidaSig"))
    
  def setupCustomMenus(self, edu_session_menu_bar_hdlr):
    self.m_debugging_menu = QPopupMenu()
    self.m_load_organism_wmi_id = self.m_debugging_menu.insertItem(
      "Load Organism",
      self, PYSIGNAL("doDebugLoadOrganismSig"))
    self.m_menu_bar.insertItem("Debugging", self.m_debugging_menu)

    self.connect(
      self, PYSIGNAL("doDebugLoadOrganismSig"),
      self.doDebugLoadOrganismSlot)

  def doLoadPetriDishConfigFileSlot(self, genesisFileName = None):
#    s = QFileDialog.getOpenFileName(
#      ".",
#      "(*.avida)",
#      None,
#      "open file dialog",
#      "Choose a file")
#    print "s:", s
    genesis = cGenesis()
    genesis.Open(cString(genesisFileName))
    if 0 == genesis.IsOpen():
      print "Warning: Unable to find file '", genesisFileName
      return
    avida = pyAvida()
    avida.construct(genesis)
    self.setAvidaSlot(avida)

    # Stops self from hearing own setAvidaSig signal

    self.disconnect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("setAvidaSig"),
      (self.m_avida,))
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
      
  def doDebugLoadOrganismSlot(self):
    if self.m_avida:
      organism_file_name = QFileDialog.getOpenFileName(
        ".",
        "(*)",
        None,
        "open file dialog",
        "Choose an organism file")
      self.m_session_mdl.m_session_mdtr.emit(
        PYSIGNAL("setDebugOrganismFileSig"),
        (organism_file_name,))
    else:
      QMessageBox.information(
        None,
        "Can't load organism file yet",
        """
This version of AvidaEd requires that a genesis file
be opened before an organism file.
"""
      )

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.sessionInitialized = False
    
    self.m_avida = None
    self.connect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)

    self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr.emit(
        PYSIGNAL("doSetupMainWindowMenuBarSig"),
        (self,))

    self.connect(
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      PYSIGNAL("doStartSig"),
      self.doStart)
    self.connect(
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      PYSIGNAL("doPauseSig"),
      self.doPause)
    self.connect(
      self.m_session_mdl.m_session_mdtr.m_edu_session_menu_bar_hdlr_mdtr,
      PYSIGNAL("doNextUpdateSig"),
      self.updatePBClickedSlot)

    self.connect(
      self.m_startpause_avida_pb, SIGNAL("clicked()"),
      self.startPausePBClickedSlot)
    self.connect(
      self.m_update_avida_pb, SIGNAL("clicked()"),
      self.updatePBClickedSlot)
      
    self.connect(
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doInitializeAvidaPhaseIISig"),
      self.doLoadPetriDishConfigFileSlot)
    self.connect(
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("fromLiveCtrlPauseAvidaSig"),
      self.doPause)
    self.connect(
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("fromLiveCtrlStartAvidaSig"),
      self.doStart)


    self.m_start_pb_text = "Start..."
    self.m_pause_pb_text = "Pause..."
    self.m_should_update = False
    self.m_control_menu.setItemVisible(self.m_next_step_cmi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_undo_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_redo_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_cut_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_copy_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_paste_emi_id, False)
    self.m_edit_menu.setItemEnabled(self.m_select_all_emi_id, False)
    self.doPause()
    self.show()

  def __del__(self):
    self.setAvidaSlot(None)
    self.disconnect(
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.setAvidaSlot)
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("setAvidaSig"),
      (None,))
      
  def doStart(self):
    if self.sessionInitialized == False:
      self.m_session_mdl.m_session_mdtr.emit(
        PYSIGNAL("doInitializeAvidaPhaseISig"),
        (self.m_session_mdl.m_tempdir,))
      self.sessionInitialized = True
    self.m_should_update = True
    self.m_startpause_avida_pb.setText(self.m_pause_pb_text)
    self.m_update_avida_pb.setEnabled(False)
    self.m_control_menu.setItemVisible(self.m_pause_cmi_id, True)
    self.m_control_menu.setItemVisible(self.m_start_cmi_id, False)
    self.m_control_menu.setItemEnabled(self.m_next_update_cmi_id, False)
    self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("doStartAvidaSig"), ())

  def doPause(self):
    self.m_should_update = False
    self.m_startpause_avida_pb.setText(self.m_start_pb_text)
    self.m_update_avida_pb.setEnabled(True)
    self.m_control_menu.setItemVisible(self.m_pause_cmi_id, False)
    self.m_control_menu.setItemVisible(self.m_start_cmi_id, True)
    self.m_control_menu.setItemEnabled(self.m_next_update_cmi_id, True)
    self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("doPauseAvidaSig"), ())

  def startPausePBClickedSlot(self):
    if True == self.m_should_update: self.doPause()
    else: self.doStart()
    
  def updatePBClickedSlot(self):
    self.emit(PYSIGNAL("doUpdateAvidaSig"), ())

  def avidaUpdatedSlot(self):
    stats = self.m_avida.m_population.GetStats()
    self.m_log_te.append(
      "UD: " + repr(stats.GetUpdate()) +
      "\t Gen: " + repr(stats.SumGeneration().Average()) +
      "\t Fit: " + repr(stats.GetAveFitness()) +
      "\t Size: " + repr(self.m_avida.m_population.GetNumOrganisms()))




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
