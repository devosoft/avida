

#ifndef AVD_HELP_CONTROLLER_HH
#include "avd_help_controller.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif
#ifndef AVD_HELP_VIEWER_HH
#include "avd_help_viewer.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QOBJECTLIST_H
#include <qobjectlist.h>
#endif
#ifndef QFILEINFO_H
#include <qfileinfo.h>
#endif
#ifndef QDIR_H
#include <qdir.h>
#endif


#ifndef DEFS_HH
#include "defs.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


avd_HelpController::avd_HelpController(
  avd_MissionControl &mission_control,
  QObject *parent,
  const char *name
):QObject(parent, name),
  m_mission_control(mission_control)
{
  GenDebug("entered.");

  /* FIXME:  replace w/QSettings stuff. */
#ifdef PLATFORM_IS_MAC_OS_X
  /* FIXME:  handle errors bitte! -- kgn */
  bool worked;
  QFileInfo app_fi(qApp->argv()[0]);
  QDir app_dir(app_fi.dir(false));
  app_dir.cd("../Resources/html");
  setPathSlot(app_dir.path());
  GenDebug << " --- using basepath: " << app_dir.path();
#else
  setPathSlot(QString(HTMLDIR));
  GenDebug << " --- using basepath: " << QString(HTMLDIR);
#endif



  connect(
    &mission_control, SIGNAL(helpURLSig(const QString &)),
    this, SLOT(helpURLSlot(const QString &))
  );

  GenDebug("done.");
}

avd_HelpController::~avd_HelpController(){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_HelpController::wrapupInit(void){
  GenDebug("entered.");
  GenDebug("done.");
}


void
avd_HelpController::helpURLSlot(const QString &rel_path){
  GenDebug("entered; using relpath \"")(rel_path)('\"');

  if(!m_help_viewer){
    m_help_viewer = new avd_HelpViewer(
      m_mission_control,
      0,
      "<avd_HelpController::m_help_viewer>"
    );

    /*
    FIXME:  hard-coded ugly fixup for QTextBrowser bug: files in
    "glossary" subdirectory should be able to refer to each other
    without prepending "glossary" to names.  This ugly fix is
    hard-coded.  yuk.  -- kgn
    */
    QStringList viewer_search_paths;
    viewer_search_paths += m_base_path;
    viewer_search_paths += (m_base_path + "/glossary");

    m_help_viewer->setPath(viewer_search_paths);

    m_help_viewer->setCaption("avd_HelpViewer");
    if(
      QApplication::desktop()->width() > 640
      &&
      QApplication::desktop()->height() > 480
    ){
      m_help_viewer->resize(640, 480);
      m_help_viewer->show();
    } else {
      m_help_viewer->showMaximized();
    }
    m_help_viewer->setSource("index.html");
  }
  m_help_viewer->show();
  m_help_viewer->raise();
  m_help_viewer->setSource(rel_path);

  //QFileInfo qfi(QDir(m_help_viewer->getPath().first()), rel_path);
  //GenDebug("qfi.filePath(): ")(qfi.filePath());
  //GenDebug("qfi.fileName(): ")(qfi.fileName());
  //GenDebug("qfi.absFilePath(): ")(qfi.absFilePath());
  //GenDebug("qfi.baseName(true): ")(qfi.baseName(true));
  //GenDebug("qfi.extension(true): ")(qfi.extension(true));
  //GenDebug("qfi.dirPath(true): ")(qfi.dirPath(true));
  //GenDebug("qfi.baseName(false): ")(qfi.baseName(false));
  //GenDebug("qfi.extension(false): ")(qfi.extension(false));
  //GenDebug("qfi.dirPath(false): ")(qfi.dirPath(false));

  //QDir qdt(qfi.dir(true));
  //GenDebug("qdt.path(): ")(qdt.path());
  //GenDebug("qdt.absPath(): ")(qdt.absPath());
  //GenDebug("qdt.canonicalPath(): ")(qdt.canonicalPath());
  //GenDebug("qdt.dirName(): ")(qdt.dirName());

  //QDir qdf(qfi.dir(false));
  //GenDebug("qdf.path(): ")(qdf.path());
  //GenDebug("qdf.absPath(): ")(qdf.absPath());
  //GenDebug("qdf.canonicalPath(): ")(qdf.canonicalPath());
  //GenDebug("qdf.dirName(): ")(qdf.dirName());

  //GenDebug("QDir::currentDirPath(): ")(QDir::currentDirPath());

  //GenDebug("qApp->argv()[0]: ")(qApp->argv()[0]);

  GenDebug("done.");
}

void
avd_HelpController::setPathSlot(const QString &base_path){
  GenDebug("entered.");

  m_base_path = base_path;

  GenDebug("done.");
}

/* vim: set ts=2 ai et: */
