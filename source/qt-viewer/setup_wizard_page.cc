//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology
//
// Read the COPYING and README files, or contact 'avida@alife.org',
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <qcombobox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qhbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qvalidator.h>
#include <qvbox.h>
#include <qwizard.h>

#ifndef GENESIS_HH
#include "genesis.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif

#ifndef SETUP_WIZARD_PAGE_HH
#include "setup_wizard_page.hh"
#endif


using namespace std;


AvidaSetupPage::AvidaSetupPage(
  QWizard *parent,
  const QString &directions,
  const QString &help,
  const QString &file_comment
):QHBox(parent),
  m_help(help),
  m_file_comment(file_comment)
{
  /* FIXME: make spacing be configurable parameters.  -- kaben.  */
  setSpacing(6);

  QLabel *directions_label = new QLabel(this);

  /* FIXME: make palette a configurable parameter.  -- kaben.  */
  directions_label->setPalette(white);
  directions_label->setText(directions);

  /* FIXME: make indent a configurable parameter.  -- kaben.  */
  directions_label->setIndent(6);
  directions_label->setMaximumWidth(directions_label->sizeHint().width());

  /*
  By placing QVBox *m_page into a private layout widget, we can surround
  it by spacers to compress its contents, centered vertically.
  */
  QWidget *layout_widget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  layout->addItem(
    new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum)
  );
  
  m_page = new QVBox(layout_widget);
  layout->addWidget(m_page);

  layout->addItem(
    new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum)
  );

  connect(
    this, SIGNAL(setBackEnabledSig(QWidget *, bool)),
    parent, SLOT(setBackEnabled(QWidget *, bool))
  );
  connect(
    this, SIGNAL(setNextEnabledSig(QWidget *, bool)),
    parent, SLOT(setNextEnabled(QWidget *, bool))
  );
  connect(
    this, SIGNAL(setFinishEnabledSig(QWidget *, bool)),
    parent, SLOT(setFinishEnabled(QWidget *, bool))
  );
  connect(
    this, SIGNAL(setHelpEnabledSig(QWidget *, bool)),
    parent, SLOT(setHelpEnabled(QWidget *, bool))
  );
}

QString
AvidaSetupPage::parameters(void)
{
  GenError("<AvidaSetupPage::parameters> not implemented.");
  return QString("### <AvidaSetupPage::parameters> not implemented.");
}


