//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
// 
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_FILE_HH
#define SETUP_FILE_HH

#include <qobject.h>

class avd_SetupFile : public QObject
{
private:
  QString m_file_name;
public:
  avd_SetupFile(
    QString file_name,
    QObject *parent = 0,
    const char *name = 0
  );
};

#endif /* !SETUP_FILE_HH */
