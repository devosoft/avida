//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_SCROLLVIEW_HH
#define N_ORIG_INSTRUCTION_SCROLLVIEW_HH

#include <qscrollview.h>
#include <qptrvector.h>
#include <qguardedptr.h>
#include <qmap.h>

#include "defs.hh"

#include "n_orig_instruction_scrollview_config.hh"
#include "n_orig_instruction_pixmapstructs.hh"

class N_Instruction_LineWidget;
class QPopupMenu;
class cInstruction;

class N_Instruction_ScrollView : public QScrollView {
  Q_OBJECT
public:
  static int
  s_break_button_width,
  s_break_button_height,
  s_break_button_x,
  s_line_number_label_width,
  s_line_number_label_height,
  s_line_number_label_x,
  s_head_button_width,
  s_head_button_height,
  s_ihead_button_x,
  s_rhead_button_x,
  s_whead_button_x,
  s_fhead_button_x,
  s_flag_label_width,
  s_flag_label_height,
  s_cflag_label_x,
  s_mflag_label_x,
  s_eflag_label_x,
  s_iflag_label_x,
  s_instr_label_width,
  s_instr_label_height,
  s_instr_label_x,
  
  s_field_spacing, s_button_width, s_button_height;
  
  static QColor
  s_b_button_bg,
  s_line_number_label_bg,
  s_i_button_bg,
  s_r_button_bg, 
  s_w_button_bg, 
  s_f_button_bg,
  s_c_label_bg,
  s_m_label_bg,
  s_e_label_bg,
  s_i_label_bg,
  s_inst_field_bg,
  s_ih_highlight_color,
  s_rh_highlight_color,
  s_wh_highlight_color,
  s_fh_highlight_color,
  s_highlight_color,
  s_highlight_color_set[PixmapHlSet::COUNT_HL];

  static void N_Instruction_ScrollView_dynamicConfig(void);

  enum ButtonEnum {
    BUTTON_NONE,
    BUTTON_BP,
    BUTTON_IH,
    BUTTON_RH,
    BUTTON_WH,
    BUTTON_FH,
    BUTTON_INST,
    BUTTON_ENUM_COUNT
  };
private:
  /*
  struct PixmapHlSet {
    QPixmap *hl, *uhl;
  };
  struct PixmapOnOffSet {
    PixmapHlSet on, off;
  };
  */
  static PixmapOnOffSet
    s_bp_pm_set,
    s_ih_pm_set,
    s_rh_pm_set,
    s_wh_pm_set,
    s_fh_pm_set,
    s_cf_pm_set,
    s_mf_pm_set,
    s_ef_pm_set,
    s_if_pm_set;
  //typedef QMap<cInstruction, PixmapHlSet> InstructionPixmapMap;
  typedef QMap<UCHAR, PixmapHlSet> InstructionPixmapMap;
  static InstructionPixmapMap s_instr_pm_set;
  QPtrVector<N_Instruction_LineWidget> m_instr_lines_list; 

  bool m_buttons_enabled;
  int m_pressed_mouse_in_line;
  int m_pressed_mouse_in_button;
  int m_last_mouse_in_line;
  int m_last_mouse_in_button;
  QGuardedPtr<QPopupMenu> m_instr_modifier_menu;
  QGuardedPtr<QPopupMenu> m_instr_change_menu;
  QGuardedPtr<QPopupMenu> m_instr_insert_menu;
public:
  N_Instruction_ScrollView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = WStaticContents | WRepaintNoErase
  );
  ~N_Instruction_ScrollView(void);
  virtual void addChild(N_Instruction_LineWidget *child, int x=0, int y=0);
public slots:
  void setButtonsEnabled(bool buttons_enabled){
    m_buttons_enabled = buttons_enabled;
  }
//  virtual void resizeContents(int w, int h);
signals:
  void buttonSig(int id, int lineno);
  void instructionSig(const QPoint &point, int lineno);
protected:
  virtual void drawContents(
    QPainter *p,
    int clipx,
    int clipy,
    int clipw,
    int cliph 
  );
  virtual void contentsMousePressEvent(QMouseEvent *event);
  virtual void contentsMouseReleaseEvent(QMouseEvent *event);
  virtual void contentsMouseMoveEvent(QMouseEvent *event);
  virtual void leaveEvent(QEvent *);
  static int locateButton(int x);
  void drawButtonFrame(
    int mouse_in_button,
    int mouse_in_line,
    bool raised = true
  );
  void undrawLastButtonFrame(void);
  static void drawPrimitive(
    QPixmap *pixmap,
    int x, int y, int width, int height,
    const QColor &color,
    const QString &string, int text_flags
  );
public:
  static void buildPixmapPair(
    PixmapHlSet &pair,
    int x, int y, int width, int height,
    const QColor &hl_color, const QColor &uhl_color,
    const QString &string,
    int text_flags
  );
  static void rebuildPixmapPair(
    PixmapHlSet &pair,
    int x, int y, int width, int height,
    const QColor &hl_color, const QColor &uhl_color,
    const QString &string,
    int text_flags
  );
  static void buildPixmapSet(
    PixmapOnOffSet &set,
    int x, int y, int width, int height,
    const QColor &hl_color, const QColor &uhl_color,
    const QString &on_string, const QString &off_string,
    int text_flags
  );
  static void rebuildPixmapSet(
    PixmapOnOffSet &set,
    int x, int y, int width, int height,
    const QColor &hl_color, const QColor &uhl_color,
    const QString &on_string, const QString &off_string,
    int text_flags
  );
  void drawLine(N_Instruction_LineWidget *line);
};

#endif /* !N_ORIG_INSTRUCTION_SCROLLVIEW_HH */