GenesisPage::GenesisPage(
  QWizard *parent,
  const QString &directions,
  const QString &help
)
: AvidaSetupPage(parent, directions, help)
{
  Message("<GenesisPage> constructor.");

  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  QHButtonGroup *button_group = new QHButtonGroup(layout_widget);
  layout->addWidget(button_group);

  layout->addItem(new QSpacerItem(20, 20));

  m_use_existing_genesis_button
    = new QRadioButton("Use an existing genesis file", button_group);
  connect(
    m_use_existing_genesis_button, SIGNAL(clicked(void)),
    this, SLOT(useExistingGenesisButtonSlot(void))
  );

  m_create_new_genesis_button
    = new QRadioButton("Create a new genesis file", button_group);
  connect(
    m_create_new_genesis_button, SIGNAL(clicked(void)),
    this, SLOT(createNewGenesisButtonSlot(void))
  );

  m_genesis_field_label = new QLabel(layout_widget);
  layout->addWidget(m_genesis_field_label);

  m_genesis_filename_label = new QLabel(layout_widget);
  layout->addWidget(m_genesis_filename_label);
  /* FIXME: make palette a configurable parameter.  -- kaben.  */
  m_genesis_filename_label->setPalette(white);
  /* FIXME: make indent a configurable parameter.  -- kaben.  */
  m_genesis_filename_label->setIndent(6);

  QHBoxLayout *hlayout1 = new QHBoxLayout(layout);
  hlayout1->addItem(new QSpacerItem(20, 20));
  
  m_genesis_filename_button = new QPushButton(
    "Set Genesis File",
    layout_widget
  );
  connect(
    m_genesis_filename_button, SIGNAL(clicked(void)),
    this, SLOT(genesisFilenameButtonSlot(void))
  );
  hlayout1->addWidget(m_genesis_filename_button);
  hlayout1->addItem(new QSpacerItem(20, 20));

  //layout->addWidget(m_genesis_filename_button);

  layout->addItem(new QSpacerItem(20, 20));

  layout->addWidget(new QLabel("Path to work directory:", layout_widget));

  m_work_directory_label = new QLabel(layout_widget);
  layout->addWidget(m_work_directory_label);
  /* FIXME: make palette a configurable parameter.  -- kaben.  */
  m_work_directory_label->setPalette(white);
  /* FIXME: make indent a configurable parameter.  -- kaben.  */
  m_work_directory_label->setIndent(6);

  QHBoxLayout *hlayout2 = new QHBoxLayout(layout);
  hlayout2->addItem(new QSpacerItem(20, 20));

  m_work_directory_button = new QPushButton(
    "Set Work Directory",
    layout_widget
  );
  connect(
    m_work_directory_button, SIGNAL(clicked(void)),
    this, SLOT(workDirectoryButtonSlot(void))
  );
  hlayout2->addWidget(m_work_directory_button);
  hlayout2->addItem(new QSpacerItem(20, 20));

  //layout->addWidget(m_work_directory_button);

  /*
  FIXME: move me elsewhere...
  FIXME: this is really ugly...
  -- kaben.
  */
  QString ignore;
  if(isExistingGenesisValid(QFileInfo("genesis").absFilePath(), ignore))
  {
    Message("<GenesisPage> found genesis file in cwd.");
    m_existing_genesis_filename = QFileInfo("genesis").absFilePath();
  } else {
    char *env = getenv("AVIDA_GENESIS_FILE");
    if(env && isExistingGenesisValid(QString(env), ignore))
    {
      if(
        isExistingGenesisValid(
          QFileInfo(
            QString(env)
          ).absFilePath(),
          ignore
        )
      ){
        Message("<GenesisPage> found genesis file in $AVIDA_GENESIS_FILE.");
        m_existing_genesis_filename = QFileInfo(QString(env)).absFilePath();
      } else {
        Message("<GenesisPage> didn't find genesis file.");
        free(env);
      }
    } else Message("<GenesisPage> didn't find genesis file.");
  }
  //m_use_existing_genesis_button->setChecked(true);
  //useExistingGenesisButtonSlot();

  if(isCurrentDirValid(QDir::currentDirPath(), ignore))
  {
    Message("<GenesisPage> current directory is valid.");
    m_working_dirname = QDir::currentDirPath();
    m_work_directory_label->setText(m_working_dirname);
  } else {
    Message("<GenesisPage> current directory \"")
    (QDir::currentDirPath())
    ("\" is invalid.");
  }

  isValid();
}

bool
GenesisPage::isValid(void)
{
  Message("<GenesisPage::isValid> entered.");

  QString ignore;
  if(isCurrentDirValid(m_work_directory_label->text(), ignore))
  {
    if(
      m_use_existing_genesis_button->isChecked()
      &&
      isExistingGenesisValid(m_genesis_filename_label->text(), ignore)
    ){
      Message(" --- enabling next and finish, returning true.");
      emit setNextEnabledSig(this, false);
      emit setFinishEnabledSig(this, true);
      return true;
    } else if(
      isNewGenesisValid(m_genesis_filename_label->text(), ignore)
    ){
      Message(" --- enabling next, disabling finish, returning true.");
      emit setNextEnabledSig(this, true);
      emit setFinishEnabledSig(this, false);
      return true;
    }
  }
  Message(" --- disabling next and finish, returning false.");
  emit setNextEnabledSig(this, false);
  emit setFinishEnabledSig(this, false);
  return false;
}

bool
GenesisPage::isExistingGenesisValid(
  const QString &filepath,
  QString &message
){
  Message("<GenesisPage::isExistingGenesisValid> entered.");

  /*
  FIXME: need more validation.
  be sure that environment file, instruction set file, event file, and
  start creature as specified in genesis file are readable and valid.
  -- kaben.
  */
  QFileInfo genesis_fileinfo(filepath);
  if(filepath == QString::null){
    message = "genesis file name was null.";
    return false;
  }
  if(!genesis_fileinfo.exists()){
    message = "the file \"" + filepath + "\" does not exist.";
    return false;
  }
  if(!genesis_fileinfo.isReadable()){
    message = "the file \"" + filepath + "\" is not readable.";
    return false;
  }
  generateFilepathsFromExistingGenesis(filepath);
  return true;
}

