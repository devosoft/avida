#ifndef AVD_HELP_VIEWER_HH
#define AVD_HELP_VIEWER_HH

#ifndef QGUARDEDPTR_H
#include <qguardedptr.h>
#endif
#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif


class QTextBrowser;
class QWidget;

class avd_MissionControl;


class avd_HelpViewer : public QMainWindow
{
  Q_OBJECT
private:
  avd_MissionControl &m_mission_control;
  QTextBrowser *m_text_browser;

public:
  avd_HelpViewer(
    avd_MissionControl &mission_control,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  ~avd_HelpViewer();

  QStringList getPath(void);

public slots:
  void setPath(const QStringList &path);
  void setSource(const QString &source);

  void helpContents();
  void helpGenesis();
  void helpInstructionSet();
  void helpEvents();
  void helpEnvironments();
  void helpAnalyzeMode();

signals:
  void userSettingsSig();
  void quitSig();
};


#else
#warning multiple inclusion of help_viewer.hh
#endif /* !AVD_HELP_VIEWER_HH */

/* vim: set ts=2 ai et: */
