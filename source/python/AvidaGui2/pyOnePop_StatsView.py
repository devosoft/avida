# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file './pyOnePop_StatsView.ui'
#
# Created: Tue Apr 5 15:43:51 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyOnePop_StatsView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyOnePop_StatsView")

        self.setSizePolicy(QSizePolicy(3,3,0,0,self.sizePolicy().hasHeightForWidth()))

        pyOnePop_StatsViewLayout = QHBoxLayout(self,11,6,"pyOnePop_StatsViewLayout")

        layout46 = QVBoxLayout(None,0,6,"layout46")

        self.groupBox3 = QGroupBox(self,"groupBox3")
        self.groupBox3.setSizePolicy(QSizePolicy(5,5,0,0,self.groupBox3.sizePolicy().hasHeightForWidth()))
        groupBox3_font = QFont(self.groupBox3.font())
        groupBox3_font.setPointSize(10)
        self.groupBox3.setFont(groupBox3_font)
        self.groupBox3.setColumnLayout(0,Qt.Vertical)
        self.groupBox3.layout().setSpacing(6)
        self.groupBox3.layout().setMargin(11)
        groupBox3Layout = QVBoxLayout(self.groupBox3.layout())
        groupBox3Layout.setAlignment(Qt.AlignTop)

        layout53 = QVBoxLayout(None,0,6,"layout53")

        layout20 = QHBoxLayout(None,0,6,"layout20")

        self.textLabel14 = QLabel(self.groupBox3,"textLabel14")
        textLabel14_font = QFont(self.textLabel14.font())
        textLabel14_font.setBold(1)
        textLabel14_font.setUnderline(1)
        self.textLabel14.setFont(textLabel14_font)
        layout20.addWidget(self.textLabel14)

        self.textLabel14_2 = QLabel(self.groupBox3,"textLabel14_2")
        textLabel14_2_font = QFont(self.textLabel14_2.font())
        textLabel14_2_font.setBold(1)
        textLabel14_2_font.setUnderline(1)
        self.textLabel14_2.setFont(textLabel14_2_font)
        layout20.addWidget(self.textLabel14_2)
        layout53.addLayout(layout20)

        layout51 = QHBoxLayout(None,0,6,"layout51")

        self.textLabel14_3 = QLabel(self.groupBox3,"textLabel14_3")
        self.textLabel14_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3.sizePolicy().hasHeightForWidth()))
        textLabel14_3_font = QFont(self.textLabel14_3.font())
        self.textLabel14_3.setFont(textLabel14_3_font)
        layout51.addWidget(self.textLabel14_3)

        self.m_num_not = QLabel(self.groupBox3,"m_num_not")
        self.m_num_not.setSizePolicy(QSizePolicy(5,5,0,0,self.m_num_not.sizePolicy().hasHeightForWidth()))
        m_num_not_font = QFont(self.m_num_not.font())
        self.m_num_not.setFont(m_num_not_font)
        layout51.addWidget(self.m_num_not)
        layout53.addLayout(layout51)

        layout52 = QHBoxLayout(None,0,6,"layout52")

        self.textLabel4 = QLabel(self.groupBox3,"textLabel4")
        textLabel4_font = QFont(self.textLabel4.font())
        self.textLabel4.setFont(textLabel4_font)
        self.textLabel4.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout52.addWidget(self.textLabel4)

        self.m_num_nand = QLabel(self.groupBox3,"m_num_nand")
        m_num_nand_font = QFont(self.m_num_nand.font())
        self.m_num_nand.setFont(m_num_nand_font)
        layout52.addWidget(self.m_num_nand)
        layout53.addLayout(layout52)

        layout9 = QHBoxLayout(None,0,6,"layout9")

        self.textLabel6 = QLabel(self.groupBox3,"textLabel6")
        textLabel6_font = QFont(self.textLabel6.font())
        self.textLabel6.setFont(textLabel6_font)
        self.textLabel6.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout9.addWidget(self.textLabel6)

        self.m_num_and = QLabel(self.groupBox3,"m_num_and")
        m_num_and_font = QFont(self.m_num_and.font())
        self.m_num_and.setFont(m_num_and_font)
        layout9.addWidget(self.m_num_and)
        layout53.addLayout(layout9)

        layout10 = QHBoxLayout(None,0,6,"layout10")

        self.textLabel17 = QLabel(self.groupBox3,"textLabel17")
        textLabel17_font = QFont(self.textLabel17.font())
        self.textLabel17.setFont(textLabel17_font)
        self.textLabel17.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout10.addWidget(self.textLabel17)

        self.m_num_ornot = QLabel(self.groupBox3,"m_num_ornot")
        m_num_ornot_font = QFont(self.m_num_ornot.font())
        self.m_num_ornot.setFont(m_num_ornot_font)
        layout10.addWidget(self.m_num_ornot)
        layout53.addLayout(layout10)

        layout11 = QHBoxLayout(None,0,6,"layout11")

        self.textLabel10 = QLabel(self.groupBox3,"textLabel10")
        textLabel10_font = QFont(self.textLabel10.font())
        self.textLabel10.setFont(textLabel10_font)
        self.textLabel10.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout11.addWidget(self.textLabel10)

        self.m_num_or = QLabel(self.groupBox3,"m_num_or")
        m_num_or_font = QFont(self.m_num_or.font())
        self.m_num_or.setFont(m_num_or_font)
        layout11.addWidget(self.m_num_or)
        layout53.addLayout(layout11)

        layout12 = QHBoxLayout(None,0,6,"layout12")

        self.textLabel12 = QLabel(self.groupBox3,"textLabel12")
        textLabel12_font = QFont(self.textLabel12.font())
        self.textLabel12.setFont(textLabel12_font)
        self.textLabel12.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout12.addWidget(self.textLabel12)

        self.m_num_andnot = QLabel(self.groupBox3,"m_num_andnot")
        m_num_andnot_font = QFont(self.m_num_andnot.font())
        self.m_num_andnot.setFont(m_num_andnot_font)
        layout12.addWidget(self.m_num_andnot)
        layout53.addLayout(layout12)

        layout13 = QHBoxLayout(None,0,6,"layout13")

        self.textLabel14_4 = QLabel(self.groupBox3,"textLabel14_4")
        textLabel14_4_font = QFont(self.textLabel14_4.font())
        self.textLabel14_4.setFont(textLabel14_4_font)
        self.textLabel14_4.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout13.addWidget(self.textLabel14_4)

        self.m_num_nor = QLabel(self.groupBox3,"m_num_nor")
        m_num_nor_font = QFont(self.m_num_nor.font())
        self.m_num_nor.setFont(m_num_nor_font)
        layout13.addWidget(self.m_num_nor)
        layout53.addLayout(layout13)

        layout14 = QHBoxLayout(None,0,6,"layout14")

        self.textLabel16 = QLabel(self.groupBox3,"textLabel16")
        textLabel16_font = QFont(self.textLabel16.font())
        self.textLabel16.setFont(textLabel16_font)
        self.textLabel16.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout14.addWidget(self.textLabel16)

        self.m_num_xor = QLabel(self.groupBox3,"m_num_xor")
        m_num_xor_font = QFont(self.m_num_xor.font())
        self.m_num_xor.setFont(m_num_xor_font)
        layout14.addWidget(self.m_num_xor)
        layout53.addLayout(layout14)

        layout15 = QHBoxLayout(None,0,6,"layout15")

        self.textLabel18 = QLabel(self.groupBox3,"textLabel18")
        textLabel18_font = QFont(self.textLabel18.font())
        self.textLabel18.setFont(textLabel18_font)
        self.textLabel18.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignLeft)
        layout15.addWidget(self.textLabel18)

        self.m_num_equals = QLabel(self.groupBox3,"m_num_equals")
        m_num_equals_font = QFont(self.m_num_equals.font())
        self.m_num_equals.setFont(m_num_equals_font)
        layout15.addWidget(self.m_num_equals)
        layout53.addLayout(layout15)
        groupBox3Layout.addLayout(layout53)
        spacer1 = QSpacerItem(16,16,QSizePolicy.Minimum,QSizePolicy.Minimum)
        groupBox3Layout.addItem(spacer1)
        layout46.addWidget(self.groupBox3)

        self.groupBox5 = QGroupBox(self,"groupBox5")
        self.groupBox5.setSizePolicy(QSizePolicy(5,5,0,0,self.groupBox5.sizePolicy().hasHeightForWidth()))
        groupBox5_font = QFont(self.groupBox5.font())
        groupBox5_font.setPointSize(11)
        self.groupBox5.setFont(groupBox5_font)
        self.groupBox5.setColumnLayout(0,Qt.Vertical)
        self.groupBox5.layout().setSpacing(6)
        self.groupBox5.layout().setMargin(11)
        groupBox5Layout = QGridLayout(self.groupBox5.layout())
        groupBox5Layout.setAlignment(Qt.AlignTop)

        self.m_dom_fitness = QLabel(self.groupBox5,"m_dom_fitness")
        self.m_dom_fitness.setSizePolicy(QSizePolicy(5,5,0,0,self.m_dom_fitness.sizePolicy().hasHeightForWidth()))
        m_dom_fitness_font = QFont(self.m_dom_fitness.font())
        m_dom_fitness_font.setPointSize(10)
        self.m_dom_fitness.setFont(m_dom_fitness_font)

        groupBox5Layout.addWidget(self.m_dom_fitness,1,1)

        self.m_avg_fitness = QLabel(self.groupBox5,"m_avg_fitness")
        self.m_avg_fitness.setSizePolicy(QSizePolicy(0,0,0,0,self.m_avg_fitness.sizePolicy().hasHeightForWidth()))
        self.m_avg_fitness.setMinimumSize(QSize(45,10))
        self.m_avg_fitness.setMaximumSize(QSize(45,5))
        m_avg_fitness_font = QFont(self.m_avg_fitness.font())
        m_avg_fitness_font.setPointSize(10)
        self.m_avg_fitness.setFont(m_avg_fitness_font)
        self.m_avg_fitness.setLineWidth(7)

        groupBox5Layout.addWidget(self.m_avg_fitness,0,1)

        self.textLabel14_3_3_2 = QLabel(self.groupBox5,"textLabel14_3_3_2")
        self.textLabel14_3_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_font = QFont(self.textLabel14_3_3_2.font())
        textLabel14_3_3_2_font.setPointSize(10)
        self.textLabel14_3_3_2.setFont(textLabel14_3_3_2_font)

        groupBox5Layout.addWidget(self.textLabel14_3_3_2,1,0)

        self.m_num_orgs = QLabel(self.groupBox5,"m_num_orgs")
        self.m_num_orgs.setSizePolicy(QSizePolicy(5,5,0,0,self.m_num_orgs.sizePolicy().hasHeightForWidth()))
        m_num_orgs_font = QFont(self.m_num_orgs.font())
        m_num_orgs_font.setPointSize(10)
        self.m_num_orgs.setFont(m_num_orgs_font)

        groupBox5Layout.addWidget(self.m_num_orgs,2,1)

        self.textLabel14_3_3 = QLabel(self.groupBox5,"textLabel14_3_3")
        self.textLabel14_3_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_font = QFont(self.textLabel14_3_3.font())
        textLabel14_3_3_font.setPointSize(10)
        self.textLabel14_3_3.setFont(textLabel14_3_3_font)

        groupBox5Layout.addWidget(self.textLabel14_3_3,0,0)

        self.textLabel14_3_3_2_2_2 = QLabel(self.groupBox5,"textLabel14_3_3_2_2_2")
        self.textLabel14_3_3_2_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2_2.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2_2_2.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_2_2_font = QFont(self.textLabel14_3_3_2_2_2.font())
        textLabel14_3_3_2_2_2_font.setPointSize(10)
        self.textLabel14_3_3_2_2_2.setFont(textLabel14_3_3_2_2_2_font)

        groupBox5Layout.addWidget(self.textLabel14_3_3_2_2_2,3,0)

        self.m_avg_gest = QLabel(self.groupBox5,"m_avg_gest")
        self.m_avg_gest.setSizePolicy(QSizePolicy(5,5,0,0,self.m_avg_gest.sizePolicy().hasHeightForWidth()))
        m_avg_gest_font = QFont(self.m_avg_gest.font())
        m_avg_gest_font.setPointSize(10)
        self.m_avg_gest.setFont(m_avg_gest_font)

        groupBox5Layout.addWidget(self.m_avg_gest,3,1)

        self.textLabel14_3_3_2_2 = QLabel(self.groupBox5,"textLabel14_3_3_2_2")
        self.textLabel14_3_3_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2_2.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_2_font = QFont(self.textLabel14_3_3_2_2.font())
        textLabel14_3_3_2_2_font.setPointSize(10)
        self.textLabel14_3_3_2_2.setFont(textLabel14_3_3_2_2_font)

        groupBox5Layout.addWidget(self.textLabel14_3_3_2_2,2,0)
        layout46.addWidget(self.groupBox5)
        pyOnePop_StatsViewLayout.addLayout(layout46)

        self.groupBox4 = QGroupBox(self,"groupBox4")
        self.groupBox4.setSizePolicy(QSizePolicy(5,5,0,0,self.groupBox4.sizePolicy().hasHeightForWidth()))
        groupBox4_font = QFont(self.groupBox4.font())
        groupBox4_font.setPointSize(11)
        self.groupBox4.setFont(groupBox4_font)
        self.groupBox4.setColumnLayout(0,Qt.Vertical)
        self.groupBox4.layout().setSpacing(6)
        self.groupBox4.layout().setMargin(11)
        groupBox4Layout = QVBoxLayout(self.groupBox4.layout())
        groupBox4Layout.setAlignment(Qt.AlignTop)

        layout80 = QGridLayout(None,1,1,0,6,"layout80")

        self.m_org_name = QLabel(self.groupBox4,"m_org_name")
        self.m_org_name.setSizePolicy(QSizePolicy(0,0,0,0,self.m_org_name.sizePolicy().hasHeightForWidth()))
        self.m_org_name.setMinimumSize(QSize(70,15))
        self.m_org_name.setMaximumSize(QSize(70,15))
        m_org_name_font = QFont(self.m_org_name.font())
        m_org_name_font.setPointSize(10)
        self.m_org_name.setFont(m_org_name_font)

        layout80.addWidget(self.m_org_name,0,1)

        self.textLabel14_3_3_2_2_2_2 = QLabel(self.groupBox4,"textLabel14_3_3_2_2_2_2")
        self.textLabel14_3_3_2_2_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2_2_2.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2_2_2_2.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_2_2_2_font = QFont(self.textLabel14_3_3_2_2_2_2.font())
        textLabel14_3_3_2_2_2_2_font.setPointSize(10)
        self.textLabel14_3_3_2_2_2_2.setFont(textLabel14_3_3_2_2_2_2_font)

        layout80.addWidget(self.textLabel14_3_3_2_2_2_2,2,0)

        self.textLabel14_3_3_3 = QLabel(self.groupBox4,"textLabel14_3_3_3")
        self.textLabel14_3_3_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_3.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_3_font = QFont(self.textLabel14_3_3_3.font())
        textLabel14_3_3_3_font.setPointSize(10)
        self.textLabel14_3_3_3.setFont(textLabel14_3_3_3_font)

        layout80.addWidget(self.textLabel14_3_3_3,0,0)

        self.m_org_age = QLabel(self.groupBox4,"m_org_age")
        m_org_age_font = QFont(self.m_org_age.font())
        m_org_age_font.setPointSize(10)
        self.m_org_age.setFont(m_org_age_font)

        layout80.addWidget(self.m_org_age,3,1)

        self.m_org_fitness = QLabel(self.groupBox4,"m_org_fitness")
        self.m_org_fitness.setSizePolicy(QSizePolicy(5,5,0,0,self.m_org_fitness.sizePolicy().hasHeightForWidth()))
        m_org_fitness_font = QFont(self.m_org_fitness.font())
        m_org_fitness_font.setPointSize(10)
        self.m_org_fitness.setFont(m_org_fitness_font)

        layout80.addWidget(self.m_org_fitness,1,1)

        self.textLabel1 = QLabel(self.groupBox4,"textLabel1")
        textLabel1_font = QFont(self.textLabel1.font())
        textLabel1_font.setPointSize(10)
        self.textLabel1.setFont(textLabel1_font)

        layout80.addWidget(self.textLabel1,3,0)

        self.textLabel14_3_3_2_2_3 = QLabel(self.groupBox4,"textLabel14_3_3_2_2_3")
        self.textLabel14_3_3_2_2_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2_3.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2_2_3.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_2_3_font = QFont(self.textLabel14_3_3_2_2_3.font())
        textLabel14_3_3_2_2_3_font.setPointSize(10)
        self.textLabel14_3_3_2_2_3.setFont(textLabel14_3_3_2_2_3_font)

        layout80.addWidget(self.textLabel14_3_3_2_2_3,1,0)

        self.m_org_gestation_time = QLabel(self.groupBox4,"m_org_gestation_time")
        self.m_org_gestation_time.setSizePolicy(QSizePolicy(5,5,0,0,self.m_org_gestation_time.sizePolicy().hasHeightForWidth()))
        m_org_gestation_time_font = QFont(self.m_org_gestation_time.font())
        m_org_gestation_time_font.setPointSize(10)
        self.m_org_gestation_time.setFont(m_org_gestation_time_font)

        layout80.addWidget(self.m_org_gestation_time,2,1)
        groupBox4Layout.addLayout(layout80)
        spacer1_2_2 = QSpacerItem(16,16,QSizePolicy.Minimum,QSizePolicy.Minimum)
        groupBox4Layout.addItem(spacer1_2_2)
        pyOnePop_StatsViewLayout.addWidget(self.groupBox4)

        self.languageChange()

        self.resize(QSize(485,743).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("pyOnePop_StatsView"))
        self.groupBox3.setTitle(self.__tr("Population Task Report"))
        self.textLabel14.setText(self.__tr("Task"))
        self.textLabel14_2.setText(self.__tr("orgs performing"))
        self.textLabel14_3.setText(self.__tr("not"))
        self.m_num_not.setText(self.__tr("-"))
        self.textLabel4.setText(self.__tr("nand"))
        self.m_num_nand.setText(self.__tr("-"))
        self.textLabel6.setText(self.__tr("and"))
        self.m_num_and.setText(self.__tr("-"))
        self.textLabel17.setText(self.__tr("ornot"))
        self.m_num_ornot.setText(self.__tr("-"))
        self.textLabel10.setText(self.__tr("or"))
        self.m_num_or.setText(self.__tr("-"))
        self.textLabel12.setText(self.__tr("andnot"))
        self.m_num_andnot.setText(self.__tr("-"))
        self.textLabel14_4.setText(self.__tr("nor"))
        self.m_num_nor.setText(self.__tr("-"))
        self.textLabel16.setText(self.__tr("xor"))
        self.m_num_xor.setText(self.__tr("-"))
        self.textLabel18.setText(self.__tr("equals"))
        self.m_num_equals.setText(self.__tr("-"))
        self.groupBox5.setTitle(self.__tr("Population Statistics"))
        self.m_dom_fitness.setText(self.__tr("-"))
        self.m_avg_fitness.setText(self.__tr("-"))
        self.textLabel14_3_3_2.setText(self.__tr("Dominant Org. Fitness:"))
        self.m_num_orgs.setText(self.__tr("-"))
        self.textLabel14_3_3.setText(self.__tr("Avg. Fitness:"))
        self.textLabel14_3_3_2_2_2.setText(self.__tr("Avg. Gestation (instructions):"))
        self.m_avg_gest.setText(self.__tr("-"))
        self.textLabel14_3_3_2_2.setText(self.__tr("Population Size:"))
        self.groupBox4.setTitle(self.__tr("Org. Clicked on  Report"))
        self.m_org_name.setText(self.__tr("-"))
        self.textLabel14_3_3_2_2_2_2.setText(self.__tr("Gestation (instructions):"))
        self.textLabel14_3_3_3.setText(self.__tr("Name:"))
        self.m_org_age.setText(self.__tr("-"))
        self.m_org_fitness.setText(self.__tr("-"))
        self.textLabel1.setText(self.__tr("Age (updates):"))
        self.textLabel14_3_3_2_2_3.setText(self.__tr("Fitness:"))
        self.m_org_gestation_time.setText(self.__tr("-"))


    def __tr(self,s,c = None):
        return qApp.translate("pyOnePop_StatsView",s,c)
