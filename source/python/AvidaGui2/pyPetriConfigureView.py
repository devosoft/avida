# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyPetriConfigureView.ui'
#
# Created: Mon Mar 14 14:51:31 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.14
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyPetriConfigureView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyPetriConfigureView")

        f = QFont(self.font())
        f.setPointSize(9)
        self.setFont(f)
        self.setAcceptDrops(1)

        pyPetriConfigureViewLayout = QVBoxLayout(self,11,6,"pyPetriConfigureViewLayout")

        layout79 = QHBoxLayout(None,0,1,"layout79")

        layout76 = QVBoxLayout(None,0,6,"layout76")

        self.MutationRateHeadTextLabel = QLabel(self,"MutationRateHeadTextLabel")
        MutationRateHeadTextLabel_font = QFont(self.MutationRateHeadTextLabel.font())
        MutationRateHeadTextLabel_font.setBold(1)
        self.MutationRateHeadTextLabel.setFont(MutationRateHeadTextLabel_font)
        self.MutationRateHeadTextLabel.setAlignment(QLabel.AlignCenter)
        layout76.addWidget(self.MutationRateHeadTextLabel)

        layout75 = QHBoxLayout(None,0,6,"layout75")

        self.MutationSlider = QSlider(self,"MutationSlider")
        MutationSlider_font = QFont(self.MutationSlider.font())
        self.MutationSlider.setFont(MutationSlider_font)
        self.MutationSlider.setMaxValue(1000)
        self.MutationSlider.setValue(500)
        self.MutationSlider.setOrientation(QSlider.Horizontal)
        layout75.addWidget(self.MutationSlider)

        self.MutationPercentTextLabel = QLabel(self,"MutationPercentTextLabel")
        MutationPercentTextLabel_font = QFont(self.MutationPercentTextLabel.font())
        self.MutationPercentTextLabel.setFont(MutationPercentTextLabel_font)
        layout75.addWidget(self.MutationPercentTextLabel)
        layout76.addLayout(layout75)
        layout79.addLayout(layout76)

        layout78 = QVBoxLayout(None,0,6,"layout78")

        self.WorldSizeHeadTextLable = QLabel(self,"WorldSizeHeadTextLable")
        WorldSizeHeadTextLable_font = QFont(self.WorldSizeHeadTextLable.font())
        WorldSizeHeadTextLable_font.setBold(1)
        self.WorldSizeHeadTextLable.setFont(WorldSizeHeadTextLable_font)
        self.WorldSizeHeadTextLable.setTextFormat(QLabel.AutoText)
        self.WorldSizeHeadTextLable.setAlignment(QLabel.WordBreak | QLabel.AlignCenter)
        layout78.addWidget(self.WorldSizeHeadTextLable)

        layout77 = QHBoxLayout(None,0,6,"layout77")

        self.WorldSizeSlider = QSlider(self,"WorldSizeSlider")
        WorldSizeSlider_font = QFont(self.WorldSizeSlider.font())
        self.WorldSizeSlider.setFont(WorldSizeSlider_font)
        self.WorldSizeSlider.setMinValue(1)
        self.WorldSizeSlider.setMaxValue(200)
        self.WorldSizeSlider.setLineStep(10)
        self.WorldSizeSlider.setValue(60)
        self.WorldSizeSlider.setOrientation(QSlider.Horizontal)
        layout77.addWidget(self.WorldSizeSlider)

        self.WorldSizeTextLabel = QLabel(self,"WorldSizeTextLabel")
        self.WorldSizeTextLabel.setEnabled(1)
        WorldSizeTextLabel_font = QFont(self.WorldSizeTextLabel.font())
        self.WorldSizeTextLabel.setFont(WorldSizeTextLabel_font)
        layout77.addWidget(self.WorldSizeTextLabel)
        layout78.addLayout(layout77)
        layout79.addLayout(layout78)
        pyPetriConfigureViewLayout.addLayout(layout79)

        layout95 = QVBoxLayout(None,0,6,"layout95")

        self.DeathLabel = QLabel(self,"DeathLabel")
        DeathLabel_font = QFont(self.DeathLabel.font())
        DeathLabel_font.setBold(1)
        self.DeathLabel.setFont(DeathLabel_font)
        self.DeathLabel.setAlignment(QLabel.AlignCenter)
        layout95.addWidget(self.DeathLabel)

        layout94 = QHBoxLayout(None,0,6,"layout94")

        self.DeathButtonGroup = QButtonGroup(self,"DeathButtonGroup")
        self.DeathButtonGroup.setSizePolicy(QSizePolicy(QSizePolicy.MinimumExpanding,QSizePolicy.MinimumExpanding,1,0,self.DeathButtonGroup.sizePolicy().hasHeightForWidth()))
        self.DeathButtonGroup.setFrameShadow(QButtonGroup.Plain)
        self.DeathButtonGroup.setLineWidth(0)
        self.DeathButtonGroup.setFlat(1)

        self.DieYesButton = QRadioButton(self.DeathButtonGroup,"DieYesButton")
        self.DieYesButton.setGeometry(QRect(0,20,50,21))
        DieYesButton_font = QFont(self.DieYesButton.font())
        self.DieYesButton.setFont(DieYesButton_font)

        self.DieNoButton = QRadioButton(self.DeathButtonGroup,"DieNoButton")
        self.DieNoButton.setGeometry(QRect(0,0,50,21))
        DieNoButton_font = QFont(self.DieNoButton.font())
        self.DieNoButton.setFont(DieNoButton_font)
        self.DieNoButton.setChecked(1)
        layout94.addWidget(self.DeathButtonGroup)

        layout92 = QVBoxLayout(None,0,6,"layout92")

        self.DeathTextLabel2 = QLabel(self,"DeathTextLabel2")
        self.DeathTextLabel2.setEnabled(0)
        DeathTextLabel2_font = QFont(self.DeathTextLabel2.font())
        self.DeathTextLabel2.setFont(DeathTextLabel2_font)
        layout92.addWidget(self.DeathTextLabel2)

        layout91 = QHBoxLayout(None,0,6,"layout91")

        self.LifeSpanSpinBox = QSpinBox(self,"LifeSpanSpinBox")
        self.LifeSpanSpinBox.setEnabled(0)
        self.LifeSpanSpinBox.setMaxValue(99999)
        self.LifeSpanSpinBox.setMinValue(1)
        self.LifeSpanSpinBox.setValue(10)
        layout91.addWidget(self.LifeSpanSpinBox)

        self.DeathTextLabel3 = QLabel(self,"DeathTextLabel3")
        self.DeathTextLabel3.setEnabled(0)
        DeathTextLabel3_font = QFont(self.DeathTextLabel3.font())
        self.DeathTextLabel3.setFont(DeathTextLabel3_font)
        layout91.addWidget(self.DeathTextLabel3)
        layout92.addLayout(layout91)
        layout94.addLayout(layout92)
        layout95.addLayout(layout94)
        pyPetriConfigureViewLayout.addLayout(layout95)

        layout86 = QVBoxLayout(None,0,6,"layout86")

        self.RandomHeadTextLabel = QLabel(self,"RandomHeadTextLabel")
        self.RandomHeadTextLabel.setEnabled(1)
        RandomHeadTextLabel_font = QFont(self.RandomHeadTextLabel.font())
        RandomHeadTextLabel_font.setBold(1)
        self.RandomHeadTextLabel.setFont(RandomHeadTextLabel_font)
        self.RandomHeadTextLabel.setAlignment(QLabel.AlignCenter)
        layout86.addWidget(self.RandomHeadTextLabel)

        self.RadomGeneratedRadioButton = QRadioButton(self,"RadomGeneratedRadioButton")
        RadomGeneratedRadioButton_font = QFont(self.RadomGeneratedRadioButton.font())
        self.RadomGeneratedRadioButton.setFont(RadomGeneratedRadioButton_font)
        self.RadomGeneratedRadioButton.setChecked(1)
        layout86.addWidget(self.RadomGeneratedRadioButton)

        layout85 = QHBoxLayout(None,0,6,"layout85")

        self.RandomFixedRadioButton = QRadioButton(self,"RandomFixedRadioButton")
        RandomFixedRadioButton_font = QFont(self.RandomFixedRadioButton.font())
        self.RandomFixedRadioButton.setFont(RandomFixedRadioButton_font)
        layout85.addWidget(self.RandomFixedRadioButton)

        self.RandomSpinBox = QSpinBox(self,"RandomSpinBox")
        self.RandomSpinBox.setEnabled(0)
        RandomSpinBox_font = QFont(self.RandomSpinBox.font())
        self.RandomSpinBox.setFont(RandomSpinBox_font)
        self.RandomSpinBox.setMaxValue(32767)
        self.RandomSpinBox.setMinValue(1)
        self.RandomSpinBox.setValue(32767)
        layout85.addWidget(self.RandomSpinBox)
        spacer1 = QSpacerItem(60,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout85.addItem(spacer1)
        layout86.addLayout(layout85)
        pyPetriConfigureViewLayout.addLayout(layout86)

        layout84 = QVBoxLayout(None,0,6,"layout84")

        self.AncestorHeadTextLabel = QLabel(self,"AncestorHeadTextLabel")
        AncestorHeadTextLabel_font = QFont(self.AncestorHeadTextLabel.font())
        AncestorHeadTextLabel_font.setBold(1)
        self.AncestorHeadTextLabel.setFont(AncestorHeadTextLabel_font)
        self.AncestorHeadTextLabel.setAlignment(QLabel.WordBreak | QLabel.AlignCenter)
        layout84.addWidget(self.AncestorHeadTextLabel)

        self.AncestorComboBox = QComboBox(0,self,"AncestorComboBox")
        AncestorComboBox_font = QFont(self.AncestorComboBox.font())
        self.AncestorComboBox.setFont(AncestorComboBox_font)
        self.AncestorComboBox.setAcceptDrops(1)
        self.AncestorComboBox.setEditable(1)
        layout84.addWidget(self.AncestorComboBox)
        pyPetriConfigureViewLayout.addLayout(layout84)

        layout80 = QVBoxLayout(None,0,6,"layout80")

        self.BirthHeadTextLabel = QLabel(self,"BirthHeadTextLabel")
        BirthHeadTextLabel_font = QFont(self.BirthHeadTextLabel.font())
        BirthHeadTextLabel_font.setBold(1)
        self.BirthHeadTextLabel.setFont(BirthHeadTextLabel_font)
        self.BirthHeadTextLabel.setAlignment(QLabel.WordBreak | QLabel.AlignCenter)
        layout80.addWidget(self.BirthHeadTextLabel)

        self.BirthButtonGroup = QButtonGroup(self,"BirthButtonGroup")
        self.BirthButtonGroup.setFrameShadow(QButtonGroup.Plain)
        self.BirthButtonGroup.setLineWidth(0)
        self.BirthButtonGroup.setFlat(1)
        self.BirthButtonGroup.setColumnLayout(0,Qt.Vertical)
        self.BirthButtonGroup.layout().setSpacing(6)
        self.BirthButtonGroup.layout().setMargin(11)
        BirthButtonGroupLayout = QHBoxLayout(self.BirthButtonGroup.layout())
        BirthButtonGroupLayout.setAlignment(Qt.AlignTop)

        self.LocalBirthRadioButton = QRadioButton(self.BirthButtonGroup,"LocalBirthRadioButton")
        LocalBirthRadioButton_font = QFont(self.LocalBirthRadioButton.font())
        self.LocalBirthRadioButton.setFont(LocalBirthRadioButton_font)
        self.LocalBirthRadioButton.setChecked(1)
        BirthButtonGroupLayout.addWidget(self.LocalBirthRadioButton)

        self.MassActionRadioButton = QRadioButton(self.BirthButtonGroup,"MassActionRadioButton")
        MassActionRadioButton_font = QFont(self.MassActionRadioButton.font())
        self.MassActionRadioButton.setFont(MassActionRadioButton_font)
        BirthButtonGroupLayout.addWidget(self.MassActionRadioButton)
        layout80.addWidget(self.BirthButtonGroup)
        pyPetriConfigureViewLayout.addLayout(layout80)

        layout83 = QVBoxLayout(None,0,6,"layout83")

        self.StopHeadTextLabel = QLabel(self,"StopHeadTextLabel")
        StopHeadTextLabel_font = QFont(self.StopHeadTextLabel.font())
        StopHeadTextLabel_font.setBold(1)
        self.StopHeadTextLabel.setFont(StopHeadTextLabel_font)
        self.StopHeadTextLabel.setAlignment(QLabel.WordBreak | QLabel.AlignCenter)
        layout83.addWidget(self.StopHeadTextLabel)

        self.StopButtonGroup = QButtonGroup(self,"StopButtonGroup")
        self.StopButtonGroup.setFrameShadow(QButtonGroup.Plain)
        self.StopButtonGroup.setLineWidth(0)
        self.StopButtonGroup.setFlat(1)
        self.StopButtonGroup.setColumnLayout(0,Qt.Vertical)
        self.StopButtonGroup.layout().setSpacing(6)
        self.StopButtonGroup.layout().setMargin(11)
        StopButtonGroupLayout = QHBoxLayout(self.StopButtonGroup.layout())
        StopButtonGroupLayout.setAlignment(Qt.AlignTop)

        self.StopManuallyRadioButton = QRadioButton(self.StopButtonGroup,"StopManuallyRadioButton")
        StopManuallyRadioButton_font = QFont(self.StopManuallyRadioButton.font())
        self.StopManuallyRadioButton.setFont(StopManuallyRadioButton_font)
        self.StopManuallyRadioButton.setChecked(1)
        StopButtonGroupLayout.addWidget(self.StopManuallyRadioButton)

        layout81 = QHBoxLayout(None,0,6,"layout81")

        self.StopAtRadioButton = QRadioButton(self.StopButtonGroup,"StopAtRadioButton")
        layout81.addWidget(self.StopAtRadioButton)

        self.StopAtSpinBox = QSpinBox(self.StopButtonGroup,"StopAtSpinBox")
        self.StopAtSpinBox.setEnabled(0)
        StopAtSpinBox_font = QFont(self.StopAtSpinBox.font())
        self.StopAtSpinBox.setFont(StopAtSpinBox_font)
        self.StopAtSpinBox.setMaxValue(10000000)
        self.StopAtSpinBox.setMinValue(1)
        self.StopAtSpinBox.setLineStep(1000)
        self.StopAtSpinBox.setValue(10000)
        layout81.addWidget(self.StopAtSpinBox)
        StopButtonGroupLayout.addLayout(layout81)
        layout83.addWidget(self.StopButtonGroup)
        pyPetriConfigureViewLayout.addLayout(layout83)

        self.SavePetriPushButton = QPushButton(self,"SavePetriPushButton")
        SavePetriPushButton_font = QFont(self.SavePetriPushButton.font())
        self.SavePetriPushButton.setFont(SavePetriPushButton_font)
        pyPetriConfigureViewLayout.addWidget(self.SavePetriPushButton)

        self.languageChange()

        self.resize(QSize(309,451).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.connect(self.RadomGeneratedRadioButton,SIGNAL("clicked()"),self.RandomFixedRadioButton.toggle)
        self.connect(self.RandomFixedRadioButton,SIGNAL("clicked()"),self.RadomGeneratedRadioButton.toggle)


    def languageChange(self):
        self.setCaption(self.__tr("Form1"))
        self.MutationRateHeadTextLabel.setText(self.__tr("Mutation Rate"))
        QToolTip.add(self.MutationRateHeadTextLabel,self.__tr("Set the rate of mutation from 0 to 1"))
        QToolTip.add(self.MutationSlider,self.__tr("Set Mutation Rate from 0 to 100 %"))
        self.MutationPercentTextLabel.setText(self.__tr("100 %"))
        self.WorldSizeHeadTextLable.setText(self.__tr("World Size"))
        self.WorldSizeTextLabel.setText(self.__tr("60 x 60 cells"))
        self.DeathLabel.setText(self.__tr("Do organisms die of old age?"))
        self.DeathButtonGroup.setTitle(QString.null)
        self.DieYesButton.setText(self.__tr("Yes"))
        QToolTip.add(self.DieYesButton,self.__tr("Organism can die without being replaced"))
        self.DieNoButton.setText(self.__tr("No"))
        QToolTip.add(self.DieNoButton,self.__tr("Organisms only die when replaced by new oranisims"))
        self.DeathTextLabel2.setText(self.__tr("when # executed instructions exceeds"))
        QToolTip.add(self.LifeSpanSpinBox,self.__tr("How long"))
        self.DeathTextLabel3.setText(self.__tr("times the genome length"))
        self.RandomHeadTextLabel.setText(self.__tr("Random Number Generator"))
        self.RadomGeneratedRadioButton.setText(self.__tr("Different each time the program runs"))
        self.RandomFixedRadioButton.setText(self.__tr("Set at"))
        self.AncestorHeadTextLabel.setText(self.__tr("Ancestral Organism(s)"))
        self.AncestorComboBox.clear()
        self.AncestorComboBox.insertItem(self.__tr("default.organism"))
        self.AncestorComboBox.setCurrentItem(0)
        self.BirthHeadTextLabel.setText(self.__tr("Where should new organism be placed?"))
        self.BirthButtonGroup.setTitle(QString.null)
        self.LocalBirthRadioButton.setText(self.__tr("Nearby their parent"))
        self.MassActionRadioButton.setText(self.__tr("Anywhere, randomly"))
        self.StopHeadTextLabel.setText(self.__tr("Stop Run at"))
        self.StopButtonGroup.setTitle(QString.null)
        self.StopManuallyRadioButton.setText(self.__tr("Manually"))
        self.StopAtRadioButton.setText(QString.null)
        self.StopAtSpinBox.setSuffix(self.__tr(" updates"))
        self.SavePetriPushButton.setText(self.__tr("Freeze Petri Dish"))


    def __tr(self,s,c = None):
        return qApp.translate("pyPetriConfigureView",s,c)
