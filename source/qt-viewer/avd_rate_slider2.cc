//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qslider.h>

#include <limits.h>
#include <math.h>

#include "avd_rate_slider2.hh"


avd_RateSlider2::avd_RateSlider2(QSlider *slider)
: QObject(slider), m_slider(slider)
{
  m_slider->setMinValue(0);
  m_slider->setMaxValue(INT_MAX);
  connect(
    m_slider, SIGNAL(valueChanged(int)),
    this, SLOT(valueChangedSlot(int))
  );
  connect(
    m_slider, SIGNAL(sliderMoved(int)),
    this, SLOT(sliderMovedSlot(int))
  );
  connect(
    m_slider, SIGNAL(sliderReleased(void)),
    this, SLOT(sliderReleasedSlot(void))
  );
}

void
avd_RateSlider2::setMaxRate(double max_rate)
{
  m_scaling_factor = m_slider->maxValue() / max_rate;
}

double
avd_RateSlider2::maxRate(void){
  return m_slider->maxValue() / m_scaling_factor;
}

void
avd_RateSlider2::setRate(double rate){
  m_slider->setValue(
    (int)(m_slider->maxValue() - (m_scaling_factor * rate))
  );
}

double
avd_RateSlider2::rate(void){
  return (m_slider->maxValue() - m_slider->value()) / m_scaling_factor;
}


void
avd_RateSlider2::setLineStep(double step)
{
  m_slider->setLineStep((int)(m_scaling_factor * step));
}

void
avd_RateSlider2::setPageStep(double step)
{
  m_slider->setPageStep((int)(m_scaling_factor * step));
}

void
avd_RateSlider2::valueChangedSlot(int value){
  emit rateChanged((m_slider->maxValue() - value) / m_scaling_factor);
}

void
avd_RateSlider2::sliderMovedSlot(int value){
  emit sliderMoved((m_slider->maxValue() - value) / m_scaling_factor);
}

void
avd_RateSlider2::sliderReleasedSlot(void){
  emit sliderReleased();
}





avd_LogRateSlider2::avd_LogRateSlider2(QSlider *slider)
: QObject(slider), m_slider(slider)
{
  m_slider->setMinValue(0);
  m_slider->setMaxValue(INT_MAX);
  connect(
    m_slider, SIGNAL(valueChanged(int)),
    this, SLOT(valueChangedSlot(int))
  );
  connect(
    m_slider, SIGNAL(sliderMoved(int)),
    this, SLOT(sliderMovedSlot(int))
  );
  connect(
    m_slider, SIGNAL(sliderReleased(void)),
    this, SLOT(sliderReleasedSlot(void))
  );
}

void
avd_LogRateSlider2::setMinRate(double min_rate)
{
  m_min_rate = min_rate;
  m_scaling_factor = m_slider->maxValue() / log10(min_rate);
}

double
avd_LogRateSlider2::minRate(void){
  return pow(10.0, m_slider->maxValue() / m_scaling_factor);
}

void
avd_LogRateSlider2::setRate(double rate){
  m_slider->setValue(
    (rate < m_min_rate)?
      (m_slider->maxValue()):
      ((int)(m_scaling_factor * log10(rate)))
  );
}

double
avd_LogRateSlider2::rate(void){
  int slider_value = m_slider->value();
  return (slider_value == m_slider->maxValue())?
    (0):(pow(10.0, slider_value / m_scaling_factor));
}

void
avd_LogRateSlider2::setLineStep(double exp_step)
{
  m_slider->setLineStep((int)(m_scaling_factor * exp_step));
}

void
avd_LogRateSlider2::setPageStep(double exp_step)
{
  m_slider->setPageStep((int)(m_scaling_factor * exp_step));
}

void
avd_LogRateSlider2::valueChangedSlot(int value){
  emit rateChanged(rate());
}

void
avd_LogRateSlider2::sliderMovedSlot(int value){
  emit sliderMoved(rate());
}

void
avd_LogRateSlider2::sliderReleasedSlot(void){
  emit sliderReleased();
}


