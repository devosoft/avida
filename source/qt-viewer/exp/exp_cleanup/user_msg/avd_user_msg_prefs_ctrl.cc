#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QFONTDIALOG_H
#include <qfontdialog.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QSETTINGS_H
#include <qsettings.h>
#endif
#ifndef QSPINBOX_H
#include <qspinbox.h>
#endif
#ifndef QTEXTEDIT_H
#include <qtextedit.h>
#endif

#ifndef DEFS_HH
#include "defs.hh"
#endif

#ifndef AVD_PREFS_ITEM_VIEW_HH
#include "preferences/avd_prefs_item_view.hh"
#endif
#ifndef AVD_USER_MSG_CTRL_HH
#include "avd_user_msg_ctrl.hh"
#endif
#ifndef AVD_USER_MSG_GUI_DBG_HH
#include "avd_user_msg_gui_dbg.hh"
#endif
#ifndef AVD_USER_MSG_PREFS_VIEW_HH
#include "avd_user_msg_prefs_view.hh"
#endif
#ifndef AVD_USER_MSG_VIEW_HH
#include "avd_user_msg_view.hh"
#endif

#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_HH
#include "user_msg/avd_message_display_typetrack.hh"
#endif

#ifndef AVD_USER_MSG_PREFS_CTRL_HH
#include "avd_user_msg_prefs_ctrl.hh"
#endif


avdMsgModuleLVI::avdMsgModuleLVI(QListView *parent, QString label)
: QListViewItem(parent, label)
{
  setExpandable(true);
}
avdMsgTypeLVI::avdMsgTypeLVI(
  avdMsgModuleLVI *parent,
  const QString &parent_text,
  const QString &text,
  avdMessageType &mt)
: QListViewItem(parent, text)
{
  QString settings_key('/' + parent_text + '/' + text);
  setExpandable(true);
  avdMsgCnfgCLI *is_active = new avdMsgCnfgCLI
  ( this, "Is Active", settings_key + "/is_active", mt.m_is_active);
  avdMsgCnfgCLI *show_where = new avdMsgCnfgCLI
  ( this, "Show Where", settings_key + "/show_where", mt.m_show_where);
  avdMsgCnfgCLI *show_function = new avdMsgCnfgCLI
  ( this, "Show Function", settings_key + "/show_function", mt.m_show_function);
}

avdMsgCnfgCLI::avdMsgCnfgCLI(
  avdMsgTypeLVI *parent,
  const QString &text,
  const QString &key,
  bool &flag)
: QCheckListItem(parent, text, QCheckListItem::CheckBox)
, m_flag(flag)
, m_key(key)
{
  QSettings settings;
  settings.setPath("Avida", AVIDA_VERSION);
  settings.beginGroup("/avdUserMsgPrefs/avdMessageTypeSet");
  m_flag = settings.readBoolEntry(m_key, m_flag);
  setOn(m_flag);
}
void avdMsgCnfgCLI::stateChange(bool state){
  m_flag = state;
  QSettings settings;
  settings.setPath("Avida", AVIDA_VERSION);
  settings.writeEntry("/avdUserMsgPrefs/avdMessageTypeSet" + m_key, state);
};

void avdUserMsgPrefsCtrl::setView(avdUserMsgPrefsView *view)
{ SETretainable(m_view, view); }
void avdUserMsgPrefsCtrl::setConsoleFont(QFont console_font)
{ m_console_font = console_font; }
QWidget *avdUserMsgPrefsCtrl::getView()
{ return m_view; }
avdUserMsgPrefsView *avdUserMsgPrefsCtrl::getUserMsgPrefsView()
{ return m_view; }

avdUserMsgPrefsCtrl::avdUserMsgPrefsCtrl(
  QObject *parent, const char *name)
