#ifndef AVD_PREFS_VIEW_HH
#define AVD_PREFS_VIEW_HH

#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QListBox;
class QPushButton;
class QWidgetStack;
class avdPrefsView : public QMainWindow, public cRetainable {
  Q_OBJECT
protected:
  QListBox *m_listbox;
  //QPushButton *m_cancel_pb;
  //QPushButton *m_apply_pb;
  //QPushButton *m_okay_pb;
  QWidgetStack *m_widgetstack;
public:
  avdPrefsView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel
  );
  QListBox *listBox() { return m_listbox; }
  //QPushButton *cancelBtn() { return m_cancel_pb; }
  //QPushButton *applyBtn() { return m_apply_pb; }
  //QPushButton *okayBtn() { return m_okay_pb; }
  QWidgetStack *widgetStack() { return m_widgetstack; }
};

#endif

// arch-tag: header file for preferences gui view
