//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qhbox.h>
#include <qwidgetstack.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qpixmap.h>
#include <qdragobject.h>

#ifndef AVD_HIDESHOWBUTTON_HH
#include "avd_hide_show_button.hh"
#endif


using namespace std;


avd_HideShowButton::avd_HideShowButton(QWidget *parent, const char *name)
//: QToolButton(parent, name)
: QHBox(parent, name)
{
  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum));
  m_widget_stack = new QWidgetStack(this);

  /* FIXME:  size of hide-show button should be configurable.  -- K.
  */
  m_widget_stack->setFixedSize(22, 20);

  m_rightfacing_button = new QToolButton(RightArrow, this);
  m_rightfacing_button->setToggleButton(FALSE);
  m_rightfacing_button->setAutoRaise(TRUE);
  connect(
    m_rightfacing_button, SIGNAL(clicked(void)),
    this, SLOT(rightfacingButtonClicked(void))
  );
  m_widget_stack->addWidget(m_rightfacing_button);

  m_downfacing_button = new QToolButton(DownArrow, this);
  m_downfacing_button->setToggleButton(FALSE);
  m_downfacing_button->setAutoRaise(TRUE);
  connect(
    m_downfacing_button, SIGNAL(clicked(void)),
    this, SLOT(downfacingButtonClicked(void))
  );
  m_widget_stack->addWidget(m_downfacing_button);

  setToggleButton(false);
}

void
avd_HideShowButton::setToggleButton(bool down){
  m_down = down;
  emit toggled(down);
  if(down){
    m_widget_stack->raiseWidget(m_downfacing_button);
  } else {
    m_widget_stack->raiseWidget(m_rightfacing_button);
  }
}

void
avd_HideShowButton::rightfacingButtonClicked(void){
  setToggleButton(true);
}

void
avd_HideShowButton::downfacingButtonClicked(void){
  setToggleButton(false);
}