bool
GenesisPage::isNewGenesisValid(
  const QString &filepath,
  QString &message
){
  Message("<GenesisPage::isNewGenesisValid> entered.");
  Message(" --- testing path \"")
  (QFileInfo(filepath).dirPath(true))
  ("\" to file \"")
  (filepath)
  ("\"...");

  QString dirpath = QFileInfo(filepath).dirPath(true);
  QFileInfo genesis_dirinfo(dirpath);
  if(filepath == QString::null){
    message = "no file name was specified.";
    return false;
  }
  if(!genesis_dirinfo.exists()){
    message = "the directory \"" + dirpath + "\" does not exist.";
    return false;
  }
  if(!genesis_dirinfo.isReadable()){
    message = "the directory \"" + dirpath + "\" is not readable.";
    return false;
  }
  if(!genesis_dirinfo.isWritable()){
    message = "the directory \"" + dirpath + "\" is not writable.";
    return false;
  }
  return true;
}

bool
GenesisPage::isCurrentDirValid(
  const QString &dirpath,
  QString &message
){
  Message("<GenesisPage::isCurrentDirValid> entered.");
  QFileInfo dirpath_fileinfo(dirpath);
  if(dirpath == QString::null){
    message = "no directory was specified.";
    return false;
  }
  if(!dirpath_fileinfo.exists()){
    message = "the directory \"" + dirpath + "\" does not exist.";
    return false;
  }
  if(!dirpath_fileinfo.isReadable()){
    message = "the directory \"" + dirpath + "\" is not readable.";
    return false;
  }
  if(!dirpath_fileinfo.isWritable()){
    message = "the directory \"" + dirpath + "\" is not writable.";
    return false;
  }
  return true;
}

QString
GenesisPage::absPath(
  const QString &filepath,
  const QString &genesis_path
){
  Message("<GenesisPage::absPath> entered.");

  QString absolute_path;
  QFileInfo absolute_pathinfo(filepath);

  if(absolute_pathinfo.isRelative())
  {
    /*
    FIXME:
    this makes dumb presumption of relative path to file.
    -- kaben.
    */
    absolute_pathinfo.setFile(
      QFileInfo(genesis_path).dir(true),
      absolute_pathinfo.fileName()
    );
  }

  Message
  (" --- returning \"")
  (absolute_pathinfo.absFilePath())
  ("\" from filepath \"")
  (filepath)
  ("\" and genesis_path \"")
  (genesis_path)
  ("\".");

  Message(" --- done.");

  return absolute_pathinfo.absFilePath();
}

void
GenesisPage::generateFilepathsFromExistingGenesis(
  const QString &genesis_path
){
  Message("<GenesisPage::generateFilepathsFromExistingGenesis> entered.");

  cGenesis genesis;
  genesis.SetVerbose();
  genesis.Open(genesis_path.latin1());

  m_environment_filename = absPath(
    QString(
      genesis.ReadString("ENVIRONMENT_FILE", "environment.cfg")
    ),
    genesis_path
  );
  m_instruction_set_filename = absPath(
    QString(
      genesis.ReadString("INST_SET", "inst_set")
    ),
    genesis_path
  );
  m_start_creature_filename = absPath(
    QString(
      genesis.ReadString("START_CREATURE")
    ),
    genesis_path
  );
  m_events_filename = absPath(
    QString(
      genesis.ReadString("EVENT_FILE", "events.cfg")
    ),
    genesis_path
  );

  Message("<GenesisPage::generateFilepathsFromExistingGenesis> done.");
}

void
GenesisPage::useExistingGenesisButtonSlot(void)
{
  Message("<GenesisPage::useExistingGenesisButtonSlot> entered.");

  m_genesis_field_label->setText("Path to existing genesis file:");
  m_genesis_filename_label->setText(m_existing_genesis_filename);

  emit usingExistingGenesisSig(true);
  isValid();
}

