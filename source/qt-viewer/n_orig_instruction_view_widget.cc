//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qlayout.h>
#include <qhbox.h>
//#include <qscrollview.h>
#include <qsizepolicy.h>
#include <qlabel.h>
#include <qpopupmenu.h>

#include "avd_mission_control.hh"
#include "n_orig_instruction_scrollview.hh"
#include "n_orig_instruction_line_widget.hh"
#include "inst_set.hh"
#include "hardware_cpu.hh"
#include "population_cell_wrapped_accessors.hh"

#include "n_orig_instruction_view_widget.hh"

#ifndef FUNCTIONS_HH
#include "functions.hh"
#endif

#ifndef DEBUG
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#else
//# define LOCAL_DEBUG 1
//# define USE_LOCAL_STRINGS 1
# define LOCAL_DEBUG 0
# define USE_LOCAL_STRINGS 0
#endif


using namespace std;


N_Instruction_ViewWidget::N_Instruction_ViewWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f), m_instr_lines_list(2048)
{
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::N_Instruction_ViewWidget> entered.\n";
  #endif

  setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));
  m_vboxlayout = new QVBoxLayout(this);
  m_header = new QWidget(this);
  m_header->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_header->setFixedHeight(N_Instruction_ScrollView::s_line_number_label_height);
  m_header_layout = new QHBoxLayout(m_header);
  m_vboxlayout->addWidget(m_header);

  m_break_label = new QLabel("<b>brk</b>", m_header);
  m_break_label->setMinimumWidth(N_Instruction_ScrollView::s_break_button_width);
  m_break_label->setMinimumHeight(N_Instruction_ScrollView::s_break_button_height);
  m_break_label->setAlignment(Qt::AlignHCenter);
  m_header_layout->addWidget(m_break_label);

  m_lineno_label = new QLabel("<b>line</b>", m_header);
  m_lineno_label->setMinimumWidth(N_Instruction_ScrollView::s_line_number_label_width);
  m_lineno_label->setMinimumHeight(N_Instruction_ScrollView::s_line_number_label_height);
  m_lineno_label->setAlignment(Qt::AlignHCenter);
  m_header_layout->addWidget(m_lineno_label);

  m_head_label = new QLabel("<b>heads</b>", m_header);
  m_head_label->setMinimumWidth(4 * N_Instruction_ScrollView::s_head_button_width);
  m_head_label->setMinimumHeight(N_Instruction_ScrollView::s_head_button_height);
  m_head_label->setAlignment(Qt::AlignHCenter);
  m_header_layout->addWidget(m_head_label);

  m_flag_label = new QLabel("<b>flags</b>", m_header);
  m_flag_label->setMinimumWidth(3 * N_Instruction_ScrollView::s_flag_label_width);
  m_flag_label->setMinimumHeight(N_Instruction_ScrollView::s_flag_label_height);
  m_flag_label->setAlignment(Qt::AlignHCenter);
  m_header_layout->addWidget(m_flag_label);

  m_instr_label = new QLabel("<b>instruction</b>", m_header);
  m_instr_label->setMinimumWidth(N_Instruction_ScrollView::s_instr_label_width);
  m_instr_label->setMinimumHeight(N_Instruction_ScrollView::s_instr_label_height);
  m_instr_label->setAlignment(Qt::AlignHCenter);
  m_header_layout->addWidget(m_instr_label);

  //m_scroll_view = new N_Instruction_ScrollView(this);
  m_scroll_view = new N_Instruction_ScrollView(
    this,
    "<N_Instruction_ViewWidget::m_scroll_view>",
    WStaticContents | WRepaintNoErase
  );
  connect(this, SIGNAL(enableButtonsSig(bool)), m_scroll_view, SLOT(setButtonsEnabled(bool)));
  connect(m_scroll_view, SIGNAL(buttonSig(int, int)), this, SLOT(lineButtonSlot(int, int)));
  connect(m_scroll_view, SIGNAL(instructionSig(const QPoint &, int)),
    this, SLOT(lineInstSlot(const QPoint &, int)));
  m_scroll_view->enableClipper(true);
  m_vboxlayout->addWidget(m_scroll_view);
  m_instr_lines_list.setAutoDelete(true);
  checkInstrLinesListSize(1);

  /*
  This is how we enable mouse tracking within the scrollview.
  Apparently must be called after creation of the scrollview;
  trying to call from withing the scrollview constructor fails;
  presumably something behind the scenes undoes the call before
  constructor completion.
  */
  m_scroll_view->viewport()->setMouseTracking(true);

  //m_message_label = new QLabel("burp...", this);
  //m_message_label->setMargin(m_message_label->lineWidth());
  //m_message_label->setFrameStyle(QFrame::Panel);
  //m_vboxlayout->addWidget(m_message_label);

  //m_subcontainer->adjustSize();
  //m_container->adjustSize();

  m_scroll_view->setHScrollBarMode(QScrollView::AlwaysOff);
  //m_scroll_view->setMinimumWidth(m_container->width());
  //m_scroll_view->setMinimumHeight(20 + 16 * m_subcontainer->height());
  //m_scroll_view->setResizePolicy(QScrollView::AutoOneFit);

  /*
  m_scroll_view->setMinimumWidth(m_instr_lines_list[0]->width());
  m_scroll_view->setMinimumHeight(20 + 16 * m_instr_lines_list[0]->height());
  */
  m_scroll_view->setMinimumWidth(N_Instruction_ScrollView::s_button_width);
  m_scroll_view->setMinimumHeight(N_Instruction_ScrollView::s_button_height);
  m_header_layout->setMargin(1);
  setFixedWidth(m_scroll_view->width());
  m_instr_modifier_menu = new QPopupMenu(this);
  m_instr_change_menu = new QPopupMenu(this);
  connect(m_instr_change_menu, SIGNAL(aboutToShow()), this, SLOT(instChangeMenuAboutToShow()));
  connect(m_instr_change_menu, SIGNAL(activated(int)), this, SLOT(instChangeMenuItemActivated(int)));
  m_instr_modifier_menu->insertItem("Change to", m_instr_change_menu, CHANGE);
  m_instr_modifier_menu->insertItem("Remove", this, SLOT(instRemoveMenuItemActivated()), 0, REMOVE);
  
  m_instr_insert_menu = new QPopupMenu(this);
  connect(m_instr_insert_menu, SIGNAL(aboutToShow()), this, SLOT(instInsertMenuAboutToShow()));
  connect(m_instr_insert_menu, SIGNAL(activated(int)), this, SLOT(instInsertMenuItemActivated(int)));
  m_instr_modifier_menu->insertItem("Insert", m_instr_insert_menu, INSERT);
}

