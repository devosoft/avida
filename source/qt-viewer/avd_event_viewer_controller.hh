#ifndef AVD_EVENT_VIEWER_CONTROLLER_HH
#define AVD_EVENT_VIEWER_CONTROLLER_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QOBJECT_H
#include <qobject.h>
#endif

class avd_MissionControl;
class EventViewWidget;


class avd_EventViewerController : public QObject
{
  Q_OBJECT
private:
  avd_MissionControl &m_mission_control;
  QGuardedPtr<EventViewWidget> m_event_view_widget;
public:
  avd_EventViewerController(
    avd_MissionControl &mission_control,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_EventViewerController();

  void wrapupInit(void);
public slots:
  void newEventViewWidgetSlot(void);
};



#endif /* !AVD_EVENT_VIEWER_CONTROLLER_HH */
