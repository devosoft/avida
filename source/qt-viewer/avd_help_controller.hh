#ifndef AVD_HELP_CONTROLLER_HH
#define AVD_HELP_CONTROLLER_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QWIDGET_H
#include <qwidget.h>
#endif


class avd_MissionControl;
class avd_HelpViewer;


class avd_HelpController : public QObject
{
  Q_OBJECT
private:
  avd_MissionControl &m_mission_control;
  QGuardedPtr<avd_HelpViewer> m_help_viewer;
  QString m_base_path;
public:
  avd_HelpController(
    avd_MissionControl &mission_control,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_HelpController();

  void wrapupInit(void);
public slots:
  void helpURLSlot(const QString &rel_path);
  void setPathSlot(const QString &base_path);
};


#else
#warning multiple inclusion of help_controller.hh
#endif /* !AVD_HELP_CONTROLLER_HH */

/* vim: set ts=2 ai et: */

