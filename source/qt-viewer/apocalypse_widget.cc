//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qstring.h>

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#include "avd_rate_validator2.hh"

#include "apocalypse_widget.hh"


using namespace std;


ApocalypseWidget::ApocalypseWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  GenDebug("<ApocalypseWidget> constructor.\n");

  setupLayout();
  setupConnects();
}

ApocalypseWidget::~ApocalypseWidget(void)
{
  GenDebug("<ApocalypseWidget> destructor.\n");
}

void
ApocalypseWidget::setupLayout(void)
{
  GenDebug("<ApocalypseWidget::setupLayout> entered.\n");

  m_vbox_layout = new QVBoxLayout(
    this,
    0,
    6,
    "<ApocalypseWidget::m_vbox_layout>"
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

  m_apocalypse_pushbutton = new QPushButton(
    this
  );
  m_vbox_layout->addWidget(m_apocalypse_pushbutton);
}

void
ApocalypseWidget::setupConnects(void)
{
  GenDebug("<ApocalypseWidget::setupConnects> entered.\n");

  connect(
    m_rate_slider, SIGNAL(valueChanged(int)),
    this, SLOT(valueChangedSlot(int))
  );
  connect(
    m_rate_line_edit, SIGNAL(returnPressed(void)),
    this, SLOT(returnPressedSlot(void))
  );
  connect(
    m_apocalypse_pushbutton, SIGNAL(clicked(void)),
    this, SIGNAL(clicked(void))
  );
}

void
ApocalypseWidget::setLabel(const QString &text)
{
  GenDebug("<ApocalypseWidget::setLabel> entered.\n");

  m_title_label->setText(text);
}

void
ApocalypseWidget::setButtonText(const QString &text)
{
  GenDebug("<ApocalypseWidget::setButtonText> entered.\n");

  m_apocalypse_pushbutton->setText(text);
}

void
ApocalypseWidget::setMaxRate(double max_rate)
{
  GenDebug("<ApocalypseWidget::setMaxRate> entered.\n");

  m_max_rate = max_rate;
  m_scaling_factor = m_rate_slider->maxValue() / max_rate;
}

void
ApocalypseWidget::setRate(double rate)
{
  GenDebug("<ApocalypseWidget::setRate> entered.\n");

  m_last_rate = rate;
  m_rate_slider->setValue(
    (int)(m_rate_slider->maxValue() - (m_scaling_factor * rate))
  );
}

void
ApocalypseWidget::setLineStep(double frac_step)
{
  GenDebug("<ApocalypseWidget::setLineStep> entered.\n");

  m_rate_slider->setLineStep((int)(m_scaling_factor * frac_step));
}

void
ApocalypseWidget::setPageStep(double frac_step)
{
  GenDebug("<ApocalypseWidget::setPageStep> entered.\n");

  m_rate_slider->setPageStep((int)(m_scaling_factor * frac_step));
}

void
ApocalypseWidget::setRateValidator(avd_RateValidator2 *validator)
{
  GenDebug("<ApocalypseWidget::setRateValidator> entered.\n");

  m_rate_line_edit->setValidator(validator);
}

double
ApocalypseWidget::maxRate(void)
{
  GenDebug("<ApocalypseWidget::maxRate> entered.\n");

  return m_max_rate;
}

double
ApocalypseWidget::rate(void)
{
  GenDebug("<ApocalypseWidget::rate> entered.\n");

  return m_last_rate;
}

/* side effect:  changes member variable m_last_rate.  */
void
ApocalypseWidget::valueChangedSlot(int value){
  m_last_rate = (m_rate_slider->maxValue() - value) / m_scaling_factor;

  m_rate_line_edit->setText(QString("%1").arg(m_last_rate));
}

void
ApocalypseWidget::returnPressedSlot(void){
  GenDebug("<ApocalypseWidget::returnPressedSlot> entered.\n");

  bool sigs_were_blocked = m_rate_slider->signalsBlocked();

  m_rate_slider->blockSignals(true);
  /* next line changes member variable m_last_rate.  */
  setRate(m_rate_line_edit->text().toDouble());
  m_rate_slider->blockSignals(sigs_were_blocked);
}
