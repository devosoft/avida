//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_CONSTANTS_HH
#define SETUP_WIZARD_CONSTANTS_HH

#include <qobject.h>
#include <qmap.h>
#include <qtextstream.h>

#include <float.h>

class cGenesis;

class cSetupDescription : public QObject {
protected:
  const QString m_file_comment;
  const QString m_file_key;
public:
  cSetupDescription(
    QString file_key,
    QString file_comment,
    QObject *parent = 0,
    const char *name = 0
  );
  virtual QString file_comment();
  virtual QString file_key();
  virtual QString key();
  virtual void writeFileEntry(QTextStream &t);

  virtual QString defaultQStringValue(bool &ok);
  virtual int defaultIntValue(bool &ok);
  virtual double defaultDblValue(bool &ok);

  virtual QString qStringValue(bool &ok);
  virtual int intValue(bool &ok);
  virtual double dblValue(bool &ok);

  virtual int set(QString value);
  virtual int set(int value);
  virtual int set(double value);

  virtual int check(QString value);
  virtual int check(int value);
  virtual int check(double value);

  virtual int load(cGenesis *);
  virtual int save(cGenesis *);
};


class cSetupQString : public cSetupDescription {
protected:
  const QString m_default;
  QString m_value;
public:
  cSetupQString(
    QString file_key,
    QString file_comment,
    QString default_value,
    QObject *parent = 0,
    const char *name = 0
  );
  virtual QString file_comment();
  virtual QString file_key();
  virtual QString defaultQStringValue(bool &ok);
  virtual QString qStringValue(bool &ok);
  virtual int set(QString value);
  virtual int load(cGenesis *);
  virtual int save(cGenesis *);
};


class cSetupHeader : public cSetupQString {
public:
  cSetupHeader(
    QString header,
    QObject *parent = 0,
    const char *name = 0
  );
  virtual void writeFileEntry(QTextStream &t);
};


class cSetupIntRange : public cSetupDescription {
protected:
  const int m_min;
  const int m_max;
  const int m_default;
  int m_value;
  bool m_show_range;
public:
  cSetupIntRange(
    QString file_key,
    QString file_comment,
    int min,
    int max,
    int default_value,
    bool show_range,
    QObject *parent = 0,
    const char *name = 0
  );
  virtual QString file_comment();
  virtual QString file_key();
  virtual void writeFileEntry(QTextStream &t);
  int min();
  int max();
  virtual int defaultIntValue(bool &ok);
  virtual int intValue(bool &ok);
  virtual int set(int value);
  virtual int load(cGenesis *);
  virtual int save(cGenesis *);
};


typedef QMap<int, QString> tStringMap;
typedef QMapIterator<int, QString> tStringMapIterator;

class cSetupIntChoice : public cSetupDescription {
protected:
  tStringMap m_choice_map;
  int m_default;
  int m_value;
  bool m_show_range;
public:
  cSetupIntChoice(
    QString file_key,
    QString file_comment,
    int default_value,
    bool show_range,
    QObject *parent = 0,
    const char *name = 0
  );
  virtual QString file_comment();
  virtual QString file_key();
  virtual void writeFileEntry(QTextStream &t);
  virtual int defaultIntValue(bool &ok);
  virtual int intValue(bool &ok);
  virtual int set(int value);
  QString default_filestring();
  QString filestring();
  tStringMapIterator insertMapping(
    const int &key,
    const QString &value
  );
  virtual int load(cGenesis *);
  virtual int save(cGenesis *);
};


class cSetupDoubleRange : public cSetupDescription {
protected:
  const double m_min;
  const double m_max;
  const double m_default;
  double m_value;
  bool m_show_range;
public:
  cSetupDoubleRange(
    QString file_key,
    QString file_comment,
    double min,
    double max,
    double default_value,
    bool show_range,
    QObject *parent = 0,
    const char *name = 0
  );
  virtual QString file_comment();
  virtual QString file_key();
  virtual void writeFileEntry(QTextStream &t);
  double min();
  double max();
  virtual double defaultDblValue(bool &ok);
  virtual double dblValue(bool &ok);
  virtual int set(double value);
  virtual int load(cGenesis *);
  virtual int save(cGenesis *);
};


#endif /* !SETUP_WIZARD_CONSTANTS_HH */
