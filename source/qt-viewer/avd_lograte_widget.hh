//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_LOGRATE_WIDGET_HH
#define AVD_LOGRATE_WIDGET_HH


#include <qwidget.h>


class avd_RateValidator2;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QSlider;
class QSpacerItem;
class QString;
class QVBoxLayout;


class avd_LogRateWidget : public QWidget
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
  double m_scaling_factor;
  double m_min_rate;
  double m_last_rate;

public:
  avd_LogRateWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  ~avd_LogRateWidget(void);
protected:
  void setupLayout(void);
  void setupConnects(void);
public:
  void setLabel(const QString &text);
  void setMinRate(double max_rate);
  void setRate(double rate);
  void setLineStep(double exp_step);
  void setPageStep(double exp_step);
  void setRateValidator(avd_RateValidator2 *validator);
  double minRate(void);
  double rate(void);
  bool hasFocus(void);
signals:
  void rateChanged(double);
protected slots:
  void valueChangedSlot(int value);
  void returnPressedSlot(void);
};


#endif /* !AVD_LOGRATE_WIDGET_HH */

