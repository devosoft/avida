//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qlayout.h>

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HH
#include "message_display.hh"
#endif

#ifndef AVD_WIDGETLISTITEMCONTAINER_H
#include "avd_widget_list_item_container.hh"
#endif


using namespace std;


avd_WidgetListItemContainer::avd_WidgetListItemContainer(
  QWidget *parent,
  const char *name,
  WFlags f)
: QWidget(parent, name, f)
{
  m_vbox_layout = new QVBoxLayout(this);
}

void
avd_WidgetListItemContainer::insertItem(
  QWidget *newChild
)
{
  if(newChild == NULL){
    GenError(
      "<avd_WidgetListItemContainer::insertItem>\n"
      " --- tried to insert null child widget.\n"
    );

    return;
  }

  newChild->reparent(this, QPoint(), true);
  m_vbox_layout->addWidget(newChild);
}


