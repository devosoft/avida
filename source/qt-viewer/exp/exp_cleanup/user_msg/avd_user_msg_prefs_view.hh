#ifndef AVD_USER_MSG_PREFS_VIEW_HH
#define AVD_USER_MSG_PREFS_VIEW_HH

#ifndef AVD_PREFS_ITEM_VIEW_HH
#include "preferences/avd_prefs_item_view.hh"
#endif

class QListView;
class QPushButton;
class QSpinBox;
class avdUserMsgPrefsView : public avdPrefsItemView {
  Q_OBJECT
protected:
  QListView *m_list_view;
  QPushButton *m_font_btn;
  QSpinBox *m_spin_box;
public:
  explicit avdUserMsgPrefsView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  );
  QListView *listView(){ return m_list_view; }
  QPushButton *fontBtn(){ return m_font_btn; }
  QSpinBox *spinBox(){ return m_spin_box; }
};

#endif

// arch-tag: header file for user debug-message display preferences view
