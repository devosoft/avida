//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef APOCALYPSE_WIDGET_HH
#define APOCALYPSE_WIDGET_HH


#include <qwidget.h>


class avd_RateValidator2;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;
class QSpacerItem;
class QString;
class QVBoxLayout;


class ApocalypseWidget : public QWidget
{
  Q_OBJECT

protected:
  QVBoxLayout *m_vbox_layout;
    QLabel *m_title_label;
    QLineEdit *m_rate_line_edit;
    QHBoxLayout *m_hbox_layout;
      QSpacerItem *m_spacer1;
      QSlider *m_rate_slider;
      QSpacerItem *m_spacer2;
    QPushButton *m_apocalypse_pushbutton;
  double m_scaling_factor;
  double m_max_rate;
  double m_last_rate;

public:
  ApocalypseWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  ~ApocalypseWidget(void);
protected:
  void setupLayout(void);
  void setupConnects(void);
public:
  void setLabel(const QString &text);
  void setButtonText(const QString &text);
  void setMaxRate(double max_rate);
  void setRate(double rate);
  void setLineStep(double frac_step);
  void setPageStep(double frac_step);
  void setRateValidator(avd_RateValidator2 *validator);
  double maxRate(void);
  double rate(void);
  void valueChangedSlot(int value);
signals:
  void clicked(void);
protected slots:
  void returnPressedSlot(void);
};


#endif /* !APOCALYPSE_WIDGET_HH */


