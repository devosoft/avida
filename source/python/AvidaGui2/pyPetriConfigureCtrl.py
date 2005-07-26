# -*- coding: utf-8 -*-

from pyAvida import pyAvida
from pyFreezeDialogCtrl import pyFreezeDialogCtrl
from pyPetriConfigureView import pyPetriConfigureView
from pyWriteGenesisEvent import pyWriteGenesisEvent
from pyWriteToFreezer import pyWriteToFreezer

from AvidaCore import cGenesis, cString

from qt import *

import os, os.path, shutil
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
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("freezeDishPhaseISig"))
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezeDishPhaseIISig"), self.FreezePetriSlot)
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("FillDishSig"), self.FillDishSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doInitializeAvidaPhaseISig"), self.DisablePetriConfigureSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doEnablePetriDishSig"), self.EnablePetriConfigureSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doInitializeAvidaPhaseISig"), self.CreateFilesFromPetriSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"), 
      self.setAvidaSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doInitializeAvidaPhaseIISig"),
      self.doLoadPetriDishConfigFileSlot)
    self.connect( self, PYSIGNAL("petriDishDroppedInPopViewSig"), 
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("petriDishDroppedInPopViewSig"))
    self.ChangeMutationTextSlot()
    self.ChangeWorldSizeTextSlot()
    self.populated = False
    
  def destruct(self):
    self.m_session_petri_view = None
    self.m_avida = None
    self.full_petri_dict = {}
    self.DishDisabled = False
    self.disconnect(self.MutationSlider, SIGNAL("valueChanged(int)"), 
      self.ChangeMutationTextSlot)
    self.disconnect(self.WorldSizeSlider, SIGNAL("valueChanged(int)"), 
      self.ChangeWorldSizeTextSlot)
    self.disconnect(self.DieYesButton, SIGNAL("clicked()"), 
      self.ChangeDeathTextSlot)
    self.disconnect(self.DieNoButton, SIGNAL("clicked()"), 
      self.ChangeDeathTextSlot)
    self.disconnect(self.RadomGeneratedRadioButton, SIGNAL("clicked()"), 
      self.ChangeRandomSpinBoxSlot)
    self.disconnect(self.RandomFixedRadioButton, SIGNAL("clicked()"), 
      self.ChangeRandomSpinBoxSlot)
    self.disconnect(self.StopManuallyRadioButton, SIGNAL("clicked()"), 
      self.ChangeStopSpinBoxSlot)
    self.disconnect(self.StopAtRadioButton, SIGNAL("clicked()"), 
      self.ChangeStopSpinBoxSlot)
    self.disconnect(self.SavePetriPushButton, SIGNAL("clicked()"), 
      self.m_session_mdl.m_session_mdtr, PYSIGNAL("freezeDishPhaseISig"))
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("freezeDishPhaseIISig"), self.FreezePetriSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("FillDishSig"), self.FillDishSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doInitializeAvidaPhaseISig"), self.DisablePetriConfigureSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doEnablePetriDishSig"), self.EnablePetriConfigureSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doInitializeAvidaPhaseISig"), self.CreateFilesFromPetriSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"), 
      self.setAvidaSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("doInitializeAvidaPhaseIISig"),
      self.doLoadPetriDishConfigFileSlot)
    self.disconnect( self, PYSIGNAL("petriDishDroppedInPopViewSig"), 
      self.m_session_mdl.m_session_mdtr, 
      PYSIGNAL("petriDishDroppedInPopViewSig"))
    self.populated = False
    self.m_session_mdl = None

  
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
    self.m_session_mdl.new_empty_dish = True
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("finishedPetriDishSig"), ())

       
  def DisablePetriConfigureSlot(self):

    # Turn off the controls 

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

  def EnablePetriConfigureSlot(self):

    # Turn on the controls 
    
    self.AncestorComboBox.setEnabled(True)
    self.StopAtSpinBox.setEnabled(True)
    self.StopManuallyRadioButton.setEnabled(True)
    self.StopAtRadioButton.setEnabled(True)
    self.WorldSizeSlider.setEnabled(True)
    self.RandomSpinBox.setEnabled(True)
    self.RadomGeneratedRadioButton.setEnabled(True)
    self.RandomFixedRadioButton.setEnabled(True)
    self.MutationSlider.setEnabled(True)
    self.LocalBirthRadioButton.setEnabled(True)
    self.MassActionRadioButton.setEnabled(True)
    self.LifeSpanSpinBox.setEnabled(True)
    self.DieNoButton.setEnabled(True)
    self.DieYesButton.setEnabled(True)
    self.MutationPercentTextLabel.setEnabled(True)
    self.WorldSizeTextLabel.setEnabled(True)
    self.MutationRateHeadTextLabel.setEnabled(True)
    self.WorldSizeHeadTextLable.setEnabled(True)
    self.DeathLabel.setEnabled(True)
    self.RandomHeadTextLabel.setEnabled(True)
    self.AncestorHeadTextLabel.setEnabled(True)
    self.BirthHeadTextLabel.setEnabled(True)
    self.StopHeadTextLabel.setEnabled(True)
    self.DeathTextLabel2.setEnabled(True)
    self.DeathTextLabel3.setEnabled(True)
    self.DishDisabled = False


  def CreateFilesFromPetriSlot(self, out_dir = None):

    # The input files will be placed in a python generated temporary directory
    # ouput files will be stored in tmp_dir/output until the data is frozen
    # (ie saved)

    self.full_petri_dict["SETTINGS"] = self.Form2Dictionary()
    write_object = pyWriteGenesisEvent(self.full_petri_dict, 
      self.m_session_mdl.m_current_workspace,
      self.m_session_mdl.m_current_freezer, self.m_session_mdl.m_tempdir,
      self.m_session_mdl.m_tempdir_out)
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("doInitializeAvidaPhaseIISig"), (os.path.join(self.m_session_mdl.m_tempdir, "genesis.avida"),))
      
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
    file_name_len = len(file_name.rstrip())

    # If the user is saving a full population expand the name and insert
    # the population dictionary into the temporary dictionary

    if (file_name_len > 0):
      is_empty_dish = m_pop_up_freezer_file_name.isEmpty()
      if (not is_empty_dish):
        os.mkdir(file_name)

        # Copy the average and count files from the teporary output directory
        # to the Freezer directory
        
        tmp_ave_file = os.path.join(self.m_session_mdl.m_tempdir_out, "average.dat")
        if (os.path.exists(tmp_ave_file)):
          shutil.copyfile(tmp_ave_file, os.path.join(file_name, "average.dat"))
        tmp_count_file = os.path.join(self.m_session_mdl.m_tempdir_out, "count.dat")
        if (os.path.exists(tmp_count_file)):
          shutil.copyfile(tmp_count_file, os.path.join(file_name, "count.dat"))
        file_name = os.path.join(file_name, "petri_dish")
        tmp_dict["POPULATION"] = population_dict
      freezer_file = pyWriteToFreezer(tmp_dict, file_name)
      if (is_empty_dish):
        self.m_session_mdl.saved_empty_dish = True
      else:
        self.m_session_mdl.saved_full_dish = True
    
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("doRefreshFreezerInventorySig"), ())
    if send_reset_signal:
      print "sending reset signal from pyPetriConfigureCtrl:FreezePetriSlot" 
      self.m_session_mdl.m_session_mdtr.emit(
        PYSIGNAL("restartPopulationSig"), (self.m_session_mdl, ))

    # If the send_quit_signal flag was sent to this routine kill the application
    # (Instead of killing the application directly a signal should be sent
    # upto the workspace moderator)

    if send_quit_signal:
      qApp.quit()


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
      if os.path.exists(str(freezer_item_name)) == False:
        print "that was not a valid path (2)" 
      else: 
        self.emit(PYSIGNAL("petriDishDroppedInPopViewSig"), (e,))
        print "emitted(1)"
