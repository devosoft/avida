//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GODBOX_WIDGET2_HH
#define GODBOX_WIDGET2_HH


#include "godbox2.hh"
#include <qmainwindow.h>


class QPushButton;
class QGroupBox;
class QHBoxLayout; 
class QVBoxLayout; 
class QSpacerItem;

class avd_MissionControl;

class ApocalypseWidget;
class avd_RateValidator2;
class avd_LogRateSlider2;
class avd_LogRateWidget;
class avd_RateSlider2;
class avd_RateValidator2;
class RepopulateWidget;


//class GodBoxWidget2 : public QWidget
class GodBoxWidget2 : public QMainWindow
{
  Q_OBJECT
public:
  GodBoxWidget2(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  ~GodBoxWidget2(void);

public:
  static void GodBoxWidget2_dynamicConfig(void);
  void setMissionControl(avd_MissionControl *mission_control);
  void setup(void);

protected:
  avd_MissionControl *m_mission_control;
  avd_RateValidator2 *m_rate_validator;

  QVBoxLayout *m_vbox_layout;
    QHBoxLayout *m_hbox_controls_layout;
      QGroupBox *m_mutation_rates_groupbox;
        avd_LogRateWidget *m_pt_mutation_widget;
        avd_LogRateWidget *m_ins_del_mutation_widget;
        avd_LogRateWidget *m_cp_mutation_widget;
      QGroupBox *m_apocalypse_groupbox;
        ApocalypseWidget *m_apocalypse_widget;
      QGroupBox *m_repopulate_groupbox;
        RepopulateWidget *m_repopulate_widget;
    QHBoxLayout *m_hbox_buttons_layout;
      QPushButton *m_help_button;
      QSpacerItem *m_button_spacer;
      QPushButton *m_close_button;

public:
  static QString s_preset_organisms_dir;

protected:
  void setupLayout(void);
  void minimizeLayout(void);
  void setupRates(void);
  void setupConnects(void);
  void loadMutRates(void);
  void loadCreatureList(void);

private slots:
  void ptMutRateWidgetChanged(double rate);
  void insDelMutRateWidgetChanged(double rate);
  void cpMutRateWidgetChanged(double rate);
  void apocalypsePushButtonClicked(void);
  void injectOneButtonClicked(void);
  void injectAllButtonClicked(void);
  void helpButtonClicked(void);
  void closeButtonClicked(void);
  void stateChanged(void);

  void helpContents();
  void helpGenesis();
  void helpInstructionSet();
  void helpEvents();
  void helpEnvironments();
  void helpAnalyzeMode();

};

#endif /* !GODBOX_WIDGET2_HH */

