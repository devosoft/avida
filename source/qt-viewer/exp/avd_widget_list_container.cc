//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "avd_widget_list_item_container.hh"
#include <qlayout.h>

#include "avd_widget_list_container.hh"


#ifndef DEBUG
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#else
//# define LOCAL_DEBUG 1
//# define USE_LOCAL_STRINGS 1
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#endif

#if USE_LOCAL_STRINGS
# define DSTR
#else
# define DSTR(ignore...) ("")
#endif


using namespace std;


avd_WidgetListContainer::avd_WidgetListContainer(
  QWidget *parent,
  const char *name,
  WFlags f)
: QWidget(parent, name, f)
{
  QVBoxLayout *vbox_layout = new QVBoxLayout(this);
  m_subcontainer = new avd_WidgetListItemContainer(this);
  vbox_layout->addWidget(m_subcontainer);
  QSpacerItem* spacer = new QSpacerItem(
    0, 0,
    QSizePolicy::Minimum, QSizePolicy::MinimumExpanding
  );
  vbox_layout->addItem(spacer);
}

void
avd_WidgetListContainer::insertItem(
  QWidget *newChild
)
{
  m_subcontainer->insertItem(newChild);
}


