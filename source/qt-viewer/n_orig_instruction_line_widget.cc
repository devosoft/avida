//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qapplication.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include "n_orig_instruction_field_widget.hh"

#include "inst_set.hh"
#include "hardware_cpu.hh"

#include "n_orig_instruction_line_widget.hh"
#include "n_orig_instruction_scrollview.hh"


using namespace std;


N_Instruction_LineWidget::N_Instruction_LineWidget(
  int line_number,
  QWidget *parent,
  const char *name,
  WFlags f
)
: QWidget(parent, name, f)
{
  m_line_number = line_number;

  m_pixmap = new QPixmap();

  m_pixmap_is_initialized = false;
  m_highlighting_index = PixmapHlSet::NO_HL;

  m_last_i = m_last_r = m_last_w = m_last_f = m_last_c = m_last_m =
  m_last_e = m_last_b = m_last_if = false;

  m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
  m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
  m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;

  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_LineWidget::N_Instruction_LineWidget>\n"
  << " --- m_line_number " << m_line_number << ", "
  << "m_y " << m_y << ".\n";
  #endif
}

N_Instruction_LineWidget::~N_Instruction_LineWidget(void){
  #if LOCAL_DEBUG
  cout
  << "<N_Instruction_LineWidget::~N_Instruction_LineWidget>\n"
  << " --- destructor called.\n";
  #endif

  delete m_pixmap;

  #if LOCAL_DEBUG
  cout << " --- deleted pixmap.\n";
  #endif
}

void
N_Instruction_LineWidget::resizePixmap(int w, int h){
  m_pixmap->resize(w, h);
  m_is_dirty = true;
}

void
N_Instruction_LineWidget::setPixmapOptimization(
  int optimization
){
  m_pixmap->setOptimization((QPixmap::Optimization) optimization);
  m_is_dirty = true;
}

void
N_Instruction_LineWidget::setInstructionLine(
  const cCPUMemory &memory,
  const cInstSet &inst_set,
  bool force_redraw
){
  if(force_redraw){
    // force unhighlight.
    // (we'll figure out which line to highlight later.)
    m_is_highlit = false;
    m_highlighting_index = PixmapHlSet::NO_HL;

    // mark everything as dirty.
    m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
    m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
    m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;

    // should we draw a 'breakpoint' flag?
    m_last_b = memory.FlagBreakpoint(m_line_number);
    // clear marked head positions 
    /*
    m_last_i = m_clear_i = false;
    m_last_r = m_clear_r = false;
    m_last_w = m_clear_w = false;
    m_last_f = m_clear_f = false;
    */
    m_last_i = false;
    m_last_r = false;
    m_last_w = false;
    m_last_f = false;
    m_clear_i = true;
    m_clear_r = true;
    m_clear_w = true;
    m_clear_f = true;
    // should we draw a 'copied' flag?
    m_last_c = memory.FlagCopied(m_line_number);
    // should we draw a 'mutated' flag?
    m_last_m = memory.FlagMutated(m_line_number);
    // should we draw an 'executed' flag?
    m_last_e = memory.FlagExecuted(m_line_number);
    // should we draw an 'injected' flag?
    m_last_if = memory.FlagInjected(m_line_number);
    // set instruction id and text.
    m_last_instruction = memory[m_line_number];
    m_last_instruction_text = QString(inst_set.GetName(m_last_instruction)());
  } else {
    // unmark head positions, and note that we need 
    if(m_last_i){ m_clear_i = true; }
    if(m_last_r){ m_clear_r = true; }
    if(m_last_w){ m_clear_w = true; }
    if(m_last_f){ m_clear_f = true; }
    if(memory.FlagCopied(m_line_number) != m_last_c){
      m_last_c = !m_last_c;
      m_cf_is_dirty = m_is_dirty = true;
    }
    if(memory.FlagMutated(m_line_number) != m_last_m){
      m_last_m = !m_last_m;
      m_mf_is_dirty = m_is_dirty = true;
    }
    if(memory.FlagExecuted(m_line_number) != m_last_e){
      m_last_e = !m_last_e;
      m_ef_is_dirty = m_is_dirty = true;
    }
    if(memory.FlagInjected(m_line_number) != m_last_if){
      m_last_if = !m_last_if;
      m_if_is_dirty = m_if_is_dirty = true;
    }
    if(memory.FlagBreakpoint(m_line_number) != m_last_b){
      m_last_b = !m_last_b;
      m_bf_is_dirty = m_is_dirty = true;
    }
    if(memory[m_line_number] != m_last_instruction){
      m_last_instruction = memory[m_line_number];
      m_last_instruction_text = QString(inst_set.GetName(m_last_instruction)());
      m_il_is_dirty = m_is_dirty = true;
    }
  }
}

void
N_Instruction_LineWidget::setIHeadButton(void){
  m_clear_i = false;
  m_last_i = m_is_highlit = true;
  m_highlighting_index = PixmapHlSet::IH_HL;
  m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
  m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
  m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
}

void
N_Instruction_LineWidget::setRHeadButton(void){
  m_clear_r = false;
  m_last_r = true;
  //m_rh_is_dirty = m_is_dirty = true;
  m_highlighting_index = PixmapHlSet::RH_HL;
  m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
  m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
  m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
}

void
N_Instruction_LineWidget::setWHeadButton(void){
  m_clear_w = false;
  m_last_w = true;
  //m_wh_is_dirty = m_is_dirty = true;
  m_highlighting_index = PixmapHlSet::WH_HL;
  m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
  m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
  m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
}

void
N_Instruction_LineWidget::setFHeadButton(void){
  m_clear_f = false;
  m_last_f = true;
  //m_fh_is_dirty = m_is_dirty = true;
  m_highlighting_index = PixmapHlSet::FH_HL;
  m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
  m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
  m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
}

void
N_Instruction_LineWidget::clearHeadButtons(void){
  if (m_clear_i){
    m_last_i = m_is_highlit = false;
    m_highlighting_index = PixmapHlSet::NO_HL;
    m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
    m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
    m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
  }
  if (m_clear_r){
    m_last_r = false;
    //m_rh_is_dirty = m_is_dirty = true;
    m_highlighting_index = PixmapHlSet::NO_HL;
    m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
    m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
    m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
  }
  if (m_clear_w){
    m_last_w = false;
    //m_wh_is_dirty = m_is_dirty = true;
    m_highlighting_index = PixmapHlSet::NO_HL;
    m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
    m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
    m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
  }
  if (m_clear_f){
    m_last_f = false;
    //m_fh_is_dirty = m_is_dirty = true;
    m_highlighting_index = PixmapHlSet::NO_HL;
    m_bf_is_dirty = m_ln_is_dirty = m_ih_is_dirty = m_rh_is_dirty =
    m_wh_is_dirty = m_fh_is_dirty = m_cf_is_dirty = m_mf_is_dirty =
    m_ef_is_dirty = m_if_is_dirty = m_il_is_dirty = m_is_dirty = true;
  }
}
