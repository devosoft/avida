//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_WIDGETLISTITEMCONTAINER_H
#define AVD_WIDGETLISTITEMCONTAINER_H

#include <qwidget.h>


class QVBoxLayout;


/**
 * The avd_WidgetListItemContainer widget is part of the
 * avd_WidgetListItem widget, and is used to hold arbitrary widgets in a
 * vertical layout.
 *
 * @short Part of the avd_WidgetListItem.
 * @version $Id: avd_widget_list_item_container.hh,v 1.6 2003/06/04 18:35:14 goingssh Exp $
 * @see avd_WidgetListItem
 * @see avd_WidgetList
 */
class avd_WidgetListItemContainer : public QWidget {

  Q_OBJECT

public:

  /**
   * avd_WidgetListItemContainer constructor.
   *
   * @param parent  The parent avd_WidgetListItem.
   * @param name  Qt identifier; used in Qt debugging messages and Designer.
   * @param f  Qt widget flags; typically 0 or omitted.
   */
  avd_WidgetListItemContainer(
    QWidget * parent = 0,
    const char *name = 0,
    WFlags f = 0
  );

  /**
   * Inserts a new child widget into the container area.  Usually called
   * from the parent avd_WidgetListItem.
   *
   * @param newChild  Any subclass of QWidget.
   */
  void insertItem(QWidget *newChild);

private:

  QVBoxLayout *m_vbox_layout;

};


#endif /* !AVD_WIDGETLISTITEMCONTAINER_H */

