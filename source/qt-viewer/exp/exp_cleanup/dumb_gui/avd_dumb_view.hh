#ifndef AVD_DUMB_VIEW_HH
#define AVD_DUMB_VIEW_HH

#ifndef QMAINWINDOW_H
#include <qmainwindow.h>
#endif
#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

class QLineEdit;
class QPushButton;
class QTextEdit;
class avdDumbView : public QMainWindow, public cRetainable {
  Q_OBJECT
protected:
  QPushButton *m_start_btn;
  QPushButton *m_pause_btn;
  QPushButton *m_update_btn;
  QPushButton *m_step_btn;
  QPushButton *m_reset_btn;
  QPushButton *m_open_btn;
  QPushButton *m_close_btn;
  QLineEdit *m_step_id_le;
  QTextEdit *m_msg_te;
public:
  avdDumbView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WType_TopLevel | WDestructiveClose
  );
  QPushButton *startBtn() { return m_start_btn; }
  QPushButton *pauseBtn() { return m_pause_btn; }
  QPushButton *updateBtn() { return m_update_btn; }
  QPushButton *stepBtn() { return m_step_btn; }
  QPushButton *resetBtn() { return m_reset_btn; }
  QPushButton *openBtn() { return m_open_btn; }
  QPushButton *closeBtn() { return m_close_btn; }
  QLineEdit *stepIdLineEd() { return m_step_id_le; }
  QTextEdit *msgTextEd() { return m_msg_te; }
};

#endif

// arch-tag: header file for dumb gui view
