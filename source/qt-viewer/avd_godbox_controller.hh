#ifndef AVD_GODBOX_CONTROLLER_HH
#define AVD_GODBOX_CONTROLLER_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QOBJECT_H
#include <qobject.h>
#endif

class avd_MissionControl;
class GodBoxWidget2;


class avd_GodBoxController : public QObject
{
  Q_OBJECT
private:
  avd_MissionControl &m_mission_control;
  QGuardedPtr<GodBoxWidget2> m_godbox;
public:
  avd_GodBoxController(
    avd_MissionControl &mission_control,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_GodBoxController();

  void wrapupInit(void);
public slots:
  void newGodBoxWidgetSlot(void);
};



#endif /* !AVD_GODBOX_CONTROLLER_HH */
