//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qlayout.h>
#include <qscrollview.h>
#include <qlayout.h>

#ifndef AVD_WIDGETLIST_HH
#include "avd_widget_list.hh"
#endif
#ifndef AVD_WIDGETLISTITEMCONTAINER_HH
#include "avd_widget_list_container.hh"
#endif


using namespace std;


avd_WidgetList::avd_WidgetList(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_vbox_layout = new QVBoxLayout(this);

  m_scroll_view = new QScrollView(this, "view");
  m_scroll_view->setResizePolicy(QScrollView::AutoOneFit);

  m_vbox_layout->addWidget(m_scroll_view);

  //m_container = new avd_WidgetListItemContainer();
  m_container = new avd_WidgetListContainer();
  //QVBoxLayout *scrollview_layout = new QVBoxLayout(m_scroll_view);
  m_scroll_view->addChild(m_container);
  //m_container->setMinimumSize( QSize( 20, 20 ) );
  //m_container->show();
  //scrollview_layout->addWidget(m_container);
}

void
avd_WidgetList::insertItem(
  QWidget *newChild
)
{
  m_container->insertItem(newChild);
}

