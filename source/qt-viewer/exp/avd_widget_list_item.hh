//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_WIDGETLISTITEM_HH
#define AVD_WIDGETLISTITEM_HH

#include <qwidget.h>


class avd_HideShowButton;
class avd_WidgetListItemContainer;
class QGridLayout;
class QLabel;


/**
 * The avd_WidgetListItem widget implements a widget list item.
 *
 * A widget list item comprises a title area, a container area, and a
 * button to hide/show the container area.  The container area may hold
 * arbitrary widgets; these widgets are laid-out vertically.  In
 * particular the container area may contain other widget list items,
 * forming a collapsible hierarchial view of widgets.
 *
 * @short A labeled collapsible container of other widgets.
 * @version $Id: avd_widget_list_item.hh,v 1.6 2003/06/04 18:35:14 goingssh Exp $
 * @see avd_WidgetList
 */
class avd_WidgetListItem : public QWidget{
  Q_OBJECT
public:
  /**
   * avd_WidgetListItem constructor.
   *
   * @param parent  The parent widget.  Often NULL or omitted.
   * @param name  Qt identifier; used in Qt debugging messages and Designer.
   * @param f  Qt widget flags; typically 0 or omitted.
   */
  avd_WidgetListItem(
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
  /**
   * Sets the text in the title area.
   *
   * @param title  The new title text.
   */
  void setLabelText(const QString &title);
  void setContainerVisibility(bool should_show);
private slots:
  /**
   * A private slot/method to hide/show the container area.  Connected to
   * the <pre>toggled</pre> signal of the avd_HideShowButton
   * <pre>m_hideshow.</pre>
   *
   * @param show  If true, container area is shown, and otherwise hidden.
   */
  void toggleContainerVisibility(bool show);
private:
  QGridLayout *m_grid_layout;
  /**
   * A toggle button which when clicked will hide or show the container
   * area.
   *
   * @see avd_HideShowButton
   */
  avd_HideShowButton *m_hideshow;
  /**
   * The title area.
   */
  QLabel *m_label;
  /**
   * The container area.
   *
   * @see avd_WidgetListItemContainer
   */
  avd_WidgetListItemContainer *m_container;
};


#endif /* !AVD_WIDGETLISTITEM_HH */
