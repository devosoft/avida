#ifndef AVD_ANALYZER_CREATOR_HH
#define AVD_ANALYZER_CREATOR_HH


#ifndef QOBJECT_H
#include <qobject.h>
#endif


class avd_o_AnalyzerCreatorData;
class avdMissionControl;

class avd_o_AnalyzerCreator : public QObject
{
  Q_OBJECT
public:
  avd_o_AnalyzerCreator(
    avdMissionControl *mc,
    QObject *parent = 0,
    const char *name = 0
  );
  ~avd_o_AnalyzerCreator(void);
  void wrapupInit(void);
public slots:
  void newAnalyzerSlot(void);
};


#endif

// arch-tag: header file for old analyzer gui creator
