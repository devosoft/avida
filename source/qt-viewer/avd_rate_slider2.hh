//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_RATE_SLIDER2_HH
#define AVD_RATE_SLIDER2_HH


#include <qwidget.h>


class QSlider;


class avd_RateSlider2 : public QObject
{
  Q_OBJECT
private:
  QSlider *m_slider;
  double m_scaling_factor;
public:
  avd_RateSlider2(QSlider *slider);
  void setMaxRate(double max_rate);
  double maxRate(void);
  void setRate(double);
  double rate(void);
  void setLineStep(double);
  void setPageStep(double);
signals:
  void rateChanged(double);
  void sliderMoved(double);
  void sliderReleased(void);
private slots:
  void valueChangedSlot(int value);
  void sliderMovedSlot(int value);
  void sliderReleasedSlot(void);
};


class avd_LogRateSlider2 : public QObject
{
  Q_OBJECT
private:
  QSlider *m_slider;
  double m_scaling_factor;
  double m_min_rate;
public:
  avd_LogRateSlider2(QSlider *slider);
  void setMinRate(double max_rate);
  double minRate(void);
  void setRate(double);
  double rate(void);
  void setLineStep(double);
  void setPageStep(double);
signals:
  void rateChanged(double);
  void sliderMoved(double);
  void sliderReleased(void);
private slots:
  void valueChangedSlot(int value);
  void sliderMovedSlot(int value);
  void sliderReleasedSlot(void);
};



#endif /* !AVD_RATE_SLIDER2_HH */

