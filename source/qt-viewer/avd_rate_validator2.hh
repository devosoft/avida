//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef AVD_RATE_VALIDATOR2_HH
#define AVD_RATE_VALIDATOR2_HH


#include <qvalidator.h>


class avd_RateValidator2 : public QDoubleValidator
{
public:
  avd_RateValidator2(QObject *parent, const char *name = 0);
  ~avd_RateValidator2(void);
  void fixup(QString &input) const;
};


#endif /* !AVD_RATE_VALIDATOR2_HH */
