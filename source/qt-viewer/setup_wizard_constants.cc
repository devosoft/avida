//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_CONSTANTS_HH
#include "setup_wizard_constants.hh"
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#ifndef GENESIS_HH
#include "genesis.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif


using namespace std;


using namespace std;


/*
  cSetupDescription
*/
cSetupDescription::cSetupDescription(
  QString file_key,
  QString file_comment,
  QObject *parent,
  const char *name
)
: QObject(parent, name),
  m_file_key(file_key),
  m_file_comment(file_comment)
{}

QString
cSetupDescription::file_comment()
{
  return "# <cSetupDescription::file_comment> not implemented";
}

QString
cSetupDescription::file_key()
{
  return "# <cSetupDescription::file_key> not implemented";
}

QString
cSetupDescription::key()
{
  return m_file_key;
}

void
cSetupDescription::writeFileEntry(QTextStream &t)
{
  t << file_comment();
  t << file_key() << endl;
}

QString
cSetupDescription::defaultQStringValue(bool &ok){
  ok = false;
  Message("<cSetupDescription::defaultQStringValue> not implemented: ")
  (m_file_key);
  return "<cSetupDescription::defaultQStringValue> not implemented:" +
  m_file_key;
}

int
cSetupDescription::defaultIntValue(bool &ok){
  ok = false;
  Message("<cSetupDescription::defaultIntValue> not implemented: ")
  (m_file_key);
  return 0;
}

double
cSetupDescription::defaultDblValue(bool &ok){
  ok = false;
  Message("<cSetupDescription::defaultDblValue> not implemented: ")
  (m_file_key);
  return 0.0;
}

QString
cSetupDescription::qStringValue(bool &ok){
  ok = false;
  Message("<cSetupDescription::qStringValue> not implemented: ")
  (m_file_key);
  return "<cSetupDescription::qStringValue> not implemented";
}

int
cSetupDescription::intValue(bool &ok){
  ok = false;
  Message("<cSetupDescription::intValue> not implemented: ")
  (m_file_key);
  return 0;
}

double
cSetupDescription::dblValue(bool &ok){
  ok = false;
  Message("<cSetupDescription::dblValue> not implemented: ")
  (m_file_key);
  return 0.0;
}

int 
cSetupDescription::set(QString value){
  Message("<cSetupDescription::set(QString)> not implemented: ")
  (m_file_key);
  return 1;
}

int 
cSetupDescription::set(int value){
  Message("<cSetupDescription::set(int)> not implemented: ")
  (m_file_key);
  return 1;
}

int 
cSetupDescription::set(double value){
  Message("<cSetupDescription::set(double)> not implemented: ")
  (m_file_key);
  return 1;
}

int 
cSetupDescription::check(QString value){
  Message("<cSetupDescription::check(QString)> not implemented: ")
  (m_file_key);
  return 1;
}

int 
cSetupDescription::check(int value){
  Message("<cSetupDescription::check(int)> not implemented: ")
  (m_file_key);
  return 1;
}

int 
cSetupDescription::check(double value){
  Message("<cSetupDescription::check(double)> not implemented: ")
  (m_file_key);
  return 1;
}

int 
cSetupDescription::load(cGenesis *){
  Message("<cSetupDescription::load> not implemented: ")
  (m_file_key);
  return 1;
}

int 
cSetupDescription::save(cGenesis *){
  Message("<cSetupDescription::save> not implemented: ")
  (m_file_key);
  return 1;
}

/*
  cSetupQString
*/
cSetupQString::cSetupQString(
  QString file_key,
  QString file_comment,
  QString default_value,
  QObject *parent,
  const char *name
)
: cSetupDescription(
    file_key,
    file_comment,
    parent,
    name
  ),
  m_default(default_value),
  m_value(default_value)
{}

QString
cSetupQString::file_comment()
{
  return m_file_comment;
}

QString
cSetupQString::file_key()
{
  return m_file_key + ' ' + m_value;
}

QString
cSetupQString::defaultQStringValue(bool &ok)
{
  ok = true;
  return m_default;
}

int
cSetupQString::set(QString value)
{
  m_value = value;
  return 0;
}

QString
cSetupQString::qStringValue(bool &ok)
{
  ok = true;
  return m_value;
}

int 
cSetupQString::load(cGenesis *genesis){
  cString tmpstring = genesis->ReadString(
    m_file_key.latin1(),
    m_default.latin1()
  );
  m_value = QString(tmpstring());
  return 0;
}

int 
cSetupQString::save(cGenesis *genesis){
  cString in_name = m_file_key.latin1();
  cString in_value = m_value.latin1();
  genesis->AddInput(in_name, in_value);
  return 0;
}


/*
  cSetupHeader
*/
cSetupHeader::cSetupHeader(
  QString header,
  QObject *parent,
  const char *name
)
: cSetupQString(
    "",
    header,
    "",
    parent,
    name
  )
{}

void
cSetupHeader::writeFileEntry(QTextStream &t)
{
  t << file_comment();
}


/*
  cSetupIntRange
*/
cSetupIntRange::cSetupIntRange(
  QString file_key,
  QString file_comment,
  int min,
  int max,
  int default_value,
  bool show_range,
  QObject *parent,
  const char *name
)
: cSetupDescription(
    file_key,
    file_comment,
    parent,
    name
  ),
  m_min(min),
  m_max(max),
  m_default(default_value),
  m_value(default_value),
  m_show_range(show_range)
{}

QString
cSetupIntRange::file_comment()
{
  return m_file_comment;
}

