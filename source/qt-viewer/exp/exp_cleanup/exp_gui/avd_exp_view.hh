#ifndef AVD_EXP_VIEW_HH
#define AVD_EXP_VIEW_HH

#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QPushButton;
class avdExpView : public QMainWindow, public cRetainable {
  Q_OBJECT
protected:
  QPushButton *m_scopeguard_test_btn;
public:
  avdExpView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  QPushButton *scopeguardTestBtn() { return m_scopeguard_test_btn; }
};

#endif

// arch-tag: header file for devel-experimentation gui view
