//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_HIDESHOWBUTTON_HH
#define AVD_HIDESHOWBUTTON_HH

#include <qhbox.h>


class QWidgetStack;
class QToolButton;
class QIconSet;


class avd_HideShowButton : public QHBox{
  Q_OBJECT
protected:
  QWidgetStack *m_widget_stack;
  QToolButton *m_rightfacing_button;
  QToolButton *m_downfacing_button;
  bool m_down;
public:
  /**
   * avd_HideShowButton constructor.
   *
   * @param parent  The parent avd_WidgetListItem.
   * @param name  Qt identifier; used in Qt debugging messages and Designer.
   */
  avd_HideShowButton(QWidget *parent = 0, const char *name = 0);

  bool isDown(void) const { return m_down; }
  void setToggleButton(bool down);
protected slots:
  void rightfacingButtonClicked(void);
  void downfacingButtonClicked(void);
signals:
  void toggled(bool on);
};


#endif /* !AVD_HIDESHOWBUTTON_HH */
