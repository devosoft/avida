#ifndef QLINEEDIT_H
#include <qlineedit.h>
#endif
#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QTEXTEDIT_H
#include <qtextedit.h>
#endif

#ifndef AVD_DUMB_VIEW_HH
#include "avd_dumb_view.hh"
#endif


avdDumbView::avdDumbView( QWidget *parent, const char *name, WFlags f)
:QMainWindow(parent, name, f){
  QWidget *central = new QWidget(
    this, "<avdDumbView(central)>");
  setCentralWidget(central);
  QVBoxLayout *layout = new QVBoxLayout(
    central, 10, -1, "<avdDumbView(layout)>");
  m_start_btn = new QPushButton(
    "Start", central, "<avdDumbView::m_start_btn>");
  layout->addWidget(m_start_btn);
  m_pause_btn = new QPushButton(
    "Pause", central, "<avdDumbView::m_pause_btn>");
  layout->addWidget(m_pause_btn);
  m_update_btn = new QPushButton(
    "Update", central, "<avdDumbView::m_update_btn>");
  layout->addWidget(m_update_btn);
  m_step_btn = new QPushButton(
    "Step", central, "<avdDumbView::m_step_btn>");
  layout->addWidget(m_step_btn);
  QLabel *step_label = new QLabel(
    "Step Cell ID:", central, "<avdDumbView::step_label>");
  layout->addWidget(step_label);
  m_step_id_le = new QLineEdit(central, "<avdDumbView::m_step_id_le>");
  layout->addWidget(m_step_id_le);
  m_reset_btn = new QPushButton(
    "Reset", central, "<avdDumbView::m_reset_btn>");
  layout->addWidget(m_reset_btn);
  m_open_btn = new QPushButton(
    "Open", central, "<avdDumbView::m_open_btn>");
  layout->addWidget(m_open_btn);
  m_close_btn = new QPushButton(
    "Close", central, "<avdDumbView::m_close_btn>");
  layout->addWidget(m_close_btn);
  QLabel *msg_label = new QLabel(
    "DebugMessages:", central, "<avdDumbView::msg_label>");
  layout->addWidget(msg_label);
  m_msg_te = new QTextEdit(central, "<avdDumbView::m_msg_te>");
  m_msg_te->setReadOnly(true);
  layout->addWidget(m_msg_te);
}

// arch-tag: implementation file for dumb gui view
