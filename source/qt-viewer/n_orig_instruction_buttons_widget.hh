//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_BUTTONS_WIDGET_HH
#define N_ORIG_INSTRUCTION_BUTTONS_WIDGET_HH

#include <qwidget.h>
#include <qguardedptr.h>


class avd_MissionControl;
class QHBoxLayout;
class QHButtonGroup;
class QPushButton;
class QRadioButton;
class QVBoxLayout;
class QWidgetStack;

class
N_Instruction_ButtonsWidget : public QWidget {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  QVBoxLayout *m_vboxlayout;
    QHBoxLayout *m_hboxlayout1;
      QWidgetStack *m_widgetstack;
        QPushButton *m_start_pbutton;
        QPushButton *m_stop_pbutton;
      QPushButton *m_step_pbutton;
      QPushButton *m_update_pbutton;
    QHBoxLayout *m_hboxlayout2;
      QPushButton *m_extract_pbutton;
public:
  N_Instruction_ButtonsWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  void setMissionControl(avd_MissionControl *mission_control);
public slots:
  void setPopulationCell(int cell_id);
public slots:
  void enableStepUpdateExtractButtonsSlot(void);
  void enableStartButtonSlot(void);
  void raiseStartButtonSlot(void);
  void raiseStopButtonSlot(void);
  void disableStepUpdateExtractButtonsSlot(void);
  void disableStartButtonSlot(void);

  void avidaStartingSlot(void);
  void avidaSteppingSlot(void);
  void avidaUpdatingSlot(void);
  void avidaStoppedSlot(void);
  void localStepSlot(void);

  void extractCreatureSlot(void);

signals:
  void localStepSig(int);
};

#endif /* !N_ORIG_INSTRUCTION_BUTTONS_WIDGET_HH */