void
GenesisPage::createNewGenesisButtonSlot(void)
{
  Message("<GenesisPage::createNewGenesisButtonSlot> entered.");

  m_genesis_field_label->setText("Path to new genesis file:");
  m_genesis_filename_label->setText(m_new_genesis_filename);

  emit usingExistingGenesisSig(false);
  isValid();
}

void
GenesisPage::genesisFilenameButtonSlot(void)
{
  Message("<GenesisPage::genesisFilenameButtonSlot> entered.");

  QString file_name;
  QString error_message;

  if(m_create_new_genesis_button->isChecked()){
    /*
    FIXME:
    currently the new genesis file may overwrite an existing genesis
    file without warning.
    -- kaben.
    */
    file_name = m_new_genesis_filename;
    if(file_name.isEmpty())
    {
      file_name = QDir::currentDirPath() + "/genesis";
    }
    file_name = QFileDialog::getSaveFileName(
      file_name,
      QString::null,
      this,
      0,
      "Save new genesis file as:"
    );
    Message(" --- got new genesis filename \"")
    (file_name)
    ("\".");

    if(isNewGenesisValid(file_name, error_message)){
      m_new_genesis_filename = file_name;
      createNewGenesisButtonSlot();
    } else if(file_name != QString::null){
      QMessageBox::information(
        this,
        "New genesis file",
        "Can't create new genesis file:\n"
        +
        error_message
      );
    }
  } else {
    m_use_existing_genesis_button->setChecked(true);
    file_name = m_existing_genesis_filename;
    if(file_name.isEmpty())
    {
      file_name = QDir::currentDirPath() + "/genesis";
    }
    file_name = QFileDialog::getOpenFileName(
      file_name,
      QString::null,
      this,
      0,
      "Locate existing genesis file:"
    );
    Message(" --- got existing genesis filename \"")
    (file_name)
    ("\".");

    if(isExistingGenesisValid(file_name, error_message)){
      m_existing_genesis_filename = file_name;
      /* XXX:  redundant except maybe first time...  -- kaben. */
      useExistingGenesisButtonSlot();
    } else if(file_name != QString::null){
      QMessageBox::information(
        this,
        "Existing genesis file",
        "Can't open existing genesis file:\n"
        +
        error_message
      );
    }
  }
}

void
GenesisPage::workDirectoryButtonSlot(void)
{
  Message("<GenesisPage::workDirectoryButtonSlot> entered.");

  QString error_message;
  QString dir_name = m_working_dirname;

  if(dir_name.isEmpty())
  {
    dir_name = QDir::currentDirPath();
  }
  dir_name = QFileDialog::getExistingDirectory(
    dir_name,
    this,
    0,
    "Choose work directory:"
  );
  Message(" --- got work directory \"")
  (dir_name)
  ("\".");

  if(isCurrentDirValid(dir_name, error_message)){
    if(QDir::setCurrent(dir_name)){
      m_working_dirname = dir_name;
      m_work_directory_label->setText(m_working_dirname);
      
      isValid();
    } else
      GenFatal("Couldn't reset current directory, don't know why.");
  } else if(dir_name != QString::null){
    QMessageBox::information(
      this,
      "Work directory",
      "Can't switch to work directory:\n"
      +
      error_message
    );
  }
}


WorldDimensionsPage::WorldDimensionsPage(
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
)
: AvidaSetupPage(parent, directions, help, file_comment),
  m_width_param_name(width_param_name),
  m_height_param_name(height_param_name)
{
  Message("<WorldDimensionsPage> constructor.");

  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  layout->addWidget(new QLabel("World width:", layout_widget));

  m_width_field = new QLineEdit(
    QString("%1").arg(default_width),
    layout_widget
  );
  layout->addWidget(m_width_field);
  /* FIXME: make palette a configurable parameter.  -- kaben.  */
  //m_width_field->setPalette(white);
  /* FIXME: make indent a configurable parameter.  -- kaben.  */
  //m_width_field->setIndent(6);
  m_width_field->setValidator(
    new QIntValidator(width_min, width_max, this)
  );

  layout->addItem(new QSpacerItem(20, 20));

  layout->addWidget(new QLabel("World height:", layout_widget));

  m_height_field = new QLineEdit(
    QString("%1").arg(default_height),
    layout_widget
  );
  layout->addWidget(m_height_field);
  /* FIXME: make palette a configurable parameter.  -- kaben.  */
  //m_height_field->setPalette(white);
  /* FIXME: make indent a configurable parameter.  -- kaben.  */
  //m_height_field->setIndent(6);
  m_height_field->setValidator(
    new QIntValidator(height_min, height_max, this)
  );
}

