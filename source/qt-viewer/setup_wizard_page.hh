//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#ifndef SETUP_WIZARD_PAGE_HH
#define SETUP_WIZARD_PAGE_HH

#include <qhbox.h>


class QComboBox;
class QFileInfo;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QVBox;
class QWizard;


class AvidaSetupPage : public QHBox
{
Q_OBJECT
protected:
  const QString &m_help, &m_file_comment;
  QVBox *m_page;
public:
  AvidaSetupPage(void);
  AvidaSetupPage(
    QWizard *parent,
    const QString &directions = "",
    const QString &help = "",
    const QString &file_comment = ""
  );
public:
  const QString &help(void){ return m_help; }
  const QString &fileComment(void){ return m_file_comment; }
  virtual QString parameters(void);
signals:
  void setBackEnabledSig(QWidget *self, bool valid);
  void setNextEnabledSig(QWidget *self, bool valid);
  void setFinishEnabledSig(QWidget *self, bool valid);
  void setHelpEnabledSig(QWidget *self, bool valid);
};


class GenesisPage : public AvidaSetupPage
{
Q_OBJECT
protected:
  QRadioButton *m_use_existing_genesis_button;
  QRadioButton *m_create_new_genesis_button;
  QLabel *m_genesis_field_label;
  QLabel *m_genesis_filename_label;
  QPushButton *m_genesis_filename_button;
  QLabel *m_work_directory_label;
  QPushButton *m_work_directory_button;

  QString m_existing_genesis_filename;
  QString m_new_genesis_filename;
  QString m_working_dirname;

  QString m_environment_filename;
  QString m_instruction_set_filename;
  QString m_start_creature_filename;
  QString m_events_filename;
public:
  GenesisPage(
    QWizard *parent,
    const QString &directions,
    const QString &help
  );

  const QString &existingGenesisFilename(void) const {
    return m_existing_genesis_filename;
  }
  const QString &newGenesisFilename(void) const {
    return m_new_genesis_filename;
  }
  const QString &workingDirname(void) const {
    return m_working_dirname;
  }

  const QString &environmentFilename(void) const {
    return m_environment_filename;
  }
  const QString &instructionSetFilename(void) const {
    return m_instruction_set_filename;
  }
  const QString &startCreatureFilename(void) const {
    return m_start_creature_filename;
  }
  const QString &eventsFilename(void) const {
    return m_events_filename;
  }
protected:
  bool isValid(void);
  bool isExistingGenesisValid(const QString &filepath, QString &message);
  bool isNewGenesisValid(const QString &filepath, QString &message);
  bool isCurrentDirValid(const QString &dirpath, QString &message);

  QString absPath(const QString &filepath, const QString &genesis_path);
  void generateFilepathsFromExistingGenesis(const QString &genesis_path);
protected slots:
  void useExistingGenesisButtonSlot(void);
  void createNewGenesisButtonSlot(void);
  void genesisFilenameButtonSlot(void);
  void workDirectoryButtonSlot(void);
signals:
  void usingExistingGenesisSig(bool);
};


class WorldDimensionsPage : public AvidaSetupPage
{
Q_OBJECT
public:
  QLineEdit *m_width_field;
  QLineEdit *m_height_field;
  QString m_width_param_name;
  QString m_height_param_name;
public:
  WorldDimensionsPage(
    QWizard *parent,
    const QString &directions,
    const QString &help,
    const QString &file_comment,
    const QString &width_param_name,
    unsigned int width_min, unsigned int width_max,
    unsigned int default_width,
    const QString &height_param_name,
    unsigned int height_min, unsigned int height_max,
    unsigned int default_height
  );
  virtual QString parameters(void);
};


class TopologyChoicePage : public AvidaSetupPage
{
Q_OBJECT
protected:
  QComboBox *m_combobox;
  QString m_topology_param_name;
public:
  TopologyChoicePage(
    QWizard *parent,
    const QString &directions,
    const QString &help,
    const QString &file_comment,
    const QString &topology_param_name
  );
  virtual QString parameters(void);
};


class PopulateChoicePage : public AvidaSetupPage
{
Q_OBJECT
protected:
  QComboBox *m_combobox;
public:
  PopulateChoicePage(
    QWizard *parent,
    const QString &directions,
    const QString &help,
    const QString &file_comment
  );
  virtual QString parameters(void);
};


class EndConditionPage : public AvidaSetupPage
{
Q_OBJECT
protected:
  QComboBox *m_combobox;
  QLineEdit *m_endtime_field;
public:
  EndConditionPage(
    QWizard *parent,
    const QString &directions,
    const QString &help,
    const QString &file_comment
  );
  virtual QString parameters(void);
};


#endif /* !SETUP_WIZARD_PAGE_HH */
