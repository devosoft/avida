#ifndef AVD_MAIN_WINDOW_HH
#define AVD_MAIN_WINDOW_HH

#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif


class QCloseEvent;
class QWidget;
class QStatusBar;

class avd_MissionControl;

class MapView;
class CreatureView;
class InitFileView;
class CellViewWidget;
class N_Instruction_Viewer;
class EventViewWidget;
//class GodBoxWidget;
class GodBoxWidget2;

//class MessagingPopulationWrapper;

class cPopulation;
class QWidget;


class avd_MainWindow : public QMainWindow{
  Q_OBJECT
private:
  QWidget *m_central_widget;
  QVBoxLayout *m_vlayout;
  QStatusBar *m_status_bar;
  avd_MissionControl &m_mission_control;

  QGuardedPtr<MapView> m_map_view;

private:
  void setupMenuBar();
  void setupViewer();
  void cleanupViewer();

public:
  avd_MainWindow(
    avd_MissionControl &mission_control,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  ~avd_MainWindow();

public slots:
  void closeSlot();

  // these trigger requests (signals) to Mission Control.
  void aboutAvidaHelpSlot();
  void help1Slot();
  void help2Slot();

  void helpContents();
  void helpGenesis();
  void helpInstructionSet();
  void helpEvents();
  void helpEnvironments();
  void helpAnalyzeMode();

  void openAvidaSlot(void);
  void closeAvidaSlot(void);
  void resetAvidaSlot(void);

  void startAvidaSlot(void);
  void updateAvidaSlot(void);
  void stepAvidaSlot(void);
  void stopAvidaSlot(void);
  void exitAvidaSlot(void);

  void setSelectedPopulationCellSlot(int cell_id);

  // these respond to signals from Mission Control.
  void avidaOpenedSlot(void);
  void avidaClosedSlot(void);
  void avidaResetSlot(void);

  void avidaUpdatedSlot(void);
  void avidaSteppedSlot(int cell_id);
  void avidaBreakpointSlot(int cell_id);
  void avidaStoppedSlot(void);

protected:
  void closeEvent(QCloseEvent *e);
signals:
  //void startAvidaSig(void);
  //void stepAvidaSig(int cell_id);
  //void updateAvidaSig(void);
  //void stopAvidaSig(void);
  //void exitAvidaSig(void);
  //void openAvidaSig(void);
  void closeAvidaSig(void);
  //void resetAvidaSig(void);

  void closeEventSig(avd_MainWindow *me);
  void helpURLSig(const QString &rel_path);
  void newMainWindowSig(void);
  void userSettingsSig(void);
  void quitSig(void);
};

#else
#warning multiple inclusion of avd_main_window.hh
#endif /* !AVD_MAIN_WINDOW_HH */

/* vim: set ts=2 ai et: */





//#ifndef MAIN_WINDOW_HH
//#define MAIN_WINDOW_HH
//
//#include <qmainwindow.h>
//#include <qguardedptr.h>
//
//class MapView;
//class CreatureView;
//class InitFileView;
//class CellViewWidget;
//class N_Instruction_Viewer;
//class EventViewWidget;
////class GodBoxWidget;
//class GodBoxWidget2;
//
//class MessagingPopulationWrapper;
//
//class cPopulation;
//
//class MainWindow : public QMainWindow {
//  Q_OBJECT
//private:
//  // store the command line arguments
//  const int m_argc;
//  const char * const * m_argv;
//
//  MessagingPopulationWrapper *m_pop_wrap; // the Avida population
//  
//  MapView *m_map_view;
//  
//  // pointers to a number of windows we might want to open
//  QGuardedPtr<CreatureView> m_creature_view;
//  QGuardedPtr<InitFileView> m_genesis_view;
//  QGuardedPtr<InitFileView> m_event_file_view;
//  QGuardedPtr<InitFileView> m_environment_view;
//  QGuardedPtr<InitFileView> m_inst_set_view;
//  QGuardedPtr<CellViewWidget> m_cell_view_widget;
//  QGuardedPtr<N_Instruction_Viewer> m_n_orig_instruction_viewer;
//  QGuardedPtr<EventViewWidget> m_event_view_widget;
//  //QGuardedPtr<GodBoxWidget> m_god_box_widget;
//  QGuardedPtr<GodBoxWidget2> m_god_box_widget2;
//
//  void setupMenuBar();
//  void setupViewer();
//  void cleanupViewer();
//
//public:
//  MainWindow(
//    MessagingPopulationWrapper *pop_wrap,
//    QWidget *parent,
//    int argc,
//    const char * const argv[]
//  );
//  ~MainWindow();
//
//
//signals:
//  void startAvidaSig();
//  void stopAvidaSig();
//  void resetAvidaSig();
//  void exitAvidaSig();
//
//
//public slots:
//  /**
//   * This function will start/resume the processing of Avida events.
//   **/
//  void startAvida();
//
//  /**
//   * This function will halt the processing of Avida events. A following
//   * call to startAvida() will resume exactly where at the point where
//   * stop was called.
//   **/
//  void stopAvida();
//
//  /**
//   * This function deletes the current Avida/viewer, and creates a new
//   * one, reading in all the init files again.
//   **/
//  void resetAvida();
//
//  /**
//   * This function exits the viewer.
//   **/
//  void exitProgram();
//
//signals:
//  /**
//   * This signal is emited when one update should be processed.
//   **/
//  void doUpdate();
//
//  /**
//   * This signal is emited when something in the Avida world has changed,
//   * so that windows have to be redrawn.
//   **/
//  void stateChanged();
//
//  /**
//   * This signal is emited when a the Avida object has been destroyed, e.g.
//   * on reset
//   **/
//  void avidaDestroyed();
//
//protected slots:
//  /**
//   * This slot opens the creature view.
//   **/
//  //void creatureView();
//
//  void cellView();
//  void N_Instruction_View(void);
//  void eventView(void);
//  //void godBox(void);
//  void godBox2(void);
//
//  /**
//   * This slot lets the user select which data should be plotted,
//   * and then creates a plot widget that displays this data.
//   **/
//  void createPlotView();
//
//  /**
//   * This slot opens a window displaying the genesis file.
//   **/
//  void genesisView();
//
//  /**
//   * This slot opens a window displaying the event list.
//   **/
//  void eventListView();
//
//  /**
//   * This slot opens a window displaying the environment.
//   **/
//  void environmentView();
//
//  /**
//   * This slot opens a window displaying the inst set.
//   **/
//  void instSetView();
//
//};
//
//
//
//
//#endif