: QObject(parent, name), m_view(0)
{
  Debug << "<avdUserMsgPrefsCtrl> ctor.";
  avdUserMsgPrefsView *view = new avdUserMsgPrefsView(
    0, "<avdUserMsgPrefsCtrl(m_view)>");
  Debug << " --- created view at " << getView();
  setView(view);
  Debug << " --- setPrefsItemView() done...";
  if(!getView())
  { Error << "can't create avdUserMsgPrefsView."; return; }
  Debug << " --- setPrefsItemView() non-null; done...";
  view->nameWatched("<avdUserMsgPrefsCtrl::(view)>");
  view->release();
  connect(getUserMsgPrefsView()->fontBtn(), SIGNAL(clicked()),
    this, SLOT(fontBtnClickedSlot()));
  connect(getUserMsgPrefsView()->spinBox(), SIGNAL(valueChanged(int)),
    this, SLOT(spinBoxValueChangedSlot(int)));
}
avdUserMsgPrefsCtrl::~avdUserMsgPrefsCtrl(){
  Debug << "<avdUserMsgPrefsCtrl> dtor entered.";
  setView(0);
  Debug << "<avdUserMsgPrefsCtrl> dtor done.";
}
void avdUserMsgPrefsCtrl::setup()
{
  Debug << "<avdUserMsgPrefsCtrl::setup()>.";

  if(!getUserMsgPrefsView()){
    Error << "Can't obtain user message preferences viewer.";
  } else {
    QSettings settings;
    settings.setPath("Avida", AVIDA_VERSION);
    settings.beginGroup("/avdUserMsgPrefs");
    { int saved_lines = settings.readNumEntry("/saved_lines", 1000);
      if((saved_lines < 5) || (1000 < saved_lines)) saved_lines = 1000;
      setSavedLines(saved_lines);
      getUserMsgPrefsView()->spinBox()->setValue(savedLines()); }
    { QString console_font_str
        = settings.readEntry("/console_font", QApplication::font().toString());
      QFont console_font;
      bool font_available = console_font.fromString(console_font_str);
      setConsoleFont(console_font);
    }
    settings.endGroup();

    for(
      avdMessageSetTrackerIterator
      stit = avdMessageSetTracker::Instance().begin(),
      stend = avdMessageSetTracker::Instance().end()
    ; stit != stend; ++stit){
      Debug << stit->first << ":" << stit->second->size();
      avdMsgModuleLVI *mmlvi = new avdMsgModuleLVI(getUserMsgPrefsView()->listView(), stit->first);
      for(
        avdMessageTypeIterator
        tit = stit->second->begin(),
        tend = stit->second->end()
      ; tit != tend; ++tit){
        Debug << "  " << tit->first;
        avdMsgTypeLVI *mtcli = new avdMsgTypeLVI(mmlvi, stit->first, tit->first, *tit->second);
      }
    }
  }
}
void avdUserMsgPrefsCtrl::consoleFontFunctor(const avdUserMsgCtrl &user_msg_ctrl){
  Debug << "<avdUserMsgPrefsCtrl::consoleFontFunctor(...)> entered.";
  user_msg_ctrl.getView()->textEdit()->setCurrentFont(consoleFont());
  Debug << "<avdUserMsgPrefsCtrl::consoleFontFunctor(...)> done.";
}
void avdUserMsgPrefsCtrl::savedLinesFunctor(const avdUserMsgCtrl &user_msg_ctrl){
  Debug << "<avdUserMsgPrefsCtrl::savedLinesFunctor(const avdUserMsgCtrl &)> entered.";
  user_msg_ctrl.setSavedLines(savedLines());
  Debug << "<avdUserMsgPrefsCtrl::savedLinesFunctor(...)> done.";
}
void avdUserMsgPrefsCtrl::requestUserMsgPrefsSlot(const avdUserMsgCtrl &ctrl){
  consoleFontFunctor(ctrl);
  savedLinesFunctor(ctrl);
}
void avdUserMsgPrefsCtrl::fontBtnClickedSlot(){
  Debug << "<avdUserMsgPrefsCtrl::fontBtnClickedSlot()>";
  bool ok;
  QFont console_font = QFontDialog::getFont(&ok, consoleFont(), getView());
  if(ok){
    QSettings settings;
    settings.setPath("Avida", AVIDA_VERSION);
    settings.writeEntry("/avdUserMsgPrefs/console_font", console_font.toString());
    setConsoleFont(console_font);
    avdUserMsgCtrlFunct funct(this, &avdUserMsgPrefsCtrl::consoleFontFunctor);
    emit(userMsgPrefsChangedSig(funct));
  }
  Debug << "<avdUserMsgPrefsCtrl::fontBtnClickedSlot()> done.";
}
void avdUserMsgPrefsCtrl::spinBoxValueChangedSlot(int value){
  Debug << "<avdUserMsgPrefsCtrl::spinBoxValueChangedSlot()>";
  setSavedLines(value);
  QSettings settings;
  settings.setPath("Avida", AVIDA_VERSION);
  settings.writeEntry("/avdUserMsgPrefs/saved_lines", savedLines());
  avdUserMsgCtrlFunct funct(this, &avdUserMsgPrefsCtrl::savedLinesFunctor);
  emit(userMsgPrefsChangedSig(funct));
  Debug << "<avdUserMsgPrefsCtrl::spinBoxValueChangedSlot()> done.";
}

// arch-tag: implementation file for user debug-message display gui preferences controller