N_Instruction_ViewWidget::~N_Instruction_ViewWidget(void){
  #if 0
  cout
  << "<N_Instruction_ViewWidget::~N_Instruction_ViewWidget>\n"
  << " --- destructor called.\n";
  #endif
}

void
N_Instruction_ViewWidget::setMissionControl(
  avd_MissionControl *mission_control
){
  m_mission_control = mission_control;
}

void
N_Instruction_ViewWidget::buttonsSafe(bool safe){
  cout
  << "<N_Instruction_ViewWidget::buttonsSafe> called.\n";
  m_buttons_safe = safe;
  emit enableButtonsSig(safe);
}

void
N_Instruction_ViewWidget::setPopulationCell(int cell_id){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::setPopulationCell> cell_id "
  << cell_id << ".\n";
  #endif

  m_cell_id = cell_id;
  updateViewer(true);
}

void
N_Instruction_ViewWidget::updateState(void){
  updateViewer(false);
}

void
N_Instruction_ViewWidget::checkInstrLinesListSize(int size){
  #if LOCAL_DEBUG
  cout << "<N_Instruction_ViewWidget::checkInstrLinesListSize> entered.\n";
  #endif
  int counter = m_instr_lines_list.count();
  if(counter < size){
    cCPUMemory memory;
    cInstSet inst_set;
    cHardwareCPU *hardware =
      GetHardwareCPU(
        GetOrganism(
          GetPopulationCell(m_mission_control, m_cell_id)));
    if(hardware != NULL){
      memory = hardware->IP().GetMemory();
      inst_set = hardware->GetInstSet();
    }
    if(m_instr_lines_list.size() < size) m_instr_lines_list.resize(size);
    while(counter < size){
      #if LOCAL_DEBUG
      cout << " --- m_instr_lines_list.count() " << m_instr_lines_list.count()
      << ", counter " << counter << ".\n";
      #endif
      N_Instruction_LineWidget *line_widget = new N_Instruction_LineWidget(counter);
      m_scroll_view->addChild(line_widget);

      m_instr_lines_list.insert(counter, line_widget);
      if(hardware != NULL)
      { m_instr_lines_list[counter]->setInstructionLine(memory, inst_set, true); }
      counter++;
    }
  }
  #if LOCAL_DEBUG
  cout << "<N_Instruction_ViewWidget::checkInstrLinesListSize> done.\n";
  #endif
}

