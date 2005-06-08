# -*- coding: utf-8 -*-

from pyAvida import pyAvida
from pyFreezeDialogCtrl import pyFreezeDialogCtrl
from pyPetriConfigureView import pyPetriConfigureView
from pyWriteGenesis import pyWriteGenesis
from pyWriteToFreezer import pyWriteToFreezer

from AvidaCore import cGenesis, cString

from qt import *

import os
import math

class pyPetriConfigureCtrl(pyPetriConfigureView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyPetriConfigureView.__init__(self,parent,name,fl)
    self.setAcceptDrops(1)


  def setAvidaSlot(self, avida):
    old_avida = self.m_avida
    self.m_avida = avida
    if old_avida:
      del old_avida
    if self.m_avida:
      pass
    
  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_session_petri_view = pyPetriConfigureView()
    self.m_avida = None
    self.full_petri_dict = {}
    self.DishDisabled = False
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
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"), self.setAvidaSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("doInitializeAvidaPhaseIISig"),
      self.doLoadPetriDishConfigFileSlot)
    self.connect( self, PYSIGNAL("petriDishDroppedInPopViewSig"), self.m_session_mdl.m_session_mdtr, PYSIGNAL("petriDishDroppedInPopViewSig"))
    self.ChangeMutationTextSlot()
    self.ChangeWorldSizeTextSlot()
    self.populated = False
    self.run_started = False    

  def ChangeMutationTextSlot(self):
    slide_value = float(self.MutationSlider.value())/100.0
    slide_value = pow(10,slide_value)
    if slide_value < 0.0011:
      slide_value = 0.0
    if slide_value > 1 or slide_value < 0.00001:
      slide_value_txt = ("%1.1f" % (slide_value)) + "%"
    elif slide_value > 0.1:
      slide_value_txt = ("%1.2f" % (slide_value)) + "%"
    elif slide_value > 0.01:
      slide_value_txt = ("%1.3f" % (slide_value)) + "%"
    else:
      slide_value_txt = ("%1.4f" % (slide_value)) + "%"
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
  
  def FillDishSlot(self, dish_name, petri_dict):
    
    # Stop from filling the petri dish if the dish is disabled

    if self.DishDisabled:
      return
    self.full_petri_dict = petri_dict.dictionary
    settings_dict =  petri_dict.dictionary["SETTINGS"]
    self.AncestorComboBox.removeItem (0)
    start_creature = settings_dict["START_CREATURE"]
    self.AncestorComboBox.insertItem(start_creature)
    max_updates = int(settings_dict["MAX_UPDATES"])
    self.StopAtSpinBox.setValue(max_updates)
    if max_updates < 0:
       self.StopManuallyRadioButton.setChecked(True)
       self.StopAtRadioButton.setChecked(False)
    else:
       self.StopManuallyRadioButton.setChecked(False)
       self.StopAtRadioButton.setChecked(True)
    self.WorldSizeSlider.setValue(int(settings_dict["WORLD-X"]))
    seed = int(settings_dict["RANDOM_SEED"])
    self.RandomSpinBox.setValue(seed)
    if seed == 0:
       self.RadomGeneratedRadioButton.setChecked(True)
       self.RandomFixedRadioButton.setChecked(False)
    else:
       self.RadomGeneratedRadioButton.setChecked(False)
       self.RandomFixedRadioButton.setChecked(True)
    copy_mutation_percent = float(settings_dict["COPY_MUT_PROB"]) * 100;
    if copy_mutation_percent > 0.00000001:
      self.MutationSlider.setValue(int(math.log10(copy_mutation_percent) * 100))
    else:
      self.MutationSlider.setValue(-300)
    if int(settings_dict["BIRTH_METHOD"]) in [0, 1, 2, 3]:
       self.LocalBirthRadioButton.setChecked(True)
       self.MassActionRadioButton.setChecked(False)
    else:
       self.LocalBirthRadioButton.setChecked(False)
       self.MassActionRadioButton.setChecked(True)
    self.LifeSpanSpinBox.setValue(int(settings_dict["AGE_LIMIT"]))
    if int(settings_dict["DEATH_METHOD"]) == 0:
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
    self.DishDisabled = True
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("doDisablePetriDishSig"), ())

  def CreateFilesFromPetriSlot(self, out_dir = None):
    self.full_petri_dict["SETTINGS"] = self.Form2Dictionary()
    write_object = pyWriteGenesis(self.full_petri_dict, 
      self.m_session_mdl.m_current_workspace,
      self.m_session_mdl.m_current_freezer, self.m_session_mdl.m_tempdir)
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("doInitializeAvidaPhaseIISig"), (self.m_session_mdl.m_tempdir + "genesis.avida",))
      
  def Form2Dictionary(self):
    settings_dict = {}
    
    settings_dict["START_CREATURE"] = str(self.AncestorComboBox.text(0))
    if (self.StopAtRadioButton.isChecked() == True):
      settings_dict["MAX_UPDATES"] = self.StopAtSpinBox.value()
    else:
      settings_dict["MAX_UPDATES"] = -1
    settings_dict["WORLD-X"] = self.WorldSizeSlider.value()
    settings_dict["WORLD-Y"] = self.WorldSizeSlider.value()
    if self.RandomFixedRadioButton.isChecked() == True:
      settings_dict["RANDOM_SEED"] = self.RandomSpinBox.value()
    else:
      settings_dict["RANDOM_SEED"] = 0
    slide_value = float(self.MutationSlider.value())/100.0
    slide_value = pow(10,slide_value)
    if slide_value < 0.0011:
      slide_value = 0.0
    settings_dict["COPY_MUT_PROB"] = slide_value/100.0
    if self.LocalBirthRadioButton.isChecked() == True:
      settings_dict["BIRTH_METHOD"] = 0
    else:
      settings_dict["BIRTH_METHOD"] = 4
    settings_dict["AGE_LIMIT"] = self.LifeSpanSpinBox.value()
    if self.DieNoButton.isChecked() == True:
      settings_dict["DEATH_METHOD"] = 0
    else:
      settings_dict["DEATH_METHOD"] = 2
    return settings_dict
    
  def FreezePetriSlot(self, population_dict = None, 
      send_reset_signal = False, send_quit_signal = False):
    tmp_dict = {}
    tmp_dict["SETTINGS"] = self.Form2Dictionary()
    m_pop_up_freezer_file_name = pyFreezeDialogCtrl()
    file_name = m_pop_up_freezer_file_name.showDialog(self.m_session_mdl.m_current_freezer)
    if (m_pop_up_freezer_file_name.isEmpty() == False):
      os.mkdir(file_name)
      file_name = file_name + "/petri_dish"
      tmp_dict["POPULATION"] = population_dict
    is_empty_dish = m_pop_up_freezer_file_name.EmptyRadioButton.isChecked()
    freezer_file = pyWriteToFreezer(tmp_dict, is_empty_dish, file_name)
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("doRefreshFreezerInventorySig"), ())
    if send_reset_signal:
      print "sending reset signal from pyPetriConfigureCtrl:FreezePetriSlot" 
    if send_quit_signal:
      print "sending quit signal from pyPetriConfigureCtrl:FreezePetriSlot"
      self.m_session_mdl.m_session_mdtr.emit(
        PYSIGNAL("quitAvidaPhaseIISig"), ())

  def doLoadPetriDishConfigFileSlot(self, genesisFileName = None):
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
      
  def dropEvent( self, e ):
    freezer_item_name = QString()
    print "dropEvent"
    if ( QTextDrag.decode( e, freezer_item_name ) ) :
      print "in here"
      print freezer_item_name
#      if os.path.exists(str('default.workspace/freezer/' + str(string) + '.full/')) == False:
      if os.path.exists(str(freezer_item_name)) == False:
        print "that was not a valid path (2)" 
      else: 
        self.emit(PYSIGNAL("petriDishDroppedInPopViewSig"), (e,))
        print "emitted(1)"