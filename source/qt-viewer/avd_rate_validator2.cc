//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <float.h>

#include <qmessagebox.h>
#include <qstring.h>

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#include "avd_rate_validator2.hh"


using namespace std;


avd_RateValidator2::avd_RateValidator2(
  QObject *parent,
  const char *name
)
: QDoubleValidator(parent, name)
{
  
  GenDebug("<avd_RateValidator2> constructor called.\n");

  setRange(
    0.0,
    1.0,
    DBL_MAX_10_EXP
  );
}

avd_RateValidator2::~avd_RateValidator2(void)
{
  GenDebug("<~avd_RateValidator2> destructor called.\n");
}

void
avd_RateValidator2::fixup(QString &input) const
{
  GenDebug("<avd_RateValidator2::fixup> called.\n");

  bool ok;
  double value = input.toDouble(&ok);
  if(!ok){
    QMessageBox::information(
      0,
      "Bad rate",
      "\""
      + input
      + "\" isn't a number."
    );
  }else if(0.0 > value || value > 1.0) {
    QMessageBox::information(
      0,
      "Bad rate",
      "The number \""
      + input
      + "\" is out of range.\n"
      + "Usable rates are at least zero and at most one."
    );
  } else {
    QMessageBox::information(
      0,
      "Bad rate",
      QString(
        "I only handle %1 decimal places after the decimal point\n"
        "(because numbers smaller in absolute value than %2\n"
        "can't be accurately represented on this computer)."
      ).arg(DBL_MAX_10_EXP).arg(DBL_MIN)
    );
  }
}

