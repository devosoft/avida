//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////


#include <math.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qstring.h>

#include "avd_rate_validator2.hh"

#include "avd_lograte_widget.hh"


avd_LogRateWidget::avd_LogRateWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  //cout << "<avd_LogRateWidget> constructor.\n";

  setupLayout();
  setupConnects();
}

avd_LogRateWidget::~avd_LogRateWidget(void)
{
  //cout << "<~avd_LogRateWidget> destructor.\n";
}

void
avd_LogRateWidget::setupLayout(void){
  //cout << "<avd_LogRateWidget::setupLayout> entered.\n";

  m_vbox_layout = new QVBoxLayout(
    this,
    0,
    6,
    "<avd_LogRateWidget::m_vbox_layout>"
  );

  m_title_label = new QLabel(this);
  m_vbox_layout->addWidget(m_title_label);

  m_rate_line_edit = new QLineEdit(this);
  m_vbox_layout->addWidget(m_rate_line_edit);

  m_hbox_layout = new QHBoxLayout(0);
  m_vbox_layout->addLayout(m_hbox_layout);

  m_spacer1 = new QSpacerItem(
    20,
    20,
    QSizePolicy::Expanding,
    QSizePolicy::Minimum
  );
  m_hbox_layout->addItem(m_spacer1);

  m_rate_slider = new QSlider(this);
  m_rate_slider->setSizePolicy(
    QSizePolicy(
      (QSizePolicy::SizeType)0,
      (QSizePolicy::SizeType)7,
      0, 0,
      m_rate_slider->sizePolicy().hasHeightForWidth()
    )
  );
  m_rate_slider->setOrientation(QSlider::Vertical);
  m_hbox_layout->addWidget(m_rate_slider);

  m_spacer2 = new QSpacerItem(
    20,
    20,
    QSizePolicy::Expanding,
    QSizePolicy::Minimum
  );
  m_hbox_layout->addItem(m_spacer2);

  m_rate_slider->setMinValue(0);
  m_rate_slider->setMaxValue(INT_MAX);
}

void
avd_LogRateWidget::setupConnects(void){
  //cout << "<avd_LogRateWidget::setupConnects> entered.\n";

  connect(
    m_rate_slider, SIGNAL(valueChanged(int)),
    this, SLOT(valueChangedSlot(int))
  );
  connect(
    m_rate_line_edit, SIGNAL(returnPressed(void)),
    this, SLOT(returnPressedSlot(void))
  );
}

void
avd_LogRateWidget::setLabel(const QString &text)
{
  //cout << "<avd_LogRateWidget::setLabel> entered.\n";

  m_title_label->setText(text);
}

void
avd_LogRateWidget::setMinRate(double min_rate)
{
  m_min_rate = min_rate;
  m_scaling_factor = m_rate_slider->maxValue() / log10(min_rate);
}

void
avd_LogRateWidget::setRate(double rate){
  //cout << "<avd_LogRateWidget::setRate> entered.\n";

  m_last_rate = rate;
  m_rate_slider->setValue(
    (rate < m_min_rate)?
      (m_rate_slider->maxValue()):
      ((int)(m_scaling_factor * log10(rate)))
  );
}

void
avd_LogRateWidget::setLineStep(double exp_step)
{
  m_rate_slider->setLineStep((int)(m_scaling_factor * exp_step));
}

void
avd_LogRateWidget::setPageStep(double exp_step)
{
  m_rate_slider->setPageStep((int)(m_scaling_factor * exp_step));
}

void
avd_LogRateWidget::setRateValidator(avd_RateValidator2 *validator)
{
  m_rate_line_edit->setValidator(validator);
}


double
avd_LogRateWidget::minRate(void)
{
  return m_min_rate;
}

double
avd_LogRateWidget::rate(void)
{
  return m_last_rate;
}

bool
avd_LogRateWidget::hasFocus(void)
{
  return m_rate_line_edit->hasFocus();
}

/* side effect:  changes member variable m_last_rate.  */
void
avd_LogRateWidget::valueChangedSlot(int value){
  m_last_rate = (value == m_rate_slider->maxValue())?
    (0):(pow(10.0, value / m_scaling_factor));

  m_rate_line_edit->setText(QString("%1").arg(m_last_rate));

  emit rateChanged(m_last_rate);
}

void
avd_LogRateWidget::returnPressedSlot(void){
  //cout << "<avd_LogRateWidget::returnPressedSlot> entered.\n";

  m_rate_line_edit->clearFocus();
  
  bool sigs_were_blocked = m_rate_slider->signalsBlocked();

  m_rate_slider->blockSignals(true);
  /* next line changes member variable m_last_rate.  */
  setRate(m_rate_line_edit->text().toDouble());
  m_rate_slider->blockSignals(sigs_were_blocked);

  emit rateChanged(m_last_rate);
}

