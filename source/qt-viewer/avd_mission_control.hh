#ifndef AVD_MISSION_CONTROL_HH
#define AVD_MISSION_CONTROL_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif


/*
avd_MissionControlData contains private member variables

  m_help_controller
  m_main_widget_controller
  m_settings_controller
  m_driver_controller
  m_dumb_controller

and I am not using memory management techniques or accessors to handle
them.  for now I expect to create them and mission_control exactly once,
and to not destroy them except on application exit.
-- kgn
*/

class avd_MissionControlData;
class cPopulation;
class cEventList;
class QMutex;


class avd_MissionControl : public QObject
{ 
  Q_OBJECT

private:
  avd_MissionControlData *d;

public:
  avd_MissionControl(
    int argc,
    char **argv,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_MissionControl();

protected slots:
  void quitSlot(void);

public:
  /*
  obtaining original command-line arguments.  these can be used again as
  often as needed as long as you don't clobber argv.  i'm hoping to use
  them to enable reset of avida core.
  */
  int argc(void);
  char **argv(void);

  /*
  mutex management functions.  for protecting avida core data from
  reentrance when shared between gui and driver threads.  also used to
  protect a message queue for communication between the two threads.
  */
  void lock(void);
  bool locked(void);
  bool tryLock(void);
  void unlock(void);
  QMutex *getMutex(void);

  /*
  */
  bool avidaIsRunning();
public slots:
  void setPopulation(cPopulation *population);
  void setEventList(cEventList *event_list);
  void setSelectedPopulationCellID(int cell_id);

public:
  cPopulation *getPopulation(void);
  cEventList *getEventList(void);
  int getSelectedPopulationCellID(void);

  // for triggering gui messages.
  void emitHelpURLSig(const QString &rel_path);
  //void emitNewMainWidgetSig(void);
  void emitUserSettingsSig(void);
  void emitOpenAvidaSig(void);
  void emitCloseAvidaSig(void);
  void emitResetAvidaSig(void);
  void emitNewMainWindowSig(void);
  void emitNewInstructionViewerSig(void);
  void emitNewEventViewerSig(void);
  void emitNewGodBoxSig(void);
  void emitNewPlotViewerSig(void);
  void emitQuitSig(void);

  void emitAvidaOpenedSig(void);
  void emitAvidaResetSig(void);
  void emitPopulationCellSelectedSig(int cell_id);

  // for triggering messages between threads.
    // triggering messages sent from the gui to the driver:
  void emitStartAvidaSig(void);
  void emitUpdateAvidaSig(void);
  void emitStepAvidaSig(int cell_id);
  void emitStopAvidaSig(void);
  void emitExitAvidaSig(void);
    // triggering messages sent from the driver to the gui:
  void emitAvidaUpdatedSig(void);
  void emitAvidaSteppedSig(int cell_id);
  void emitAvidaBreakpointSig(int cell_id);
  void emitAvidaStoppedSig(void);
  void emitAvidaExitedSig(void);
  void emitQueryGUISig(void);
  void emitGuiStatusSig(bool running);
  void emitIsAvidaRunningSig(void);
  void emitAvidaStatusSig(bool running);

signals:
  // gui messages.
  void helpURLSig(const QString &rel_path);
  //void newMainWidgetSig(void);
  void userSettingsSig(void);
  void openAvidaSig(void);
  void closeAvidaSig(void);
  void resetAvidaSig(void);
  void newMainWindowSig(void);
  void newInstructionViewerSig(void);
  void newEventViewerSig(void);
  void newGodBoxSig(void);
  void newPlotViewerSig(void);
  void quitSig(void);

  void avidaOpenedSig(void);
  void avidaResetSig(void);
  void populationCellSelectedSig(int cell_id);

  // threading messages.
    // messages from the gui to the driver:
  void startAvidaSig(void);
  void updateAvidaSig(void);
  void stepAvidaSig(int cell_id);
  void stopAvidaSig(void);
  void exitAvidaSig(void);
    // messages from the driver to the gui::
  void avidaUpdatedSig(void);
  void avidaSteppedSig(int cell_id);
  void avidaBreakpointSig(int cell_id);
  void avidaStoppedSig(void);
  void avidaExitedSig(void);
  void queryGUISig(void);
  void guiStatusSig(bool running);
  void isAvidaRunningSig(void);
  void avidaStatusSig(bool running);
};


#else
#warning multiple inclusion of avd_mission_control.hh
#endif /* !AVD_MISSION_CONTROL_HH */

/* vim: set ts=2 ai et: */

