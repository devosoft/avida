//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qapplication.h>
#include <qcolor.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qpainter.h>

//#include "gui_config.hh"

#include "n_orig_instruction_scrollview.hh"
#include "n_orig_instruction_scrollview_config.hh"
#include "n_orig_instruction_pixmapstructs.hh"


int
  N_Instruction_ScrollView::s_break_button_width,
  N_Instruction_ScrollView::s_break_button_height,
  N_Instruction_ScrollView::s_break_button_x,
  N_Instruction_ScrollView::s_line_number_label_width,
  N_Instruction_ScrollView::s_line_number_label_height,
  N_Instruction_ScrollView::s_line_number_label_x,
  N_Instruction_ScrollView::s_head_button_width,
  N_Instruction_ScrollView::s_head_button_height,
  N_Instruction_ScrollView::s_ihead_button_x,
  N_Instruction_ScrollView::s_rhead_button_x,
  N_Instruction_ScrollView::s_whead_button_x,
  N_Instruction_ScrollView::s_fhead_button_x,
  N_Instruction_ScrollView::s_flag_label_width,
  N_Instruction_ScrollView::s_flag_label_height,
  N_Instruction_ScrollView::s_cflag_label_x,
  N_Instruction_ScrollView::s_mflag_label_x,
  N_Instruction_ScrollView::s_eflag_label_x,
  N_Instruction_ScrollView::s_iflag_label_x,
  N_Instruction_ScrollView::s_instr_label_width,
  N_Instruction_ScrollView::s_instr_label_height,
  N_Instruction_ScrollView::s_instr_label_x,
  
  N_Instruction_ScrollView::s_field_spacing,
  N_Instruction_ScrollView::s_button_width,
  N_Instruction_ScrollView::s_button_height;

QColor
  N_Instruction_ScrollView::s_b_button_bg(192,192,192),
  N_Instruction_ScrollView::s_line_number_label_bg(224,224,224),
  N_Instruction_ScrollView::s_i_button_bg(192,192,192),
  N_Instruction_ScrollView::s_r_button_bg(224,224,224),
  N_Instruction_ScrollView::s_w_button_bg(192,192,192),
  N_Instruction_ScrollView::s_f_button_bg(224,224,224),
  N_Instruction_ScrollView::s_c_label_bg(192,192,192),
  N_Instruction_ScrollView::s_m_label_bg(224,224,224),
  N_Instruction_ScrollView::s_e_label_bg(192,192,192),
  N_Instruction_ScrollView::s_i_label_bg(224,224,224),
  N_Instruction_ScrollView::s_inst_field_bg(192,192,192),
  N_Instruction_ScrollView::s_ih_highlight_color(192,192,255),
  N_Instruction_ScrollView::s_rh_highlight_color(192,255,192),
  N_Instruction_ScrollView::s_wh_highlight_color(255,192,192),
  //N_Instruction_ScrollView::s_fh_highlight_color(240,240,240),
  N_Instruction_ScrollView::s_fh_highlight_color(176,176,176),
  N_Instruction_ScrollView::s_highlight_color(192,192,255);

PixmapOnOffSet
  N_Instruction_ScrollView::s_bp_pm_set,
  N_Instruction_ScrollView::s_ih_pm_set,
  N_Instruction_ScrollView::s_rh_pm_set,
  N_Instruction_ScrollView::s_wh_pm_set,
  N_Instruction_ScrollView::s_fh_pm_set,
  N_Instruction_ScrollView::s_cf_pm_set,
  N_Instruction_ScrollView::s_mf_pm_set,
  N_Instruction_ScrollView::s_ef_pm_set,
  N_Instruction_ScrollView::s_if_pm_set;

N_Instruction_ScrollView::InstructionPixmapMap
  N_Instruction_ScrollView::s_instr_pm_set;

void
N_Instruction_ScrollView::drawPrimitive(
  QPixmap *pixmap,
  int x, int y, int width, int height,
  const QColor &color,
  const QString &string, int text_flags
){
  QPainter p(pixmap);
  p.fillRect(
    x,
    y,
    width,
    height,
    color
  );
  p.drawText(
    x + (2 * s_field_spacing),
    y + (2 * s_field_spacing),
    width - (4 * s_field_spacing),
    height - (4 * s_field_spacing),
    text_flags,
    string
  );
}


void
N_Instruction_ScrollView::rebuildPixmapSet(
  PixmapOnOffSet &set,
  int x, int y, int width, int height,
  const QColor &hl_color, const QColor &uhl_color,
  const QString &on_string, const QString &off_string,
  int text_flags
){
  rebuildPixmapPair(
    set.on,
    x, y, width, height,
    hl_color, uhl_color,
    on_string,
    text_flags
  );
  rebuildPixmapPair(
    set.off,
    x, y, width, height,
    hl_color, uhl_color,
    off_string,
    text_flags
  );
}

void
N_Instruction_ScrollView::buildPixmapSet(
  PixmapOnOffSet &set,
  int x, int y, int width, int height,
  const QColor &hl_color, const QColor &uhl_color,
  const QString &on_string, const QString &off_string,
  int text_flags
){
  buildPixmapPair(
    set.on,
    x, y, width, height,
    hl_color, uhl_color,
    on_string,
    text_flags
  );
  buildPixmapPair(
    set.off,
    x, y, width, height,
    hl_color, uhl_color,
    off_string,
    text_flags
  );
}

