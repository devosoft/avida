#ifndef QLAYOUT_H
#include <qlayout.h>
#endif
#ifndef QLISTBOX_H
#include <qlistbox.h>
#endif
#ifndef QPUSHBUTTON_H
#include <qpushbutton.h>
#endif
#ifndef QWIDGETSTACK_H
#include <qwidgetstack.h>
#endif

#ifndef AVD_PREFS_DBG_HH
#include "avd_prefs_dbg.hh"
#endif

#ifndef AVD_PREFS_VIEW_HH
#include "avd_prefs_view.hh"
#endif


avdPrefsView::avdPrefsView(QWidget *parent, const char *name, WFlags f)
: QMainWindow(parent, name, f)
{
  QWidget *central = new QWidget(this, "<avdPrefsView(central)>");
  setCentralWidget(central);
    QVBoxLayout *central_layout = new QVBoxLayout
    ( central, 10, -1, "<avdPrefsView(central_layout)>");
      QHBoxLayout *top_sublayout = new QHBoxLayout
      ( central_layout, -1, "<avdPrefsView(top_sublayout)>");
      QHBoxLayout *settings_sublayout = new QHBoxLayout
      ( central_layout, -1, "<avdPrefsView(settings_sublayout)>");
        m_listbox = new QListBox(central, "<avdPrefsView(m_listbox)>");
      settings_sublayout->addWidget(m_listbox);
        m_widgetstack = new QWidgetStack
        ( central, "<avdPrefsView(m_widgetstack)>");       
      settings_sublayout->addWidget(m_widgetstack);
      //QHBoxLayout *buttons_sublayout = new QHBoxLayout
      //( central_layout, -1, "<avdPrefsView(buttons_sublayout)>");
      //  m_cancel_pb = new QPushButton
      //  ( "Cancel", central, "<avdPrefsView(m_cancel_pb)>");
      //buttons_sublayout->addWidget(m_cancel_pb);
      //  QSpacerItem *button_spacer = new QSpacerItem(10, 10);
      //buttons_sublayout->addItem(button_spacer);
      //  m_apply_pb = new QPushButton
      //  ( "Apply", central, "<avdPrefsView(m_apply_pb)>");
      //buttons_sublayout->addWidget(m_apply_pb);
      //  QPushButton *m_okay_pb = new QPushButton
      //  ( "Okay", central, "<avdPrefsView(m_okay_pb)>");
      //buttons_sublayout->addWidget(m_okay_pb);
}

// arch-tag: implementation file for preferences gui view
