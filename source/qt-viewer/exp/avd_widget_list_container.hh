//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_WIDGETLISTCONTAINER_H
#define AVD_WIDGETLISTCONTAINER_H

#include <qwidget.h>


class QVBoxLayout;
class avd_WidgetListItemContainer;


class avd_WidgetListContainer : public QWidget {

  Q_OBJECT

public:

  avd_WidgetListContainer(
    QWidget * parent = 0,
    const char *name = 0,
    WFlags f = 0
  );

  void insertItem(QWidget *newChild);

private:

  avd_WidgetListItemContainer *m_subcontainer;

};


#endif /* !AVD_WIDGETLISTCONTAINER_H */

