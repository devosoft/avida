#ifndef AVD_INSTRUCTIONS_VIEW_CONTROLLER_HH
#define AVD_INSTRUCTIONS_VIEW_CONTROLLER_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QOBJECT_H
#include <qobject.h>
#endif

class avd_MissionControl;
class N_Instruction_Viewer;


class avd_InstructionViewerController : public QObject
{
  Q_OBJECT
private:
  avd_MissionControl &m_mission_control;
  QGuardedPtr<N_Instruction_Viewer> m_instruction_viewer;
public:
  avd_InstructionViewerController(
    avd_MissionControl &mission_control,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_InstructionViewerController();

  void wrapupInit(void);
public slots:
  void newInstructionViewerSlot(void);
};



#endif /* !AVD_INSTRUCTIONS_VIEW_CONTROLLER_HH */
