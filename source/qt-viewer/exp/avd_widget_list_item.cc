//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qvbox.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qscrollview.h>

#ifndef AVD_WIDGETLISTITEM_HH
#include "avd_widget_list_item.hh"
#endif

#ifndef AVD_HIDESHOWBUTTON_HH
#include "avd_hide_show_button.hh"
#endif

#ifndef AVD_WIDGETLISTITEMCONTAINER_HH
#include "avd_widget_list_item_container.hh"
#endif


using namespace std;


avd_WidgetListItem::avd_WidgetListItem(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));

  m_grid_layout = new QGridLayout(this);
  
  m_hideshow = new avd_HideShowButton(this, "hideshow");
  m_grid_layout->addWidget(m_hideshow, 0, 0);

  m_label = new QLabel("a label", this, "field");
  m_grid_layout->addWidget(m_label, 0, 1);

  //m_label->setAlignment(Qt::AlignHCenter);
  //m_label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  m_label->setMinimumWidth(20*fontMetrics().width("x"));
  m_label->setMinimumHeight(fontMetrics().height());
  m_label->show();
  
  m_container = new avd_WidgetListItemContainer(this);
  m_grid_layout->addWidget(m_container, 1, 1);
  m_container->hide();

  connect(
    m_hideshow, SIGNAL(toggled(bool)),
    this, SLOT(toggleContainerVisibility(bool))
  );
}

void
avd_WidgetListItem::insertItem(
  QWidget *newChild
)
{
  m_container->insertItem(newChild);
}

void
avd_WidgetListItem::setLabelText(const QString &text)
{
  m_label->setText("<b>" + text + "</b>");
}

void
avd_WidgetListItem::setContainerVisibility(bool should_show){
  m_hideshow->setToggleButton(should_show);
}

void avd_WidgetListItem::toggleContainerVisibility(bool show)
{
  if(show){
    m_container->show();
  } else {
    m_container->hide();
  }
}


