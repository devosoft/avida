# -*- coding: utf-8 -*-

from qt import *
from pyPetriConfigureView import pyPetriConfigureView
from pyWriteGenesis import pyWriteGenesis
from pyFreezeDialogCtrl import pyFreezeDialogCtrl


class pyPetriConfigureCtrl(pyPetriConfigureView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyPetriConfigureView.__init__(self,parent,name,fl)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_session_petri_view = pyPetriConfigureView()
    self.connect(self.MutationSlider, SIGNAL("valueChanged(int)"), 
      self.ChangeMutationTextSlot)
    self.connect(self.WorldSizeSlider, SIGNAL("valueChanged(int)"), 
      self.ChangeWorldSizeTextSlot)
    self.connect(self.DieYesButton, SIGNAL("clicked()"), 
      self.ChangeDeathTextSlot)
    self.connect(self.DieNoButton, SIGNAL("clicked()"), 
      self.ChangeDeathTextSlot)
    self.connect(self.RadomGeneratedRadioButton, SIGNAL("clicked()"), 
      self.ChangeRandomSpinBoxSlot)
    self.connect(self.RandomFixedRadioButton, SIGNAL("clicked()"), 
      self.ChangeRandomSpinBoxSlot)
    self.connect(self.StopManuallyRadioButton, SIGNAL("clicked()"), 
      self.ChangeStopSpinBoxSlot)
    self.connect(self.StopAtRadioButton, SIGNAL("clicked()"), 
      self.ChangeStopSpinBoxSlot)
    self.connect(self.SavePetriPushButton, SIGNAL("clicked()"), 
      PYSIGNAL("freezeDishPhaseISig"))
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("freezeDishPhaseIISig"), 
      self.FreezePetriSlot)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doDefrostDishSig"), self.FillDishSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("doInitializeAvidaPhaseISig"),
      self.DisablePetriConfigureSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("doInitializeAvidaPhaseISig"),
      self.CreateFilesFromPetriSlot)
    self.populated = False
    self.run_started = False    

  def ChangeMutationTextSlot(self):
    slide_value_txt = str(float(self.MutationSlider.value())/10.0) + "%"
    self.MutationPercentTextLabel.setText(slide_value_txt)

  def ChangeWorldSizeTextSlot(self):
    slide_value = str(self.WorldSizeSlider.value())
    slide_value_txt = slide_value + " x " + slide_value + " cells"
    self.WorldSizeTextLabel.setText(slide_value_txt)

  def ChangeDeathTextSlot(self):
    if self.DieYesButton.isChecked() == False:
      self.DeathTextLabel2.setEnabled(False)
      self.DeathTextLabel3.setEnabled(False)
      self.LifeSpanSpinBox.setEnabled(False)
    else:
      self.DeathTextLabel2.setEnabled(True)
      self.DeathTextLabel3.setEnabled(True)
      self.LifeSpanSpinBox.setEnabled(True)

  def ChangeRandomSpinBoxSlot(self):
    if self.RadomGeneratedRadioButton.isChecked() == True:
      self.RandomSpinBox.setEnabled(False)
    else:
      self.RandomSpinBox.setEnabled(True)
  
  def ChangeStopSpinBoxSlot(self):
    if self.StopManuallyRadioButton.isChecked() == True:
      self.StopAtSpinBox.setEnabled(False)
    else:
      self.StopAtSpinBox.setEnabled(True)
  
  def FillDishSlot(self, petri_info):
    settings_info =  petri_info.dictionary["SETTINGS"]
    self.AncestorComboBox.removeItem (0)
    start_creature = settings_info["START_CREATURE"]
    self.AncestorComboBox.insertItem(start_creature)
    max_updates = int(settings_info["MAX_UPDATES"])
    self.StopAtSpinBox.setValue(max_updates)
    if max_updates < 0:
       self.StopManuallyRadioButton.setChecked(True)
       self.StopAtRadioButton.setChecked(False)
    else:
       self.StopManuallyRadioButton.setChecked(False)
       self.StopAtRadioButton.setChecked(True)
    self.WorldSizeSlider.setValue(int(settings_info["WORLD-X"]))
    seed = int(settings_info["RANDOM_SEED"])
    self.RandomSpinBox.setValue(seed)
    if seed == 0:
       self.RadomGeneratedRadioButton.setChecked(True)
       self.RandomFixedRadioButton.setChecked(False)
    else:
       self.RadomGeneratedRadioButton.setChecked(False)
       self.RandomFixedRadioButton.setChecked(True)
    copy_mutation_percent = float(settings_info["COPY_MUT_PROB"]) * 100;
    self.MutationSlider.setValue(int(copy_mutation_percent * 10))
    if int(settings_info["BIRTH_METHOD"]) in [0, 1, 2, 3]:
       self.LocalBirthRadioButton.setChecked(True)
       self.MassActionRadioButton.setChecked(False)
    else:
       self.LocalBirthRadioButton.setChecked(False)
       self.MassActionRadioButton.setChecked(True)
    self.LifeSpanSpinBox.setValue(int(settings_info["AGE_LIMIT"]))
    if int(settings_info["DEATH_METHOD"]) == 0:
       self.DieNoButton.setChecked(True)
       self.DieYesButton.setChecked(False)
       self.DeathTextLabel2.setEnabled(False)
       self.DeathTextLabel3.setEnabled(False)
       self.LifeSpanSpinBox.setEnabled(False)
    else:
       self.DieNoButton.setChecked(False)
       self.DieYesButton.setChecked(True)
       self.DeathTextLabel2.setEnabled(True)
       self.DeathTextLabel3.setEnabled(True)
       self.LifeSpanSpinBox.setEnabled(True)
       

  def DisablePetriConfigureSlot(self):
    print "called DisablePetriConfigureSlot"
    self.run_started = False
    self.AncestorComboBox.setEnabled(False)
    self.StopAtSpinBox.setEnabled(False)
    self.StopManuallyRadioButton.setEnabled(False)
    self.StopAtRadioButton.setEnabled(False)
    self.WorldSizeSlider.setEnabled(False)
    self.RandomSpinBox.setEnabled(False)
    self.RadomGeneratedRadioButton.setEnabled(False)
    self.RandomFixedRadioButton.setEnabled(False)
    self.MutationSlider.setEnabled(False)
    self.LocalBirthRadioButton.setEnabled(False)
    self.MassActionRadioButton.setEnabled(False)
    self.LifeSpanSpinBox.setEnabled(False)
    self.DieNoButton.setEnabled(False)
    self.DieYesButton.setEnabled(False)
    self.MutationPercentTextLabel.setEnabled(False)
    self.WorldSizeTextLabel.setEnabled(False)
    self.MutationRateHeadTextLabel.setEnabled(False)
    self.WorldSizeHeadTextLable.setEnabled(False)
    self.DeathLabel.setEnabled(False)
    self.RandomHeadTextLabel.setEnabled(False)
    self.AncestorHeadTextLabel.setEnabled(False)
    self.BirthHeadTextLabel.setEnabled(False)
    self.StopHeadTextLabel.setEnabled(False)
    self.DeathTextLabel2.setEnabled(False)
    self.DeathTextLabel3.setEnabled(False)

  def CreateFilesFromPetriSlot(self, out_dir = None):
    tmp_dict = self.Form2Dictionary()
    write_object = pyWriteGenesis(tmp_dict, "freezer/", "test/")
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("doInitializeAvidaPhaseIISig"), ("test/genesis.avida",))
      
  def Form2Dictionary(self):
    tmp_dict = {}
    settings_info = {}
    
    settings_info["START_CREATURE"] = str(self.AncestorComboBox.text(0))
    if (self.StopAtRadioButton.isChecked() == True):
      settings_info["MAX_UPDATES"] = self.StopAtSpinBox.value()
    else:
      settings_info["MAX_UPDATES"] = -1
    settings_info["WORLD-X"] = self.WorldSizeSlider.value()
    settings_info["WORLD-Y"] = self.WorldSizeSlider.value()
    if self.RandomFixedRadioButton.isChecked() == True:
      settings_info["RANDOM_SEED"] = self.RandomSpinBox.value()
    else:
      settings_info["RANDOM_SEED"] = 0
    settings_info["COPY_MUT_PROB"] = float(self.MutationSlider.value())/1000.0
    if self.LocalBirthRadioButton.isChecked() == True:
      settings_info["BIRTH_METHOD"] = 0
    else:
      settings_info["BIRTH_METHOD"] = 0
    settings_info["AGE_LIMIT"] = self.LifeSpanSpinBox.value()
    if self.DieNoButton.isChecked() == True:
      settings_info["DEATH_METHOD"] = 0
    else:
      settings_info["DEATH_METHOD"] = 2
    tmp_dict["SETTINGS"] = settings_info
    return tmp_dict
    
  def FreezePetriSlot(self, freeze_dir = None, population_dict = None):
    m_pop_up_freezer_file_name = pyFreezeDialogCtrl()
    file_name = m_pop_up_freezer_file_name.showDialog("freezer/")
    isEmpty = m_pop_up_freezer_file_name.isEmpty()
    print str(m_pop_up_freezer_file_name.FileNameLineEdit.text())
#    print "Name = " + str(m_pop_up_freezer_file_name.FileNameLineEdit.text())
#    print "Empty? = " + str (m_pop_up_freezer_file_name.EmptyRadioButton.isChecked())
#    print "Result = " + str(m_pop_up_freezer_file_name.result())
    tmp_dict = self.Form2Dictionary()
    tmp_dict["POPULATION"] = population_dict
    print tmp_dict
    