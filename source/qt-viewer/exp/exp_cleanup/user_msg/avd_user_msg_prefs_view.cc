#ifndef QLABEL_H
#include <qlabel.h>
#endif
#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QLISTVIEW_H
#include <qlistview.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QSPINBOX_H
#include <qspinbox.h>
#endif
#ifndef QTABWIDGET_H
#include <qtabwidget.h>
#endif

#ifndef AVD_USER_MSG_GUI_DBG_HH
#include "avd_user_msg_gui_dbg.hh"
#endif

#ifndef AVD_USER_MSG_PREFS_VIEW_HH
#include "avd_user_msg_prefs_view.hh"
#endif

avdUserMsgPrefsView::avdUserMsgPrefsView(
  QWidget *parent, const char *name, WFlags f)
:avdPrefsItemView(parent, name, f){
  Debug << "<avdUserMsgPrefsView> ctor...";
  QVBoxLayout *layout = new QVBoxLayout
  ( this, 0, -1, "<avdUserMsgPrefsView(layout)>");
    QTabWidget *tab_widget = new QTabWidget
    ( this, "<avdUserMsgPrefsView(tab_widget)>");
    layout->addWidget(tab_widget);
      QWidget *console_cfg = new QWidget
      ( tab_widget, "<avdUserMsgPrefsView(console_cfg)>");
      QVBoxLayout *console_layout = new QVBoxLayout
      ( console_cfg, 0, -1, "<avdUserMsgPrefsView(console_layout)>");
      tab_widget->addTab(console_cfg, "Appearance");
        QHBoxLayout *spinbox_layout = new QHBoxLayout
        ( console_layout, -1, "<avdUserMsgPrefsView(spinbox_layout)>");
          QLabel *spinbox_label = new QLabel
          ( "saved lines (25-1000):", console_cfg, "<avdUserMsgPrefsView(spinbox_label)>");
          spinbox_layout->addWidget(spinbox_label);
          m_spin_box = new QSpinBox
          ( 25, 1000, 5, console_cfg, "<avdUserMsgPrefsView(m_spin_box)>");
          spinbox_layout->addWidget(m_spin_box);
        QHBoxLayout *fontbtn_layout = new QHBoxLayout
        ( console_layout, -1, "<avdUserMsgPrefsView(fontbtn_layout)>");
          m_font_btn = new QPushButton
          ( "Choose font...", console_cfg, "<avdUserMsgPrefsView(m_font_btn)>");
          fontbtn_layout->addWidget(m_font_btn);
          QLabel *fontbtn_label = new QLabel
          ( console_cfg, "<avdUserMsgPrefsView(fontbtn_label)>");
          fontbtn_layout->addWidget(fontbtn_label);
      QWidget *dbg_cfg = new QWidget
      ( tab_widget, "<avdUserMsgPrefsView(dbg_cfg)>");
      QVBoxLayout *dbg_layout = new QVBoxLayout
      ( dbg_cfg, 0, -1, "<avdUserMsgPrefsView(dbg_layout)>");
      tab_widget->addTab(dbg_cfg, "Debugging");
        m_list_view = new QListView
        ( dbg_cfg, "<avdUserMsgPrefsView(m_list_view)>");
        dbg_layout->addWidget(m_list_view);
        m_list_view->addColumn("Software Module");
  Debug << "<avdUserMsgPrefsView> ctor done.";
}

// arch-tag: implementation file for user debug-message display preferences view
