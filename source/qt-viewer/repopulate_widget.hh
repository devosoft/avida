//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef REPOPULATE_WIDGET_HH
#define REPOPULATE_WIDGET_HH


#include <qwidget.h>


class QComboBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;
class QSpacerItem;
class QString;
class QVBoxLayout;


class RepopulateWidget : public QWidget
{
  Q_OBJECT

protected:
  QVBoxLayout *m_vbox_layout;
    QLabel *m_title_label;
    QComboBox *m_combo_box;
    QSpacerItem *m_spacer;
    QPushButton *m_inject_pushbutton;
    QPushButton *m_inject_all_pushbutton;

public:
  RepopulateWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  ~RepopulateWidget(void);
protected:
  void setupLayout(void);
  void setupConnects(void);
public:
  void setLabel(const QString &text);
  void setInjectButtonText(const QString &text);
  void setInjectAllButtonText(const QString &text);
  QString currentText(void) const;
  void clear(void);
  void insertItem(const QString &t, int index = -1);
signals:
  void injectOneButtonClicked(void);
  void injectAllButtonClicked(void);
};


#endif /* !REPOPULATE_WIDGET_HH */