void
N_Instruction_ScrollView::rebuildPixmapPair(
  PixmapHlSet &pair,
  int x, int y, int width, int height,
  const QColor &hl_color, const QColor &uhl_color,
  const QString &string,
  int text_flags
){
  /*
  FIXME:  kludge code.  -- K.
  */
  //assert(pair.ih_hl);
  //assert(pair.uhl);
  delete pair.ih_hl;
  delete pair.uhl;
  buildPixmapPair(
    pair,
    x, y, width, height,
    hl_color, uhl_color,
    string,
    text_flags
  );
}

void
N_Instruction_ScrollView::buildPixmapPair(
  PixmapHlSet &pair,
  int x, int y, int width, int height,
  const QColor &hl_color, const QColor &uhl_color,
  const QString &string,
  int text_flags
){
  pair.ih_hl = new QPixmap(width, height, -1, QPixmap::BestOptim);
  drawPrimitive(pair.ih_hl, x, y, width, height,
    hl_color,
    string,
    text_flags
  );

  pair.uhl = new QPixmap(width, height, -1, QPixmap::BestOptim);
  drawPrimitive(pair.uhl, x, y, width, height,
    uhl_color,
    string,
    text_flags
  );

  pair.buildPixmapSet(
    x, y, width, height, s_field_spacing,
    uhl_color,
    s_ih_highlight_color,
    s_rh_highlight_color,
    s_wh_highlight_color,
    s_fh_highlight_color,
    string,
    text_flags
  );
}

void
N_Instruction_ScrollView::N_Instruction_ScrollView_dynamicConfig(void){
  static bool configured = false;

  if(!configured){
    configured = true;

    int zwidth = QApplication::fontMetrics().width("0");
    int zheight = QApplication::fontMetrics().height();
    
    s_field_spacing = 1;

    int zspace = 4 * s_field_spacing;

    s_break_button_width = zspace + (6 * zwidth);
    s_break_button_height = zspace + zheight;
    s_break_button_x = 0;

    s_line_number_label_width = zspace + (5 * zwidth);
    s_line_number_label_height = zspace + zheight;
    s_line_number_label_x = s_break_button_x + s_break_button_width;

    s_head_button_width = zspace + (2 * zwidth);
    s_head_button_height = zspace + zheight;

    s_ihead_button_x = s_line_number_label_x + s_line_number_label_width;
    s_rhead_button_x = s_ihead_button_x + s_head_button_width;
    s_whead_button_x = s_rhead_button_x + s_head_button_width;
    s_fhead_button_x = s_whead_button_x + s_head_button_width;

    s_flag_label_width = zspace + (2 * zwidth);
    s_flag_label_height = zspace + zheight; 

    s_cflag_label_x = s_fhead_button_x + s_head_button_width;
    s_mflag_label_x = s_cflag_label_x + s_flag_label_width;
    s_eflag_label_x = s_mflag_label_x + s_flag_label_width;
    s_iflag_label_x = s_eflag_label_x + s_flag_label_width;

    s_instr_label_width = zspace + (20 * zwidth);
    s_instr_label_height = zspace + zheight;
    s_instr_label_x = s_iflag_label_x + s_flag_label_width;

    s_button_width =
      s_break_button_width +
      s_line_number_label_width +
      (4 * s_head_button_width) +
      (4 * s_flag_label_width) +
      s_instr_label_width;
    s_button_height = s_instr_label_height;

    buildPixmapSet(
      s_bp_pm_set, 0, 0, s_break_button_width, s_break_button_height,
      s_highlight_color, s_b_button_bg,
      "b", "",
      AlignCenter
    );
    buildPixmapSet(
      s_ih_pm_set, 0, 0, s_head_button_width, s_head_button_height,
      s_highlight_color, s_i_button_bg,
      "I", "",
      AlignCenter
    );
    buildPixmapSet(
      s_rh_pm_set, 0, 0, s_head_button_width, s_head_button_height,
      s_highlight_color, s_r_button_bg,
      "R", "",
      AlignCenter
    );
    buildPixmapSet(
      s_wh_pm_set, 0, 0, s_head_button_width, s_head_button_height,
      s_highlight_color, s_w_button_bg,
      "W", "",
      AlignCenter
    );
    buildPixmapSet(
      s_fh_pm_set, 0, 0, s_head_button_width, s_head_button_height,
      s_highlight_color, s_f_button_bg,
      "F", "",
      AlignCenter
    );
    buildPixmapSet(
      s_cf_pm_set, 0, 0, s_flag_label_width, s_flag_label_height,
      s_highlight_color, s_c_label_bg,
      "c", "",
      AlignCenter
    );
    buildPixmapSet(
      s_mf_pm_set, 0, 0, s_flag_label_width, s_flag_label_height,
      s_highlight_color, s_m_label_bg,
      "m", "",
      AlignCenter
    );
    buildPixmapSet(
      s_ef_pm_set, 0, 0, s_flag_label_width, s_flag_label_height,
      s_highlight_color, s_e_label_bg,
      "e", "",
      AlignCenter
    );
    buildPixmapSet(
      s_if_pm_set, 0, 0, s_flag_label_width, s_flag_label_height,
      s_highlight_color, s_i_label_bg,
      "i", "",
      AlignCenter
    );
  }
}
