#ifndef AVD_DRIVER_CONTROLLER_HH
#define AVD_DRIVER_CONTROLLER_HH
  
#ifndef QOBJECT_H
#include <qobject.h>
#endif


/*
note: I started doing some memory management stuff here, then stopped
deciding that the function below should be called exactly once.

keep it in mind, considering proper handling if you decide to copy,
delete, create, etc. dynamically sometime in the future.

-- kgn
*/

class avd_MissionControl;
class avd_DriverControllerData;

class avd_DriverController : public QObject
{
  Q_OBJECT
private:
  avd_DriverControllerData *d;

public:
  avd_DriverController(
    avd_MissionControl *mission_control,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_DriverController();
  void wrapupInit(void);

public slots:
  void openAvidaSlot(void);
  void closeAvidaSlot(void);
  bool avidaIsRunning();
};


#else
#warning multiple inclusion of avd_driver_controller.hh
#endif /* !AVD_DRIVER_CONTROLLER_HH */
  
/* vim: set ts=2 ai et: */