QString
WorldDimensionsPage::parameters(void){
  Message("<WorldDimensionsPage::parameters> entered.");

  return
    m_file_comment + '\n'
    + 
    m_width_param_name + ' ' + m_width_field->text() + '\n'
    +
    m_height_param_name + ' ' + m_height_field->text() + '\n';
}


TopologyChoicePage::TopologyChoicePage(
  QWizard *parent,
  const QString &directions,
  const QString &help,
  const QString &file_comment,
  const QString &topology_param_name
)
: AvidaSetupPage(parent, directions, help, file_comment),
  m_topology_param_name(topology_param_name)
{
  Message("<TopologyChoicePage> constructor.");

  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  layout->addWidget(new QLabel("Birth method", layout_widget));
  m_combobox = new QComboBox(layout_widget);
  layout->addWidget(m_combobox);
  m_combobox->insertItem("two-dimensional");
  m_combobox->insertItem("well-stirred");
}

QString
TopologyChoicePage::parameters(void){
  Message("<TopologyChoicePage::parameters> entered.");

  //QString result = m_file_comment + '\n' + m_topology_param_name + ' ';
  QString result;
  if(m_combobox->currentText() == "two-dimensional"){
    result += QString("%1").arg(2);
  } else {
    result += QString("%1").arg(4);
  }
  //result += '\n';
  
  Message(" --- returning " + result);

  return result;
}


PopulateChoicePage::PopulateChoicePage(
  QWizard *parent,
  const QString &directions,
  const QString &help,
  const QString &file_comment
)
: AvidaSetupPage(parent, directions, help, file_comment)
{
  Message("<PopulateChoicePage> constructor.");

  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  layout->addWidget(new QLabel("Populate method", layout_widget));
  m_combobox = new QComboBox(layout_widget);
  layout->addWidget(m_combobox);
  m_combobox->insertItem("inject-all");
  m_combobox->insertItem("inject-one");
}

QString
PopulateChoicePage::parameters(void){
  Message("<PopulateChoicePage::parameters> entered.");

  //QString result = m_file_comment + '\n';
  QString result;
  if(m_combobox->currentText() == "inject-all"){
    result +=
    "u 0 inject_all START_CREATURE # Inject all the begining";
  } else {
    result +=
    "u 0 inject START_CREATURE # Inject one the begining";
  }
  //result += '\n';
  
  Message(" --- returning " + result);

  return result;
}


EndConditionPage::EndConditionPage(
  QWizard *parent,
  const QString &directions,
  const QString &help,
  const QString &file_comment
)
: AvidaSetupPage(parent, directions, help, file_comment)
{
  Message("<EndConditionPage> constructor.");

  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  layout->addWidget(new QLabel(
    "Count by generations or updates", layout_widget
  ));
  m_combobox = new QComboBox(layout_widget);
  layout->addWidget(m_combobox);
  m_combobox->insertItem("update");
  m_combobox->insertItem("generation");

  layout->addItem(new QSpacerItem(20, 20));

  m_endtime_field = new QLineEdit(
    "100000",
    layout_widget
  );
  layout->addWidget(m_endtime_field);
  /* FIXME: make palette a configurable parameter.  -- kaben.  */
  //m_height_field->setPalette(white);
  /* FIXME: make indent a configurable parameter.  -- kaben.  */
  //m_height_field->setIndent(6);
  m_endtime_field->setValidator(
    new QIntValidator(1, 1000000, this)
  );
}

QString
EndConditionPage::parameters(void){
  Message("<EndConditionPage::parameters> entered.");

  //QString result =
  //  m_file_comment + '\n'
  //  +
  //  m_combobox->currentText() + m_endtime_field->text() + "exit\n";
  
  QString result =
    m_combobox->currentText() + ' ' + m_endtime_field->text() + " exit";
  
  Message(" --- returning " + result);

  return result;
}


