//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_WIDGETLIST_HH
#define AVD_WIDGETLIST_HH

#include <qwidget.h>


//class avd_WidgetListItemContainer;
class avd_WidgetListContainer;
class QScrollView;
class QVBoxLayout;
class QHBoxLayout;


/**
 * The avd_WidgetList widget implements a top-level widget list.
 *
 * A widget list contains a scroll area which in turn (usually) contains
 * a top-level widget list item; a widget list has neither a title area
 * nor a hide/show button, but otherwise behaves like a
 * widget list item.
 *
 * A widget list item comprises a title area, a container area, and a
 * button to hide/show the container area.  The container area may hold
 * arbitrary widgets; these widgets are laid-out vertically.  In
 * particular the container area may contain other widget list items,
 * forming a collapsible hierarchial view of widgets.
 *
 * Note:  use the member function <pre>setCaption</pre> inherited from
 * <pre>QWidget</pre> to set the window title of the avd_WidgetList and
 * its subclasses.
 *
 * @short A scrollable collapsible container of other widgets.
 * @version $Id: avd_widget_list.hh,v 1.7 2003/06/04 18:35:14 goingssh Exp $
 * @see avd_WidgetListItem
 */
class avd_WidgetList : public QWidget {
  Q_OBJECT
public:
  /**
   * avd_WidgetList constructor.
   *
   * @param parent  The parent widget.  Usually NULL or omitted.
   * @param name  Qt identifier; used in Qt debugging messages and Designer.
   * @param f  Qt widget flags; typically 0 or omitted.
   */
  avd_WidgetList(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  /**
   * Inserts a new child widget into the container area.
   *
   * @param newChild  Any subclass of QWidget.
   */
  void insertItem(QWidget *newChild);
private:
  QVBoxLayout *m_vbox_layout;
  QScrollView *m_scroll_view;
  /**
   * The container area.
   *
   * @see avd_WidgetListItemContainer
   */
  avd_WidgetListContainer *m_container;
};


#endif /* !AVD_WIDGETLIST_HH */