void
N_Instruction_ViewWidget::lineInstSlot(const QPoint &point, int lineno){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::lineInstSlot> "
  << "point.x() " << point.x()
  << ", point.y() " << point.y()
  << ", lineno " << lineno << ".\n";
  #endif

  if(m_buttons_safe){
    #if LOCAL_DEBUG
    cout << " --- buttons are safe.\n";
    cout << " --- opening menu.\n";
    #endif

    m_menu_lineno = lineno;
    //m_instr_modifier_menu->popup(point);
    m_instr_modifier_menu->exec(point);
    updateViewer(false, false);
  } else {
    #if LOCAL_DEBUG
    cout << " --- buttons are NOT safe.\n";
    #endif
  }
  #if LOCAL_DEBUG
  cout << "<N_Instruction_ViewWidget::lineInstSlot> done.\n";
  #endif
}

void
N_Instruction_ViewWidget::instChangeMenuItemActivated(int id){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::instChangeMenuItemActivated> id "
  << id << ".\n";
  #endif

  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)
      )
    );

  if(hardware == NULL){
    #if LOCAL_DEBUG
    cout << " --- hardware == NULL.  returning.\n";
    #endif

    return;
  }

  cCPUHead edit_head(hardware->IP());
  edit_head.AbsSet(m_menu_lineno);
  edit_head.SetInst(cInstruction(id));
}

void
N_Instruction_ViewWidget::instRemoveMenuItemActivated(void){
  #if LOCAL_DEBUG
  cout << "<N_Instruction_ViewWidget::instRemoveMenuItemActivated>.\n";
  #endif

  m_activated_menu = REMOVE;

  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)
      )
    );

  if(hardware == NULL){
    #if LOCAL_DEBUG
    cout << " --- hardware == NULL.  returning.\n";
    #endif

    return;
  }

  cCPUHead edit_head(hardware->IP());
  edit_head.AbsSet(m_menu_lineno);
  edit_head.RemoveInst();
}

void
N_Instruction_ViewWidget::instInsertMenuItemActivated(int id){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::instInsertMenuItemActivated> id "
  << id << ".\n";
  #endif

  m_activated_menu = INSERT;

  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)
      )
    );

  if(hardware == NULL){
    #if LOCAL_DEBUG
    cout << " --- hardware == NULL.  returning.\n";
    #endif

    return;
  }

  cCPUHead edit_head(hardware->IP());
  edit_head.AbsSet(m_menu_lineno);
  edit_head.InsertInst(cInstruction(id));
}

void
N_Instruction_ViewWidget::instChangeMenuAboutToShow(void){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::instChangeMenuAboutToShow> called.\n";
  #endif

  updateInstrSubmenu(m_instr_change_menu);
}

void
N_Instruction_ViewWidget::instInsertMenuAboutToShow(void){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::instInsertMenuAboutToShow> called.\n";
  #endif

  updateInstrSubmenu(m_instr_insert_menu);
}

