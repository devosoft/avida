//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_VIEW_WIDGET_HH
#define N_ORIG_INSTRUCTION_VIEW_WIDGET_HH

#include <qguardedptr.h>
#include <qptrvector.h>

#include <qwidget.h>


class QVBoxLayout;
class QHBoxLayout;
//class QScrollView;
class N_Instruction_ScrollView;
class QSpacerItem;
class QLabel;
class QPopupMenu;

class avd_MissionControl;
class N_Instruction_LineWidget;

class
N_Instruction_ViewWidget : public QWidget {
  Q_OBJECT
protected:
  QGuardedPtr<avd_MissionControl> m_mission_control;
  int m_cell_id;
  bool m_buttons_safe;

  QVBoxLayout *m_vboxlayout;
  ; QWidget *m_header;
  ; QHBoxLayout *m_header_layout;
  ; ; QLabel *m_break_label;
  ; ; QLabel *m_lineno_label;
  ; ; QLabel *m_head_label;
  ; ; QLabel *m_flag_label;
  ; ; QLabel *m_instr_label;

  ; N_Instruction_ScrollView *m_scroll_view;
  ; ; QPtrVector<N_Instruction_LineWidget> m_instr_lines_list;

  ; QLabel *m_message_label;

  QGuardedPtr<QPopupMenu> m_instr_modifier_menu;
  ; QGuardedPtr<QPopupMenu> m_instr_change_menu;
  ; QGuardedPtr<QPopupMenu> m_instr_insert_menu;

  enum InstMenuItems {
    NONE, CHANGE, REMOVE, INSERT
  };
  InstMenuItems m_activated_menu;
  int m_menu_lineno;
private:
  void checkInstrLinesListSize(int size);
  void updateViewer(bool force_redraw = false, bool ensure_visible = true);
  void updateInstrSubmenu(QPopupMenu *submenu);
public:
  N_Instruction_ViewWidget(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = Qt::WDestructiveClose
  );
  ~N_Instruction_ViewWidget(void);
  void setMissionControl(avd_MissionControl *mission_control);
  void buttonsSafe(bool safe);
signals:
  void enableButtonsSig(bool buttons_enabled);
public slots:
  void setPopulationCell(int cell_id);
  void updateState(void);
private slots:
  void lineButtonSlot(int id, int lineno);
  void lineInstSlot(const QPoint &point, int lineno);
  void instChangeMenuItemActivated(int id);
  void instRemoveMenuItemActivated(void);
  void instInsertMenuItemActivated(int id);
  void instChangeMenuAboutToShow(void);
  void instInsertMenuAboutToShow(void);
};

#endif /* !N_ORIG_INSTRUCTION_VIEW_WIDGET_HH */
