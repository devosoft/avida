# -*- coding: utf-8 -*-

from AvidaCore import cConfig
from qt import *
from pyPetriConfigureView import pyPetriConfigureView


class pyPetriConfigureCtrl(pyPetriConfigureView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyPetriConfigureView.__init__(self,parent,name,fl)
    self.connect(self.MutationSlider, SIGNAL("valueChanged(int)"), 
      self.ChangeMutationText)
    self.connect(self.WorldSizeSlider, SIGNAL("valueChanged(int)"), 
      self.ChangeWorldSizeText)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_session_petri_view = pyPetriConfigureView()
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doDefrostDishSig"), self.FillDishSlot)
    self.connect(self.m_session_mdl.m_session_mdtr, PYSIGNAL("setAvidaSig"),
      self.DisablePetriConfigureSlot)
    self.populated = False
    self.run_started = False

  def ChangeMutationText(self):
    slide_value_txt = str(float(self.MutationSlider.value())/10.0) + "%"
    self.MutationPercentTextLabel.setText(slide_value_txt)

  def ChangeWorldSizeText(self):
    slide_value = str(self.WorldSizeSlider.value())
    slide_value_txt = slide_value + " x " + slide_value + " cells"
    self.WorldSizeTextLabel.setText(slide_value_txt)

  def FillDishSlot(self, petri_info):
    print "called FillDish"
    print petri_info.file_name
    settings_info =  petri_info.dictionary["SETTINGS"]
    self.AncestorComboBox.removeItem (0)
    start_creature = settings_info["START_CREATURE"]
    self.AncestorComboBox.insertItem(start_creature[0])
    max_updates = int(settings_info["MAX_UPDATES"][0])
    self.StopAtSpinBox.setValue(max_updates)
    if max_updates < 0:
       self.StopManuallyRadioButton.setChecked(True)
       self.StopAtRadioButton.setChecked(False)
    else:
       self.StopManuallyRadioButton.setChecked(False)
       self.StopAtRadioButton.setChecked(True)
    self.WorldSizeSlider.setValue(int(settings_info["WORLD-X"][0]))
    seed = int(settings_info["RANDOM_SEED"][0])
    self.RandomSpinBox.setValue(seed)
    if seed == 0:
       self.RadomGeneratedRadioButton.setChecked(True)
       self.RandomFixedRadioButton.setChecked(False)
    else:
       self.RadomGeneratedRadioButton.setChecked(False)
       self.RandomFixedRadioButton.setChecked(True)
    copy_mutation_percent = float(settings_info["COPY_MUT_PROB"][0]) * 100;
    self.MutationSlider.setValue(int(copy_mutation_percent * 10))
    if int(settings_info["BIRTH_METHOD"][0]) in [0, 1, 2, 3]:
       self.LocalBirthRadioButton.setChecked(True)
       self.MassActionRadioButton.setChecked(False)
    else:
       self.LocalBirthRadioButton.setChecked(False)
       self.MassActionRadioButton.setChecked(True)
    self.LifeSpanSpinBox.setValue(int(settings_info["AGE_LIMIT"][0]))
    if int(settings_info["DEATH_METHOD"][0]) == 0:
       self.DieNoButton.setChecked(True)
       self.DieYesButton.setChecked(False)
    else:
       self.DieNoButton.setChecked(False)
       self.DieYesButton.setChecked(True)

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
