#ifndef AVD_MAIN_WINDOW_CONTROLLER_HH
#define AVD_MAIN_WINDOW_CONTROLLER_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QVALUELIST_H
#include <qvaluelist.h>
#endif
#ifndef QOBJECT_H
#include <qobject.h>
#endif

class avd_MissionControl;
class avd_MainWindow;

typedef QGuardedPtr<avd_MainWindow> avd_MainWindowGuardedPtr;
typedef QValueList<avd_MainWindowGuardedPtr> avd_MainWindowList;


class avd_MainWindowController : public QObject
{ 
  Q_OBJECT
private:
  avd_MissionControl &m_mission_control;
  avd_MainWindowList m_main_window_list;
public:
  avd_MainWindowController(
    avd_MissionControl &mission_control,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_MainWindowController();
  
  void wrapupInit(void);
public slots:
  void closeEventSlot(avd_MainWindow *main_window);
  void confirmQuit(void);
  void newMainWindowSlot(void);
signals:
  void quitSig(void);
};    
      
      
#else
#warning multiple inclusion of avd_main_window_controller.hh
#endif /* !AVD_MAIN_WINDOW_CONTROLLER_HH */
  
/* vim: set ts=2 ai et: */

