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
#include <qvbuttongroup.h>
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

#ifndef SETUP_WIZARD_PAGE2_HH
#include "setup_wizard_page2.hh"
#endif

#ifndef SETUP_GENESIS_HH
#include "setup_genesis.hh"
#endif


using namespace std;


AvidaSetupPage2::AvidaSetupPage2(
  const QString &directions,
  const QString &help,
  QWidget *parent,
  const char *name,
  WFlags f
):QHBox(parent, name, f),
  m_help(help)
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
}


SetupStylePage::SetupStylePage(
  QWidget *parent,
  const char *name,
  WFlags f
)
: AvidaSetupPage2(
    "random directions",
    "random help",
    parent,
    name,
    f
  )
{
  Message("<SetupStylePage>");
  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  QVButtonGroup *button_group = new QVButtonGroup(layout_widget);
  layout->addWidget(button_group);

  layout->addItem(new QSpacerItem(20, 20));

  QRadioButton *use_existing_genesis_button
    = new QRadioButton("Use an existing genesis file", button_group);
  connect(
    use_existing_genesis_button, SIGNAL(clicked(void)),
    this, SIGNAL(existingSetupSig(void))
  );

  QRadioButton *create_simple_genesis_button
    = new QRadioButton("Simple setup with new genesis file", button_group);
  connect(
    create_simple_genesis_button, SIGNAL(clicked(void)),
    this, SIGNAL(simpleSetupSig(void))
  );

  QRadioButton *create_detailed_genesis_button
    = new QRadioButton("Detailed setup with new genesis file", button_group);
  connect(
    create_detailed_genesis_button, SIGNAL(clicked(void)),
    this, SIGNAL(detailedSetupSig(void))
  );

  layout->addItem(new QSpacerItem(20, 20));
}


QValidator::State
avd_FileValidator::validate(QString &filepath, int &pos)
const
{
  QFileInfo genesis_fileinfo(filepath);
  if(filepath == QString::null){
    return QValidator::Intermediate;
  }
  if(!genesis_fileinfo.exists()){
    return QValidator::Intermediate;
  }
  if(!genesis_fileinfo.isReadable()){
    return QValidator::Intermediate;
  }
  return QValidator::Acceptable;
}


void
avd_FileValidator::fixup(QString &filepath)
const
{
  Message("<avd_FileValidator::fixup>");

  QFileInfo genesis_fileinfo(filepath);
  QString error_message;
  if(filepath == QString::null){
    return;
  }
  if(!genesis_fileinfo.exists()){
    error_message = "could not find the file \"" + filepath + "\".";
  } else if(!genesis_fileinfo.isReadable()){
    error_message = "the file \"" + filepath + "\" is not readable.";
  }
  QMessageBox::information( 
    0,
    "Existing genesis file",
    "Can't open existing genesis file:\n"
    +
    error_message
  );
}


