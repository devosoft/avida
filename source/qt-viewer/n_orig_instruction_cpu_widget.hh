//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_CPU_WIDGET_HH
#define N_ORIG_INSTRUCTION_CPU_WIDGET_HH

#ifndef QFRAME_H
#include <qframe.h>
#endif
#ifndef QGROUPBOX_H
#include <qgroupbox.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QPTRLIST_H
#include <qptrlist.h>
#endif
#ifndef QSCROLLVIEW_H
#include <qscrollview.h>
#endif
#ifndef QVBOX_H
#include <qvbox.h>
#endif
#ifndef QWIDGET_H
#include <qwidget.h>
#endif

class avd_MissionControl;
class QPushButton;
class QHBoxLayout;
class QLabel;
class QScrollView;
class QVBoxLayout;
class QWidgetStack;
class cLabeledField : public QWidget {
  Q_OBJECT
protected:
  QHBoxLayout *m_hboxlayout;
  QLabel *m_title_label;
  QLabel *m_data1_label;
  QLabel *m_data2_label;
  int m_base;
public:
  cLabeledField(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WDestructiveClose
  );
  void setTitle(const QString &title);
  void setData1Text(const QString &text);
  void setData2Text(const QString &text);
  void setData2Binary(unsigned long value);
  void setData1DecimalWidth(void);
  void setData2BinaryWidth(void);
  void setBase(int base);
  int getBase(){ return m_base; }
public slots:
  void showData2Text(bool);
};

//class cSummaryWidget : public QGroupBox {
class cSummaryWidget : public QVBox {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  cLabeledField *m_genotype_id;
  cLabeledField *m_genotype_name;
  cLabeledField *m_species_id;

  cLabeledField *m_fitness;
  cLabeledField *m_gestation_time;
  cLabeledField *m_cpu_speed;
  cLabeledField *m_current_merit;
  cLabeledField *m_genome_size;
  cLabeledField *m_memory_size;
  cLabeledField *m_faults;

  cLabeledField *m_location;
  cLabeledField *m_facing;

  cLabeledField *m_generation;
  cLabeledField *m_age;
  cLabeledField *m_executed;
  cLabeledField *m_last_divide;
  cLabeledField *m_offspring;

  cLabeledField *m_mem_allocated;
  cLabeledField *m_parent_true;
  cLabeledField *m_injected;
  cLabeledField *m_parasite;
  cLabeledField *m_mutated;
  cLabeledField *m_modified;
public:
  cSummaryWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
};

class cRegistersWidget : public QGroupBox {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  QPtrList<cLabeledField> m_registers;
public:
  cRegistersWidget(
    QWidget *parent = 0,
    const char *name = 0
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
  void showData2Text(bool);
  void chooseBaseSlot(int base);
};

class cInputsWidget : public QGroupBox {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  QPtrList<cLabeledField> m_inputs;
public:
  cInputsWidget(
    QWidget *parent = 0,
    const char *name = 0
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
  void showData2Text(bool);
  void chooseBaseSlot(int base);
};

class cStackPopup : public QWidget {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  int m_stack_no;
  QVBoxLayout *m_vboxlayout;
    QScrollView *m_scrollview;
      QWidget *m_layout_widget;
        QVBoxLayout *m_sv_layout;
          QPtrList<cLabeledField> m_stacklines;
public:
  cStackPopup(
    QWidget *parent = 0,
    const char *name = 0
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void setStackNumber(int stack_number);
  void updateState(void);
  void showData2Text(bool);
  void chooseBaseSlot(int base);
};

class cStackField : public cLabeledField {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  int m_stack_no;
  QPushButton *m_popup_pbutton;
  cStackPopup *m_stack_popup;
public:
  cStackField(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WDestructiveClose
  );
  void useFrame(bool);
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void setStackNumber(int stack_number);
  void updateState(void);
  void showData2Text(bool);
  void chooseBaseSlot(int base);
protected slots:
  void displayPopupSlot(void);
};

class cStacksWidget : public QGroupBox {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  cStackField *m_stackln1;
  cStackField *m_stackln2;
public:
  cStacksWidget(
    QWidget *parent = 0,
    const char *name = 0
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
  void showData2Text(bool);
  void chooseBaseSlot(int base);
};

class cBaseChoiceBtns : public QWidget {
  Q_OBJECT
protected:
  QWidgetStack *m_widgetstack;
    QPushButton *m_show_binary_pbutton;
    QPushButton *m_hide_binary_pbutton;
public:
  cBaseChoiceBtns(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WDestructiveClose
  );
  void emitChooseBaseSig(int base){ emit chooseBaseSig(base); }
public slots:
  void showBinary(void);
  void hideBinary(void);
signals:
  void showBinarySig(bool);
  void chooseBaseSig(int);
};

class QVBox;
class cTasksWidget : public QGroupBox {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  QGuardedPtr<QScrollView> m_scrollview;
    QGuardedPtr<QVBox> m_vbox;
      QPtrList<cLabeledField> m_tasklines;
public:
  cTasksWidget(
    QWidget *parent = 0,
    const char *name = 0
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
};

class QVBox;
//class N_Instruction_CPUWidget : public QTabWidget {
class N_Instruction_CPUWidget : public QVBox {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  QVBox *m_component_vbox;
    cSummaryWidget *m_summary1;
    cRegistersWidget *m_registers;
    cStacksWidget *m_stacks;
    cInputsWidget *m_inputs;
    cBaseChoiceBtns *m_basechoice_btns;
  QVBox *m_stats_vbox;
    cSummaryWidget *m_summary2;
  QVBox *m_tasks_vbox;
    cSummaryWidget *m_summary3;
    cTasksWidget *m_tasks;
  QVBox *m_genotype_vbox;
    cSummaryWidget *m_summary4;
public:
  N_Instruction_CPUWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WDestructiveClose
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
  void showData2Text(bool);
  void chooseBaseSlot(int base);
};

#endif /* !N_ORIG_INSTRUCTION_CPU_WIDGET_HH */
