//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////


#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qstring.h>


#include "repopulate_widget.hh"


RepopulateWidget::RepopulateWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  //cout << "<RepopulateWidget> constructor.\n";

  setupLayout();
  setupConnects();
}

RepopulateWidget::~RepopulateWidget(void)
{
  //cout << "<~RepopulateWidget> destructor.\n";
}

void
RepopulateWidget::setupLayout(void)
{
  //cout << "<RepopulateWidget::setupLayout> entered.\n";

  m_vbox_layout = new QVBoxLayout(
    this,
    0,
    6,
    "<RepopulateWidget::m_vbox_layout>"
  );

  m_title_label = new QLabel(this);
  m_vbox_layout->addWidget(m_title_label);

  m_combo_box = new QComboBox(FALSE, this);
  m_vbox_layout->addWidget(m_combo_box);

  m_spacer = new QSpacerItem(
    20,
    20,
    QSizePolicy::Minimum,
    QSizePolicy::Expanding
  );
  m_vbox_layout->addItem(m_spacer);

  m_inject_pushbutton = new QPushButton(this);
  m_vbox_layout->addWidget(m_inject_pushbutton);

  m_inject_all_pushbutton = new QPushButton(this);
  m_vbox_layout->addWidget(m_inject_all_pushbutton);
}

void
RepopulateWidget::setupConnects(void)
{
  //cout << "<RepopulateWidget::setupConnects> entered.\n";

  connect(
    m_inject_pushbutton, SIGNAL(clicked(void)),
    this, SIGNAL(injectOneButtonClicked(void))
  );
  connect(
    m_inject_all_pushbutton, SIGNAL(clicked(void)),
    this, SIGNAL(injectAllButtonClicked(void))
  );
}

void
RepopulateWidget::setLabel(const QString &text)
{
  //cout << "<RepopulateWidget::setLabel> entered.\n";

  m_title_label->setText(text);
}

void
RepopulateWidget::setInjectButtonText(const QString &text)
{
  //cout << "<RepopulateWidget::setInjectButtonText> entered.\n";

  m_inject_pushbutton->setText(text);
}

void
RepopulateWidget::setInjectAllButtonText(const QString &text)
{
  //cout << "<RepopulateWidget::setInjectAllButtonText> entered.\n";

  m_inject_all_pushbutton->setText(text);
}

QString
RepopulateWidget::currentText(void) const
{
  //cout << "<RepopulateWidget::currentText> entered.\n";

  return m_combo_box->currentText();
}

void
RepopulateWidget::clear(void)
{
  //cout << "<RepopulateWidget::clear> entered.\n";

  m_combo_box->clear();
}

void
RepopulateWidget::insertItem(const QString &t, int index)
{
  //cout << "<RepopulateWidget::insertItem> entered.\n";

  m_combo_box->insertItem(t, index);
}