void
N_Instruction_ViewWidget::updateInstrSubmenu(QPopupMenu *submenu){
  int i;

  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)
      )
    );
  if(hardware){
    cInstSet inst_set = hardware->GetInstSet();
    for(i = 0; i < inst_set.GetSize(); i++){
      QString instr_i_text(inst_set.GetName(i)());

      // verify menuitem i exists.
      if(submenu->idAt(i) == i){
        // verify menuitem i has correct text.
        if(submenu->text(i) != instr_i_text){
          // menuitem i needs text updated.
          submenu->changeItem(i, instr_i_text);
        }
      } else { // if it doesn't exist, insert it.
        submenu->insertItem(
          instr_i_text, i, i
        );
      }
    }

    // if there are too many menuitems, delete the excess.
    int menu_item_count = submenu->count();
    if(inst_set.GetSize() < menu_item_count){
      while(i < menu_item_count){
        submenu->removeItem(i);
        i++;
      }
    }
  } else {
    // there's no hardware for this cell, so clear all menuitems.
    /*
    FIXME:  under current logic we shouldn't ever be here, so maybe we
    should abort?
    */
    int menu_item_count = submenu->count();
    for(i = 0; i < menu_item_count; i++){
      submenu->removeItemAt(i);
    }
  }
}

void
N_Instruction_ViewWidget::lineButtonSlot(int id, int lineno){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_ViewWidget::lineButtonSlot> "
  << "id " << id << ", lineno " << lineno << ".\n";
  #endif

  if(m_buttons_safe){
    cHardwareCPU *hardware =
      GetHardwareCPU(
        GetOrganism(
          GetPopulationCell(m_mission_control, m_cell_id)
        )
      );
    if (hardware == NULL){
      #if LOCAL_DEBUG
      cout << "<N_Instruction_ViewWidget::lineButtonSlot> hardware == NULL.\n";
      #endif

      return;
    }
    #if LOCAL_DEBUG
    cout << "<N_Instruction_ViewWidget::lineButtonSlot> got valid hardware.\n";
    cout << " --- buttons are safe.\n";
    #endif

    switch((N_Instruction_ScrollView::ButtonEnum)id){
    case N_Instruction_ScrollView::BUTTON_BP: {
        #if LOCAL_DEBUG
        cout << " --- BP button id.\n";
        #endif

        cCPUHead edit_head(hardware->IP());
        edit_head.AbsSet(lineno);
        ToggleBool(edit_head.FlagBreakpoint());
      }
      break;
    case N_Instruction_ScrollView::BUTTON_IH:
      #if LOCAL_DEBUG
      cout << " --- IH button id.\n";
      #endif

      hardware->GetHead(HEAD_IP).AbsSet(lineno);
      break;
    case N_Instruction_ScrollView::BUTTON_RH:
      #if LOCAL_DEBUG
      cout << " --- RH button id.\n";
      #endif

      hardware->GetHead(HEAD_READ).AbsSet(lineno);
      break;
    case N_Instruction_ScrollView::BUTTON_WH:
      #if LOCAL_DEBUG
      cout << " --- WH button id.\n";
      #endif

      hardware->GetHead(HEAD_WRITE).AbsSet(lineno);
      break;
    case N_Instruction_ScrollView::BUTTON_FH:
      #if LOCAL_DEBUG
      cout << " --- FH button id.\n";
      #endif

      hardware->GetHead(HEAD_FLOW).AbsSet(lineno);
      break;
    case N_Instruction_ScrollView::BUTTON_INST:
      #if LOCAL_DEBUG
      cout << " --- INST button id (shouldn't be caught here).\n";
      #endif

      return;
      break; // yah, not reached.
    default:
      #if LOCAL_DEBUG
      cout << " --- unknown button id.\n";
      #endif

      return;
      break; // yah, not reached.
    }
    updateViewer(false, false);
  } else {
    #if LOCAL_DEBUG
    cout << " --- buttons are NOT safe.\n";
    #endif

  }
}

