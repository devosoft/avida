#ifndef AVD_PLOT_VIEWER_CONTROLLER_HH
#define AVD_PLOT_VIEWER_CONTROLLER_HH

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
//class avd_MainWindow;


class avd_PlotViewerController : public QObject
{ 
  Q_OBJECT
private:
  avd_MissionControl &m_mission_control;
public:
  avd_PlotViewerController(
    avd_MissionControl &mission_control,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_PlotViewerController();
  
  void wrapupInit(void);
public slots:
  void newPlotViewerSlot(void);
};    
      
      
#else
#warning multiple inclusion of avd_plot_viewer_controller.hh
#endif /* !AVD_PLOT_VIEWER_CONTROLLER_HH */
  
/* vim: set ts=2 ai et: */
