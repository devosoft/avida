//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_LINE_WIDGET_HH
#define N_ORIG_INSTRUCTION_LINE_WIDGET_HH

#include <qguardedptr.h>
#include <qwidget.h>

#include "n_orig_instruction_pixmapstructs.hh"

#include "instruction.hh"


class QLabel;
class QToolButton;
class N_Instruction_FieldWidget;
class cCPUMemory;
class cInstSet;

class
N_Instruction_LineWidget : public QWidget {
  Q_OBJECT
public:
  int m_line_number;
  int m_x, m_y;
  bool m_last_i, m_last_r, m_last_w, m_last_f;
  bool m_clear_i, m_clear_r, m_clear_w, m_clear_f;
  bool m_last_c, m_last_m, m_last_e, m_last_if, m_last_b;
  bool
    m_bf_is_dirty, m_ln_is_dirty, m_ih_is_dirty, m_rh_is_dirty,
    m_wh_is_dirty, m_fh_is_dirty, m_cf_is_dirty, m_mf_is_dirty,
    m_ef_is_dirty, m_if_is_dirty, m_il_is_dirty;
  bool m_is_dirty;
  bool m_is_highlit;
  bool m_pixmap_is_initialized;

  PixmapHlSet::HighlightEnum m_highlighting_index;

  cInstruction m_last_instruction;
  QString m_last_instruction_text;

  QPixmap *m_pixmap;
  PixmapHlSet m_lineno_pixmaps;

public:
  N_Instruction_LineWidget(
    int line_number,
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  ~N_Instruction_LineWidget(void);
  void resizePixmap(int w, int h);
  void setPixmapOptimization(int optimization);
  void setX(int x){ m_x = x;}
  void setY(int y){ m_y = y;}
  void setInstructionLine(
    const cCPUMemory &memory,
    const cInstSet &inst_set,
    bool force_redraw = false
  );
  void setIHeadButton(void);
  void setRHeadButton(void);
  void setWHeadButton(void);
  void setFHeadButton(void);
  void clearHeadButtons(void);
};

#endif /* !N_ORIG_INSTRUCTION_LINE_WIDGET_HH */

