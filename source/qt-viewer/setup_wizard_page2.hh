//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_PAGE2_HH
#define SETUP_WIZARD_PAGE2_HH

#include <qhbox.h>
#include <qvalidator.h>


class QVBox;
class QLineEdit;
class QPushButton;
class GenesisWrapper;


class AvidaSetupPage2 : public QHBox
{
  Q_OBJECT
protected:
  const QString m_help;
  QVBox *m_page;
public:
  AvidaSetupPage2(
    const QString &directions = "",
    const QString &help = "",
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
public:
  const QString &help(void){ return m_help; }
};


class SetupStylePage
: public AvidaSetupPage2
{
  Q_OBJECT
public:
  SetupStylePage(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
signals:
  void existingSetupSig();
  void simpleSetupSig();
  void detailedSetupSig();
};


class avd_FileValidator
: public QValidator
{
public:
  avd_FileValidator(QObject *parent, const char *name = 0)
  : QValidator(parent, name)
  {}
  QValidator::State validate(QString &input, int &pos) const;
  void fixup(QString &input) const;
};


class ExistingGenesisPage
: public AvidaSetupPage2
{
  Q_OBJECT
protected:
  QLineEdit *m_genesis_filename_lineedit;
  QPushButton *m_genesis_filename_button;

  GenesisWrapper *m_genwrap;

  QString m_existing_genesis_filename;
  QString m_environment_filename;
  QString m_instruction_set_filename;
  QString m_start_creature_filename;
  QString m_events_filename;
public:
  ExistingGenesisPage(
    GenesisWrapper *genwrap,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
protected:
  bool isValid(void);
  bool isExistingGenesisValid(const QString &filepath, QString &message);
  QString absPath(const QString &filepath, const QString &genesis_path);
  int generateFilepathsFromExistingGenesis(const QString &genesis_path);
protected slots:
  void genesisFilenameButtonSlot(void);
  void genesisFilenameLineEditSlot(void);
  void genesisFilenameLineEditChanged(void);
signals:
  void isValidSig(bool);
};


class WorkingDirectoryPage
: public AvidaSetupPage2
{
  Q_OBJECT
protected:
  QLineEdit *m_working_dirname_lineedit;
  QPushButton *m_working_dirname_button;

  GenesisWrapper *m_genwrap;

  QString m_working_dirname;
public:
  WorkingDirectoryPage(
    GenesisWrapper *genwrap,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
protected:
  bool isValid(void);
  bool isWorkingDirValid(const QString &dirpath, QString &message);
  //QString absPath(const QString &filepath, const QString &genesis_path);
//protected slots:
//  void workingDirnameButtonSlot(void);
//  void workingDirnameLineEditSlot(void);
//  void workingDirnameLineEditChanged(void);
signals:
  void isValidSig(bool);
};


class NewGenesisPage
: public AvidaSetupPage2
{
  Q_OBJECT
protected:
  GenesisWrapper *m_genwrap;
public:
  NewGenesisPage(
    GenesisWrapper *genwrap,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
};


class FinalPage
: public AvidaSetupPage2
{
  Q_OBJECT
protected:
  GenesisWrapper *m_genwrap;
public:
  FinalPage(
    GenesisWrapper *genwrap = 0,
    const QString &directions = "",
    const QString &help = "",
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  ):AvidaSetupPage2(directions, help, parent, name, f),
    m_genwrap(genwrap)
  {}
  GenesisWrapper *genWrap(void){ return m_genwrap; }
};


class ExistingFinalPage
: public FinalPage
{
  Q_OBJECT
public:
  ExistingFinalPage(
    GenesisWrapper *genwrap = 0,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
};


class SimpleFinalPage
: public FinalPage
{
  Q_OBJECT
public:
  SimpleFinalPage(
    GenesisWrapper *genwrap = 0,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
};


class DetailedFinalPage
: public FinalPage
{
  Q_OBJECT
public:
  DetailedFinalPage(
    GenesisWrapper *genwrap = 0,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0 
  );
};


#endif /* !SETUP_WIZARD_PAGE2_HH */