ExistingGenesisPage::ExistingGenesisPage(
  GenesisWrapper *genwrap,
  QWidget *parent,
  const char *name,
  WFlags f
)
: AvidaSetupPage2(
    "random directions",
    "random help",
    parent,
    name,
    f
  ),
  m_genwrap(genwrap)
{
  Message("<ExistingGenesisPage>");
  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *vlayout = new QVBoxLayout(layout_widget);

  vlayout->addItem(new QSpacerItem(20, 20));


  m_genesis_filename_lineedit = new QLineEdit(layout_widget);
  m_genesis_filename_lineedit->setValidator(
    new avd_FileValidator(this, "avd_FileValidator")
  );
  connect(
    m_genesis_filename_lineedit, SIGNAL(textChanged(const QString&)),
    this, SLOT(genesisFilenameLineEditChanged(void))
  );
  connect(
    m_genesis_filename_lineedit, SIGNAL(returnPressed()),
    this, SLOT(genesisFilenameLineEditSlot(void))
  );
  vlayout->addWidget(m_genesis_filename_lineedit);

  QHBoxLayout *hlayout1 = new QHBoxLayout(vlayout);
  hlayout1->addItem(new QSpacerItem(20, 20));

  m_genesis_filename_button = new QPushButton(
    "Set Genesis File",
    layout_widget
  );
  hlayout1->addWidget(m_genesis_filename_button);
  connect(
    m_genesis_filename_button, SIGNAL(clicked(void)),
    this, SLOT(genesisFilenameButtonSlot(void))
  );

  hlayout1->addItem(new QSpacerItem(20, 20));


  vlayout->addItem(new QSpacerItem(20, 20));


  /*
  FIXME: move me elsewhere...
  FIXME: this is really ugly...
  -- kaben.
  */
  QString ignore;
  if(m_existing_genesis_filename == QString::null)
  {
    // m_existing_genesis_filename appears unset.  try to set it.
    if(isExistingGenesisValid(
      QFileInfo("genesis").absFilePath(),
      ignore
    ))
    { 
      // found valid genesis file in cwd.
      // place genesis path in lineedit.
      Message("<ExistingGenesisPage> found genesis file in cwd.");
      m_existing_genesis_filename = QFileInfo("genesis").absFilePath();
      m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
      Message(" --- m_existing_genesis_filename: \"")
      (m_existing_genesis_filename + "\"");
    } else { 
      // didn't find valid genesis file in cwd; try environment
      // variable.
      char *env = getenv("AVIDA_GENESIS_FILE");
      if(env && isExistingGenesisValid(QString(env), ignore))
      {   
        // genesis file spec in environment variable; try to validate it.
        if( 
          isExistingGenesisValid(
            QFileInfo(
              QString(env)
            ).absFilePath(),
            ignore
          )
        ){
          // environment variable held valid genesis path.
          // place genesis path in lineedit.
          Message("<ExistingGenesisPage>")
          ("found genesis file in $AVIDA_GENESIS_FILE.");
          m_existing_genesis_filename = QFileInfo(
            QString(env)
          ).absFilePath();
          m_genesis_filename_lineedit->setText(
            m_existing_genesis_filename
          );
        } else {
          // environment variable genesis path was invalid.
          Message("<ExistingGenesisPage> didn't find genesis file.");
          free(env);
        }
      } else {
        // environment variable wasn't set.
        Message("<ExistingGenesisPage> didn't find genesis file.");
      }
    } 
  } else {
    // m_existing_genesis_filename already has a value.  try to
    // validate.
    if(isExistingGenesisValid(
      QFileInfo(m_existing_genesis_filename).absFilePath(),
      ignore
    ))
    {
      // m_existing_genesis_filename has valide genesis path.
      // place genesis path in lineedit.
      Message
      ("<ExistingGenesisPage> m_existing_genesis_filename valid.");
      m_existing_genesis_filename =
        QFileInfo(m_existing_genesis_filename).absFilePath();
      m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
      Message(" --- m_existing_genesis_filename: \"")
      (m_existing_genesis_filename + "\"");
    }
  }

  isValid();
}

bool
ExistingGenesisPage::isValid(void)
{
  Message("<ExistingGenesisPage::isValid>");

  QString ignore;
  if(isExistingGenesisValid(m_genesis_filename_lineedit->text(), ignore)){
    emit isValidSig(true);
    return true;
  }
  emit isValidSig(false);
  return false;
}

bool
ExistingGenesisPage::isExistingGenesisValid(
  const QString &filepath,
  QString &message
){
  Message("<ExistingGenesisPage::isExistingGenesisValid>");

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
  if (0 == generateFilepathsFromExistingGenesis(filepath))
  {
    message = "the file \"" + filepath + "\" is not readable.";
    return false;
  }
  return true;
}

int
ExistingGenesisPage::generateFilepathsFromExistingGenesis(
  const QString &genesis_path
){
  Message("<ExistingGenesisPage::generateFilepathsFromExistingGenesis>");

  cGenesis *genesis = new cGenesis();
  genesis->SetVerbose();
  int fd = genesis->Open(genesis_path.latin1());

  if(0 == fd){
    m_environment_filename = "";
    m_instruction_set_filename = "";
    m_start_creature_filename = "";
    m_events_filename = "";
  } else {
    m_environment_filename = absPath(
      QString(
        genesis->ReadString("ENVIRONMENT_FILE", "environment.cfg")
      ),
      genesis_path
    );
    m_instruction_set_filename = absPath(
      QString(
        genesis->ReadString("INST_SET", "inst_set")
      ),
      genesis_path
    );
    m_start_creature_filename = absPath(
      QString(
        genesis->ReadString("START_CREATURE")
      ),
      genesis_path
    );
    m_events_filename = absPath(
      QString(
        genesis->ReadString("EVENT_FILE", "events.cfg")
      ),
      genesis_path
    );
    m_genwrap->setGenesis(genesis);
  }

  Message("<ExistingGenesisPage::generateFilepathsFromExistingGenesis> done.");

  return fd;
}

