//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qpainter.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qpopmenu.h>
#include <qapplication.h>
#include <qcommonstyle.h>

#include "inst_set.hh"

#include "n_orig_instruction_line_widget.hh"

#include "n_orig_instruction_scrollview.hh"

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


N_Instruction_ScrollView::N_Instruction_ScrollView(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QScrollView(parent, name, f)
{
  viewport()->setEraseColor(s_inst_field_bg);

  GenDebug
  << " --- Kaben Was Here.";

  m_pressed_mouse_in_line = -1;
  m_pressed_mouse_in_button = BUTTON_NONE;
  m_last_mouse_in_line = -1;
  m_last_mouse_in_button = BUTTON_NONE;
}

N_Instruction_ScrollView::~N_Instruction_ScrollView(void){
  GenDebug
  << "<N_Instruction_ScrollView::~N_Instruction_ScrollView>\n"
  << " --- destructor called.\n";
}

void
N_Instruction_ScrollView::addChild(
  N_Instruction_LineWidget *child,
  int x,
  int y
){
  int list_size = m_instr_lines_list.count();
  m_instr_lines_list.resize(list_size + 1);
  m_instr_lines_list.insert(list_size, child);

  GenDebug
  << " --- no longer adding to parent QScrollView, just tracking.\n"
  << " --- child number " << list_size << ".";
}

void
N_Instruction_ScrollView::drawContents(
  QPainter *p,
  int clipx,
  int clipy,
  int clipw,
  int cliph
){

  GenDebug
  << " --- clipx " << clipx << ",\n"
  << " --- clipy " << clipy << ",\n"
  << " --- clipw " << clipw << ",\n"
  << " --- cliph " << cliph << ".";

  GenDebug
  << " ...\n"
  << " --- there are " << m_instr_lines_list.count() << " lines.";

  int toprow = clipy / s_button_height;
  int bottomrow =
    (clipy + cliph + s_button_height - 1)
    /
    s_button_height;
  int first_invisible_row =
    contentsHeight()
    / 
    s_button_height;
  
  if(m_instr_lines_list.count() < first_invisible_row){
    first_invisible_row = m_instr_lines_list.count();
  }
  if((bottomrow + 1) < first_invisible_row){
    first_invisible_row = (bottomrow + 1);
  }

  int r = toprow;
  while(r < first_invisible_row){
    if(m_instr_lines_list[r]->m_is_dirty){
      m_instr_lines_list[r]->m_is_dirty = false;
      drawLine(m_instr_lines_list[r]);
    }
    p->drawPixmap(
      m_instr_lines_list[r]->m_x,
      m_instr_lines_list[r]->m_y,
      *m_instr_lines_list[r]->m_pixmap
    );
    r++;
  }
  p->fillRect(
    0,
    r * s_button_height,
    s_button_width,
    (bottomrow - r + 1) * s_button_height,
    s_inst_field_bg
  );

  GenDebug << " --- returning.\n";
}



/*
Handling of mouse events...
*/

void
N_Instruction_ScrollView::contentsMousePressEvent(QMouseEvent *event){
  GenDebug
  << " --- x() " << event->x() << ", y() " << event->y() << ".\n"
  << " --- corresponds to line "
  << (event->y() / s_button_height) << ".";

  if(m_buttons_enabled){
    int mouse_in_line = event->y() / s_button_height;
    int mouse_in_button = locateButton(event->x());
  
    if( // if mouse_in_line corresponds to a visible line, draw button.
      mouse_in_line >= 0
      &&
      mouse_in_line * s_button_height < contentsHeight()
    ){
      m_pressed_mouse_in_line = mouse_in_line;
      m_pressed_mouse_in_button = mouse_in_button;
      drawButtonFrame(mouse_in_button, mouse_in_line, false);
    }
  }

  GenDebug << "done.\n";
}

void
N_Instruction_ScrollView::contentsMouseReleaseEvent(QMouseEvent *event){
  GenDebug
  << " --- x() " << event->x() << ", y() " << event->y() << ".\n"
  << " --- corresponds to line "
  << (event->y() / s_button_height) << ".";

  int mouse_in_line = event->y() / s_button_height;
  int mouse_in_button = locateButton(event->x());
  if(
    mouse_in_line == m_pressed_mouse_in_line
    &&
    mouse_in_button == m_pressed_mouse_in_button
    &&
    m_buttons_enabled
  ){
    switch((ButtonEnum)mouse_in_button){
    case BUTTON_NONE:
      break;
    case BUTTON_BP:
    case BUTTON_IH:
    case BUTTON_RH:
    case BUTTON_WH:
    case BUTTON_FH:
      emit buttonSig(mouse_in_button, mouse_in_line);
      break;
    case BUTTON_INST:
      emit instructionSig(
        mapToGlobal((QPoint(
          s_instr_label_x,
          (1 + mouse_in_line) * s_button_height
            - contentsY()
        ))), 
        mouse_in_line
      );
      break;
    default:
      break;
    }
  }

  /*
  GenDebug
  << "<N_Instruction_ScrollView::contentsMouseReleaseEvent>\n"
  << " --- mouse not released in a clicked button.";
  */
  
  m_pressed_mouse_in_line = -1;
  m_pressed_mouse_in_button = BUTTON_NONE;

  if( // if a button is currently drawn,
    m_last_mouse_in_line != -1
    &&
    ( // and if we're no longer in that button,
      mouse_in_line != m_last_mouse_in_line
      ||
      mouse_in_button != m_last_mouse_in_button
    )
  ){ // then undraw the currently drawn button.
    undrawLastButtonFrame();
  }

  if( // if mouse_in_line corresponds to a visible line, draw button.
    mouse_in_line >= 0
    &&
    mouse_in_line * s_button_height < contentsHeight()
    &&
    m_buttons_enabled
  ) drawButtonFrame(mouse_in_button, mouse_in_line, true);

  GenDebug << "done.\n";
}

void
N_Instruction_ScrollView::contentsMouseMoveEvent(QMouseEvent *event){
  GenDebug
  << " --- x() " << event->x() << ", y() " << event->y() << ".\n"
  << " --- corresponds to line "
  << (event->y() / s_button_height) << ".";

  int mouse_in_line = event->y() / s_button_height;
  int mouse_in_button = locateButton(event->x());

  /*
  HEY:  I'm sure the following logic could be compressed if anyone
  feels like doing it.  -- K.
  */
  if( // if a button is currently drawn,
    m_last_mouse_in_line != -1
    &&
    ( // and if we're no longer in that button,
      mouse_in_line != m_last_mouse_in_line
      ||
      mouse_in_button != m_last_mouse_in_button
    )
  ){ // then undraw the currently drawn button.
    undrawLastButtonFrame();
  }

  if(m_buttons_enabled){
    if( // first verify that mouse_in_line corresponds to a visible line.
      mouse_in_line >= 0
      &&
      mouse_in_line * s_button_height < contentsHeight()
      &&
      ( // and that we're in a new button
        mouse_in_line != m_last_mouse_in_line
        ||
        mouse_in_button != m_last_mouse_in_button
      )
    ){ // line is visible.
      if(m_pressed_mouse_in_line == -1)
      { // mouse button is not down, so track buttons.
        drawButtonFrame(mouse_in_button, mouse_in_line, true);
      } else { // mouse button is still down
        if(
          mouse_in_line == m_pressed_mouse_in_line
          &&
          mouse_in_button == m_pressed_mouse_in_button
        ){ // we're in the clicked button, which may need redrawing.
          drawButtonFrame(mouse_in_button, mouse_in_line, false);
        }
        // otherwise, if we're not in the clicked button, we don't draw a
        // new one.  that is, until the mouse button is released, we only
        // draw the clicked button.
      }
    }
  }

  GenDebug << "done.\n";
}

void
N_Instruction_ScrollView::leaveEvent(QEvent *e){
  GenDebug << "entered.\n";


  if(m_pressed_mouse_in_line == -1){
    undrawLastButtonFrame(); 
  }

  GenDebug << "done.\n";
}


int
N_Instruction_ScrollView::locateButton(int x){
  ButtonEnum located_button = BUTTON_NONE;

  if(
    s_break_button_x <= x
    && 
    x < (s_break_button_x + s_break_button_width)
  ) located_button = BUTTON_BP;
  else if(
    s_ihead_button_x <= x
    &&
    x < (s_ihead_button_x + s_head_button_width)
  ) located_button = BUTTON_IH;
  else if(
    s_rhead_button_x <= x
    &&
    x < (s_rhead_button_x + s_head_button_width)
  ) located_button = BUTTON_RH;
  else if(
    s_whead_button_x <= x
    &&
    x < (s_whead_button_x + s_head_button_width)
  ) located_button = BUTTON_WH;
  else if(
    s_fhead_button_x <= x
    &&
    x < (s_fhead_button_x + s_head_button_width)
  ) located_button = BUTTON_FH;
  else if(
    s_instr_label_x <= x
    &&
    x < (s_instr_label_x + s_instr_label_width)
  ) located_button = BUTTON_INST;
  
  GenDebug << "x " << x << ", located_button " << located_button << ".";
    
  return located_button;
}   

void
N_Instruction_ScrollView::drawButtonFrame(
  int mouse_in_button,
  int mouse_in_line,
  bool raised
){

  QPainter p(viewport());
  int cx, vx, vy, w, h;

  GenDebug
  << " --- mouse_in_line " << mouse_in_line << "\n"
  << " --- mouse_in_button " << mouse_in_button << '.';

  switch((ButtonEnum)mouse_in_button){
  case BUTTON_NONE:
    m_last_mouse_in_line = -1;
    m_last_mouse_in_button = BUTTON_NONE;
    return;
    break; // not reached
  case BUTTON_BP:
    cx = s_break_button_x;
    w = s_break_button_width;
    h = s_break_button_height;
    break;
  case BUTTON_IH:
    cx = s_ihead_button_x;
    w = s_head_button_width;
    h = s_head_button_height;
    break;
  case BUTTON_RH:
    cx = s_rhead_button_x;
    w = s_head_button_width;
    h = s_head_button_height;
    break;
  case BUTTON_WH:
    cx = s_whead_button_x;
    w = s_head_button_width;
    h = s_head_button_height;
    break;
  case BUTTON_FH:
    cx = s_fhead_button_x;
    w = s_head_button_width;
    h = s_head_button_height;
    break;
  case BUTTON_INST:
    cx = s_instr_label_x;
    w = s_instr_label_width;
    h = s_instr_label_height;
    break;
  default:
    return;
    break; // not reached.
  }

  contentsToViewport(
    cx,
    mouse_in_line * s_button_height,
    vx,
    vy
  );
  QApplication::style().drawPrimitive(
    QStyle::PE_Panel,
    &p,
    QRect(vx,vy,w,h),
    QApplication::palette().active(),
    (raised)?(QStyle::Style_Raised):(QStyle::Style_Sunken),
    QStyleOption::QStyleOption(s_field_spacing, 0)
  );
  m_last_mouse_in_line = mouse_in_line;
  m_last_mouse_in_button = mouse_in_button;
}


void
N_Instruction_ScrollView::undrawLastButtonFrame(void){
  QPainter p(viewport());
  int cx, vx, vy, w, h;
  QColor standard_bg = s_inst_field_bg;

  if(
    m_last_mouse_in_line >= 0
    &&
    (m_last_mouse_in_line * s_button_height) < contentsHeight()
  ){
    switch((ButtonEnum)m_last_mouse_in_button){
    case BUTTON_NONE:
      return;
      break; // not reached
    case BUTTON_BP:
      cx = s_break_button_x;
      standard_bg = s_b_button_bg;
      w = s_break_button_width;
      h = s_break_button_height;
      break;
    case BUTTON_IH:
      cx = s_ihead_button_x;
      standard_bg = s_i_button_bg;
      w = s_head_button_width;
      h = s_head_button_height;
      break;
    case BUTTON_RH:
      cx = s_rhead_button_x;
      standard_bg = s_r_button_bg;
      w = s_head_button_width;
      h = s_head_button_height;
      break;
    case BUTTON_WH:
      cx = s_whead_button_x;
      standard_bg = s_w_button_bg;
      w = s_head_button_width;
      h = s_head_button_height;
      break;
    case BUTTON_FH:
      cx = s_fhead_button_x;
      standard_bg = s_f_button_bg;
      w = s_head_button_width;
      h = s_head_button_height;
      break;
    case BUTTON_INST:
      cx = s_instr_label_x;
      standard_bg = s_inst_field_bg;
      w = s_instr_label_width;
      h = s_instr_label_height;
      break;
    default:
      return;
      break; // not reached.
    }

    contentsToViewport(
      cx,
      m_last_mouse_in_line * s_button_height,
      vx,
      vy
    );
    /*
    p.setPen(
      QPen(
        (m_instr_lines_list[m_last_mouse_in_line]->m_is_highlit)?
          (highlight_bg):(standard_bg),
        s_field_spacing
      )
    );
    */
    QColor highlight_bg(s_highlight_color);
    QColor background_color;
    switch(
      (PixmapHlSet::HighlightEnum)
      m_instr_lines_list[m_last_mouse_in_line]->m_highlighting_index
    ){
    case PixmapHlSet::NO_HL:
      background_color = standard_bg;
      break;
    case PixmapHlSet::IH_HL:
      background_color = s_ih_highlight_color;
      break;
    case PixmapHlSet::RH_HL:
      background_color = s_rh_highlight_color;
      break;
    case PixmapHlSet::WH_HL:
      background_color = s_wh_highlight_color;
      break;
    case PixmapHlSet::FH_HL:
      background_color = s_fh_highlight_color;
      break;
    default:
      GenFatal("bad m_highlighting_index.");
      break;  // not reached.
    }
    p.setPen(
      QPen(background_color, s_field_spacing)
    );
    p.drawRect(QRect(vx,vy,w,h));
    m_last_mouse_in_line = -1;
    m_last_mouse_in_button = BUTTON_NONE;
  }
}

void
N_Instruction_ScrollView::drawLine(N_Instruction_LineWidget *line){
  if(!line->m_pixmap_is_initialized){
    line->m_pixmap_is_initialized = true;
    line->m_x = 0;
    line->m_y = line->m_line_number * s_button_height;
    line->m_pixmap->resize(s_button_width, s_button_height);
    line->m_pixmap->setOptimization(QPixmap::BestOptim);
    buildPixmapPair(
      line->m_lineno_pixmaps,
      0, 0, s_line_number_label_width, s_line_number_label_height,
      s_highlight_color,
      s_line_number_label_bg,
      QString("%1").arg(1 +  line->m_line_number),
      AlignRight
    );
  }
  QPainter p(line->m_pixmap);

  int highlighting_index = line->m_highlighting_index;
  /*
  drawBButton
  */
  if(line->m_bf_is_dirty){
    line->m_bf_is_dirty = false;
    p.drawPixmap(
      s_break_button_x, 0,
      (line->m_last_b)
      ?(*s_bp_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_bp_pm_set.off.m_pixmaps[highlighting_index])
    );
  }

  /*
  drawLField
  */
  if(line->m_ln_is_dirty){
    line->m_ln_is_dirty = false;
    p.drawPixmap(
      s_line_number_label_x, 0,
      *line->m_lineno_pixmaps.m_pixmaps[highlighting_index]
    );
  }
  /*
  drawIButton
  */
  if(line->m_ih_is_dirty){
    line->m_ih_is_dirty = false;
    p.drawPixmap(
      s_ihead_button_x, 0,
      (line->m_last_i)
      ?(*s_ih_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_ih_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawRButton
  */
  if(line->m_rh_is_dirty){
    line->m_rh_is_dirty = false;
    p.drawPixmap(
      s_rhead_button_x, 0,
      (line->m_last_r)
      ?(*s_rh_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_rh_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawWButton
  */
  if(line->m_wh_is_dirty){
    line->m_wh_is_dirty = false;
    p.drawPixmap(
      s_whead_button_x, 0,
      (line->m_last_w)
      ?(*s_wh_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_wh_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawFButton
  */
  if(line->m_fh_is_dirty){
    line->m_fh_is_dirty = false;
    p.drawPixmap(
      s_fhead_button_x, 0,
      (line->m_last_f)
      ?(*s_fh_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_fh_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawCLabel
  */
  if(line->m_cf_is_dirty){
    line->m_cf_is_dirty = false;
    p.drawPixmap(
      s_cflag_label_x, 0,
      (line->m_last_c)
      ?(*s_cf_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_cf_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawMLabel
  */
  if(line->m_mf_is_dirty){
    line->m_mf_is_dirty = false;
    p.drawPixmap(
      s_mflag_label_x, 0,
      (line->m_last_m)
      ?(*s_mf_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_mf_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawELabel
  */
  if(line->m_ef_is_dirty){
    line->m_ef_is_dirty = false;
    p.drawPixmap(
      s_eflag_label_x, 0,
      (line->m_last_e)
      ?(*s_ef_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_ef_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawIFLabel
  */
  if(line->m_if_is_dirty){
    line->m_if_is_dirty = false;
    p.drawPixmap(
      s_iflag_label_x, 0,
      (line->m_last_if)
      ?(*s_if_pm_set.on.m_pixmaps[highlighting_index])
      :(*s_if_pm_set.off.m_pixmaps[highlighting_index])
    );
  }
  /*
  drawIField
  */
  if(line->m_il_is_dirty){
    line->m_il_is_dirty = false;
    InstructionPixmapMap::Iterator it
      = s_instr_pm_set.find(line->m_last_instruction.GetOp());
    if(it == s_instr_pm_set.end()){
      buildPixmapPair(
        s_instr_pm_set[line->m_last_instruction.GetOp()],
        0, 0, s_instr_label_width, s_instr_label_height,
        s_highlight_color, s_inst_field_bg,
        //QString(inst_set.GetName(m_last_instruction)()),
        line->m_last_instruction_text,
        AlignLeft
      );
      it = s_instr_pm_set.find(line->m_last_instruction.GetOp());
    }
    p.drawPixmap(
      s_instr_label_x, 0,
      *it.data().m_pixmaps[highlighting_index]
    );
  }
}