QString
cSetupIntRange::file_key()
{
  return (m_file_key + " %1").arg(m_value);
}

void
cSetupIntRange::writeFileEntry(QTextStream &t)
{
  t << file_comment();
  t << file_key() << endl;
  if(!m_show_range) return;

  t << QString("  # int.; minimum: %1, ").arg(m_min);
  if(INT_MAX == m_max){
    t << "maximum: INT_MAX (machine limit)" << endl;
  } else {
    t << QString("maximum: %1").arg(m_max) << endl;
  }
}

int
cSetupIntRange::min()
{
  return m_min;
}

int
cSetupIntRange::max()
{
  return m_max;
}

int
cSetupIntRange::defaultIntValue(bool &ok)
{
  ok = true;
  return m_default;
}

int
cSetupIntRange::set(int value)
{
  m_value = value;
  return 0;
}

int
cSetupIntRange::intValue(bool &ok)
{ 
  ok = true;
  return m_value;
}

int 
cSetupIntRange::load(cGenesis *genesis){
  m_value = genesis->ReadInt(
    m_file_key.latin1(),
    m_default,
    true
  );
  return 0;
}

int 
cSetupIntRange::save(cGenesis *genesis){
  cString in_name = m_file_key.latin1();
  cString in_value = QString("%1").arg(m_value).latin1();
  cout << "<cSetupIntRange::save> " << in_name << ":" << in_value << endl;
  genesis->AddInput(in_name, in_value);
  return 0;
}


/*
  cSetupIntChoice
*/
cSetupIntChoice::cSetupIntChoice(
  QString file_key,
  QString file_comment,
  int default_value,
  bool show_range,
  QObject *parent,
  const char *name
)
: cSetupDescription(
    file_key,
    file_comment,
    parent,
    name
 ),
  m_default(default_value),
  m_value(default_value),
  m_show_range(show_range)
{}

QString
cSetupIntChoice::file_comment()
{
  return m_file_comment;
}

QString
cSetupIntChoice::file_key()
{
  return (m_file_key + " %1").arg(m_value);
}

void
cSetupIntChoice::writeFileEntry(QTextStream &t)
{
  t << file_comment();
  t << file_key() << endl;
  if(!m_show_range) return;

  tStringMap::Iterator it;
  for ( it = m_choice_map.begin(); it != m_choice_map.end(); ++it ) {
    t << "  # " << it.key() << " = " << it.data() << endl;
  }
}

int
cSetupIntChoice::set(int value)
{
  m_value = value;
  return 0;
}

int
cSetupIntChoice::intValue(bool &ok)
{
  ok = true;
  return m_value;
}

int
cSetupIntChoice::defaultIntValue(bool &ok)
{
  ok = true;
  return m_default;
}

QString
cSetupIntChoice::default_filestring()
{
  return m_choice_map[m_default];
}

QString
cSetupIntChoice::filestring()
{
  return m_choice_map[m_value];
}

tStringMapIterator
cSetupIntChoice::insertMapping(
  const int &key,
  const QString &value
){
  return m_choice_map.insert(
    key,
    value
  );
}

int 
cSetupIntChoice::load(cGenesis *genesis){
  m_value = genesis->ReadInt(
    m_file_key.latin1(),
    m_default,
    true
  );
  return 0;
}

int 
cSetupIntChoice::save(cGenesis *genesis){
  cString in_name = m_file_key.latin1();
  cString in_value = QString("%1").arg(m_value).latin1();
  genesis->AddInput(in_name, in_value);
  return 0;
}



/*
  cSetupDoubleRange
*/
cSetupDoubleRange::cSetupDoubleRange(
  QString file_key,
  QString file_comment,
  double min,
  double max,
  double default_value,
  bool show_range,
  QObject *parent,
  const char *name
)
: cSetupDescription(
    file_key,
    file_comment,
    parent,
    name
  ),
  m_min(min),
  m_max(max),
  m_default(default_value),
  m_value(default_value),
  m_show_range(show_range)
{}

QString
cSetupDoubleRange::file_comment()
{
  return m_file_comment;
}

QString
cSetupDoubleRange::file_key()
{
  return (m_file_key + " %1").arg(m_value, 0, 'f', 6);
}

void
cSetupDoubleRange::writeFileEntry(QTextStream &t)
{
  t << file_comment();
  t << file_key() << endl;
  if(!m_show_range) return;

  t << QString("  # dbl. prec. float; minimum: %1, ").arg(m_min);
  if(DBL_MAX == m_max){
    t << "maximum: DBL_MAX (machine limit)" << endl;
  } else {
    t << QString("maximum: %1").arg(m_max) << endl;
  }
}

double
cSetupDoubleRange::min()
{
  return m_min;
}

double
cSetupDoubleRange::max()
{
  return m_max;
}

double
cSetupDoubleRange::defaultDblValue(bool &ok)
{
  ok = true;
  return m_default;
}

int
cSetupDoubleRange::set(double value)
{
  m_value = value;
  return 0;
}

double
cSetupDoubleRange::dblValue(bool &ok)
{
  ok = true;
  return m_value;
}

int 
cSetupDoubleRange::load(cGenesis *genesis){
  m_value = genesis->ReadFloat(
    m_file_key.latin1(),
    m_default,
    true
  );
  return 0;
}

int 
cSetupDoubleRange::save(cGenesis *genesis){
  cString in_name = m_file_key.latin1();
  cString in_value = QString("%1").arg(m_value).latin1();
  genesis->AddInput(in_name, in_value);
  return 0;
}