void
N_Instruction_ViewWidget::updateViewer(
  bool force_redraw,
  bool ensure_visible
){
  #if LOCAL_DEBUG
  cout << "<N_Instruction_ViewWidget::updateViewer> called.\n";
  #endif

  m_mission_control->lock();
  cHardwareCPU *hardware =
    GetHardwareCPU(
      GetOrganism(
        GetPopulationCell(m_mission_control, m_cell_id)
      )
    );

  if(hardware != 0){
    /*
    PREP:  get appropriate data from avida.
    */
    const cCPUMemory &memory = hardware->IP().GetMemory();
    const cInstSet &inst_set = hardware->GetInstSet();
    int ihead_pos = hardware->GetHead(HEAD_IP).GetPosition();
    int rhead_pos = hardware->GetHead(HEAD_READ).GetPosition();
    int whead_pos = hardware->GetHead(HEAD_WRITE).GetPosition();
    int fhead_pos = hardware->GetHead(HEAD_FLOW).GetPosition();

    /*
    STEP 0:  if needed, allocate space for managing display lines.
    */
    int memory_size = memory.GetSize();
    checkInstrLinesListSize(memory_size);

    /*
    STEP 1:  check for changes in breakpoints, other flags, and
    instructions.
    */

    int i;
    for(i = 0; i < memory_size; i++){
      m_instr_lines_list[i]->setInstructionLine(
        memory,
        inst_set,
        force_redraw
      );
      m_instr_lines_list[i]->clearHeadButtons();
    }

    /*
    STEP 3:  clear remaining head buttons.
    */
    //for(i = 0; i < memory_size; i++) m_instr_lines_list[i]->clearHeadButtons();

    /*
    STEP 2:  set some head buttons.
    */
    m_instr_lines_list.at(fhead_pos)->setFHeadButton();
    m_instr_lines_list.at(whead_pos)->setWHeadButton();
    m_instr_lines_list.at(rhead_pos)->setRHeadButton();
    m_instr_lines_list.at(ihead_pos)->setIHeadButton();

    /*
    STEP 4:  update viewer widget
    (try to do this as quickly as possible).
    */
    m_scroll_view->viewport()->setUpdatesEnabled(false);
    m_scroll_view->resizeContents(
      N_Instruction_ScrollView::s_button_width,
      N_Instruction_ScrollView::s_button_height * memory_size
      //(
      //  m_instr_lines_list[memory_size - 1]->m_y
      //  +
      //  N_Instruction_ScrollView::s_button_height
      //)
    );
    if(ensure_visible){
      /*
      FIXME:  whether to use ensureVisible or center, and margins,
      should be configurable.
      */
      //m_scroll_view->ensureVisible(
      //  0, ((( 2 * ihead_pos) + 1) * N_Instruction_ScrollView::s_button_height) / 2,
      //  0, N_Instruction_ScrollView::s_button_height
      //);
      m_scroll_view->center(
        //N_Instruction_ScrollView::s_button_width / 2,
        0,
        ( ( 2 * ihead_pos) + 1)
        *
        N_Instruction_ScrollView::s_button_height / 2,
        0.0,
        0.7
      );
    }
    int toprow =
      m_scroll_view->contentsX()
      /
      N_Instruction_ScrollView::s_button_height;
    int bottomrow =
      ( m_scroll_view->contentsX()
        +
        m_scroll_view->visibleHeight() 
        +
        N_Instruction_ScrollView::s_button_height
        -
        1
      )
      /
      N_Instruction_ScrollView::s_button_height;
    for(i = toprow; (i < memory_size) && (i <= bottomrow); i++){
      if(m_instr_lines_list[i]->m_is_dirty){
        m_scroll_view->updateContents(
          m_instr_lines_list[i]->m_x,
          m_instr_lines_list[i]->m_y,
          N_Instruction_ScrollView::s_button_width,
          N_Instruction_ScrollView::s_button_height
        );
      }
    }
    m_scroll_view->repaintContents(
      0, m_scroll_view->contentsHeight(),
      m_scroll_view->contentsWidth(),
      m_scroll_view->visibleHeight() - m_scroll_view->contentsHeight(),
      false
    );
    m_scroll_view->viewport()->setUpdatesEnabled(true);
    m_scroll_view->viewport()->update();
  } else {
    m_scroll_view->resizeContents(
      N_Instruction_ScrollView::s_button_width,
      0
    );
    m_scroll_view->repaintContents(
      0, m_scroll_view->contentsHeight(),
      m_scroll_view->contentsWidth(),
      m_scroll_view->visibleHeight() - m_scroll_view->contentsHeight(),
      true
    );
  }
  m_mission_control->unlock();
}