QString
ExistingGenesisPage::absPath(
  const QString &filepath,
  const QString &genesis_path
){
  Message("<ExistingGenesisPage::absPath>");
  
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
ExistingGenesisPage::genesisFilenameButtonSlot(void)
{
  Message("<ExistingGenesisPage::genesisFilenameButtonSlot>");

  QString file_name;
  QString error_message;

  file_name = m_existing_genesis_filename;
  Message(" --- file_name: \"" + m_existing_genesis_filename + "\"");
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
    m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
    emit(isValidSig(true));
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


void
ExistingGenesisPage::genesisFilenameLineEditSlot(void)
{
  Message("<ExistingGenesisPage::genesisFilenameLineEditSlot>");

  QString file_name;
  QString error_message;

  file_name = m_genesis_filename_lineedit->text();
  Message(" --- file_name: \"" + m_existing_genesis_filename + "\"");
  /*
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
  */
  
  if(isExistingGenesisValid(file_name, error_message)){
    m_existing_genesis_filename = file_name;
    m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
    emit(isValidSig(true));
  } else if(file_name != QString::null){
    QMessageBox::information( 
      0,
      "Existing genesis file",
      "Can't open existing genesis file:\n"
      +
      error_message
    );
  }
}


void
ExistingGenesisPage::genesisFilenameLineEditChanged(void)
{
  Message("<ExistingGenesisPage::genesisFilenameLineEditChanged>");
  emit(isValidSig(false));
}

WorkingDirectoryPage::WorkingDirectoryPage(
  GenesisWrapper *genwrap,
  QWidget *parent,
  const char *name,
  WFlags f
)
: AvidaSetupPage2(
    "random directions",
    "random help",
    parent,
    name,
    f
  ),
  m_genwrap(genwrap)
{
  Message("<WorkingDirectoryPage>");
  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *vlayout = new QVBoxLayout(layout_widget);

  vlayout->addItem(new QSpacerItem(20, 20));


  m_working_dirname_lineedit = new QLineEdit(layout_widget);
  //m_working_dirname_lineedit->setValidator(
  //  new avd_FileValidator(this, "avd_FileValidator")
  //);
  //connect(
  //  m_working_dirname_lineedit, SIGNAL(textChanged(const QString&)),
  //  this, SLOT(genesisFilenameLineEditChanged(void))
  //);
  //connect(
  //  m_working_dirname_lineedit, SIGNAL(returnPressed()),
  //  this, SLOT(genesisFilenameLineEditSlot(void))
  //);
  vlayout->addWidget(m_working_dirname_lineedit);

  QHBoxLayout *hlayout1 = new QHBoxLayout(vlayout);
  hlayout1->addItem(new QSpacerItem(20, 20));

  m_working_dirname_button = new QPushButton(
    "Set Working Directory",
    layout_widget
  );
  hlayout1->addWidget(m_working_dirname_button);
  //connect(
  //  m_working_dirname_button, SIGNAL(clicked(void)),
  //  this, SLOT(genesisFilenameButtonSlot(void))
  //);

  hlayout1->addItem(new QSpacerItem(20, 20));


  vlayout->addItem(new QSpacerItem(20, 20));


  /*
  FIXME: move me elsewhere...
  FIXME: this is really ugly...
  -- kaben.
  */
  //QString ignore;
  //if(m_existing_genesis_filename == QString::null)
  //{
  //  // m_existing_genesis_filename appears unset.  try to set it.
  //  if(isExistingGenesisValid(
  //    QFileInfo("genesis").absFilePath(),
  //    ignore
  //  ))
  //  { 
  //    // found valid genesis file in cwd.
  //    // place genesis path in lineedit.
  //    Message("<WorkingDirectoryPage> found genesis file in cwd.");
  //    m_existing_genesis_filename = QFileInfo("genesis").absFilePath();
  //    m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
  //    Message(" --- m_existing_genesis_filename: \"")
  //    (m_existing_genesis_filename + "\"");
  //  } else { 
  //    // didn't find valid genesis file in cwd; try environment
  //    // variable.
  //    char *env = getenv("AVIDA_GENESIS_FILE");
  //    if(env && isExistingGenesisValid(QString(env), ignore))
  //    {   
  //      // genesis file spec in environment variable; try to validate it.
  //      if( 
  //        isExistingGenesisValid(
  //          QFileInfo(
  //            QString(env)
  //          ).absFilePath(),
  //          ignore
  //        )
  //      ){
  //        // environment variable held valid genesis path.
  //        // place genesis path in lineedit.
  //        Message("<WorkingDirectoryPage>")
  //        ("found genesis file in $AVIDA_GENESIS_FILE.");
  //        m_existing_genesis_filename = QFileInfo(
  //          QString(env)
  //        ).absFilePath();
  //        m_genesis_filename_lineedit->setText(
  //          m_existing_genesis_filename
  //        );
  //      } else {
  //        // environment variable genesis path was invalid.
  //        Message("<WorkingDirectoryPage> didn't find genesis file.");
  //        free(env);
  //      }
  //    } else {
  //      // environment variable wasn't set.
  //      Message("<WorkingDirectoryPage> didn't find genesis file.");
  //    }
  //  } 
  //} else {
  //  // m_existing_genesis_filename already has a value.  try to
  //  // validate.
  //  if(isExistingGenesisValid(
  //    QFileInfo(m_existing_genesis_filename).ablsFilePath(),
  //    ignore
  //  ))
  //  {
  //    // m_existing_genesis_filename has valide genesis path.
  //    // place genesis path in lineedit.
  //    Message("<WorkingDirectoryPage> m_existing_genesis_filename valid.")
  //    m_existing_genesis_filename =
  //      QFileInfo(m_existing_genesis_filename).absFilePath();
  //    m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
  //    Message(" --- m_existing_genesis_filename: \"")
  //    (m_existing_genesis_filename + "\"");
  //  }
  //}

  //isValid();
}

bool
WorkingDirectoryPage::isValid(void)
{
  Message("<WorkingDirectoryPage::isValid>");

  QString ignore;
  if(isWorkingDirValid(m_working_dirname_lineedit->text(), ignore)){
    emit isValidSig(true);
    return true;
  }
  emit isValidSig(false);
  return false;
}

bool
WorkingDirectoryPage::isWorkingDirValid(
  const QString &dirpath,
  QString &message
){
  Message("<WorkingDirectoryPage::isWorkingDirValid>");

  /*
  FIXME: need more validation.
  be sure that environment file, instruction set file, event file, and
  start creature as specified in genesis file are readable and valid.
  -- kaben.
  */
  //QFileInfo genesis_fileinfo(filepath);
  //if(filepath == QString::null){
  //  message = "genesis file name was null.";
  //  return false;
  //}
  //if(!genesis_fileinfo.exists()){
  //  message = "the file \"" + filepath + "\" does not exist.";
  //  return false;
  //}
  //if(!genesis_fileinfo.isReadable()){
  //  message = "the file \"" + filepath + "\" is not readable.";
  //  return false;
  //}
  //if (0 == generateFilepathsFromExistingGenesis(filepath))
  //{
  //  message = "the file \"" + filepath + "\" is not readable.";
  //  return false;
  //}
  return true;
}

#if 0
int
ExistingGenesisPage::generateFilepathsFromExistingGenesis(
  const QString &genesis_path
){
  Message("<ExistingGenesisPage::generateFilepathsFromExistingGenesis>");

  cGenesis *genesis = new cGenesis();
  genesis->SetVerbose();
  int fd = genesis->Open(genesis_path.latin1());

  if(0 == fd){
    m_environment_filename = "";
    m_instruction_set_filename = "";
    m_start_creature_filename = "";
    m_events_filename = "";
  } else {
    m_environment_filename = absPath(
      QString(
        genesis->ReadString("ENVIRONMENT_FILE", "environment.cfg")
      ),
      genesis_path
    );
    m_instruction_set_filename = absPath(
      QString(
        genesis->ReadString("INST_SET", "inst_set")
      ),
      genesis_path
    );
    m_start_creature_filename = absPath(
      QString(
        genesis->ReadString("START_CREATURE")
      ),
      genesis_path
    );
    m_events_filename = absPath(
      QString(
        genesis->ReadString("EVENT_FILE", "events.cfg")
      ),
      genesis_path
    );
    m_genwrap->setGenesis(genesis);
  }

  Message("<ExistingGenesisPage::generateFilepathsFromExistingGenesis> done.");

  return fd;
}

QString
ExistingGenesisPage::absPath(
  const QString &filepath,
  const QString &genesis_path
){
  Message("<ExistingGenesisPage::absPath>");
  
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
ExistingGenesisPage::genesisFilenameButtonSlot(void)
{
  Message("<ExistingGenesisPage::genesisFilenameButtonSlot>");

  QString file_name;
  QString error_message;

  file_name = m_existing_genesis_filename;
  Message(" --- file_name: \"" + m_existing_genesis_filename + "\"");
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
    m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
    emit(isValidSig(true));
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


void
ExistingGenesisPage::genesisFilenameLineEditSlot(void)
{
  Message("<ExistingGenesisPage::genesisFilenameLineEditSlot>");

  QString file_name;
  QString error_message;

  file_name = m_genesis_filename_lineedit->text();
  Message(" --- file_name: \"" + m_existing_genesis_filename + "\"");
  /*
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
  */
  
  if(isExistingGenesisValid(file_name, error_message)){
    m_existing_genesis_filename = file_name;
    m_genesis_filename_lineedit->setText(m_existing_genesis_filename);
    emit(isValidSig(true));
  } else if(file_name != QString::null){
    QMessageBox::information( 
      0,
      "Existing genesis file",
      "Can't open existing genesis file:\n"
      +
      error_message
    );
  }
}


void
ExistingGenesisPage::genesisFilenameLineEditChanged(void)
{
  Message("<ExistingGenesisPage::genesisFilenameLineEditChanged>");
  emit(isValidSig(false));
}
#endif


NewGenesisPage::NewGenesisPage(
  GenesisWrapper *genwrap,
  QWidget *parent,
  const char *name,
  WFlags f
)
: AvidaSetupPage2(
    "random directions",
    "random help",
    parent,
    name,
    f
  ),
  m_genwrap(genwrap)
{
  Message("<NewGenesisPage>");
  QWidget *layout_widget = new QWidget(m_page);
  QVBoxLayout *layout = new QVBoxLayout(layout_widget);

  QVButtonGroup *button_group = new QVButtonGroup(layout_widget);
  layout->addWidget(button_group);

  layout->addItem(new QSpacerItem(20, 20));

  QRadioButton *use_existing_genesis_button
    = new QRadioButton("Use an existing genesis file", button_group);

  QRadioButton *create_simple_genesis_button
    = new QRadioButton("Simple setup with new genesis file", button_group);

  QRadioButton *create_detailed_genesis_button
    = new QRadioButton("Detailed setup with new genesis file", button_group);

  layout->addItem(new QSpacerItem(20, 20));
}


ExistingFinalPage::ExistingFinalPage(
  GenesisWrapper *genwrap,
  QWidget *parent,
  const char *name,
  WFlags f
)
: FinalPage(
    genwrap,
    "existing final page",
    "random help",
    parent,
    name,
    f
  )
{
  Message("<ExistingFinalPage>");
}


SimpleFinalPage::SimpleFinalPage(
  GenesisWrapper *genwrap,
  QWidget *parent,
  const char *name,
  WFlags f
)
: FinalPage(
    genwrap,
    "simple final page",
    "random help",
    parent,
    name,
    f
  )
{
  Message("<SimpleFinalPage>");
}


DetailedFinalPage::DetailedFinalPage(
  GenesisWrapper *genwrap,
  QWidget *parent,
  const char *name,
  WFlags f
)
: FinalPage(
    genwrap,
    "detailed final page",
    "random help",
    parent,
    name,
    f
  )
{
  Message("<DetailedFinalPage>");
}

