#ifndef AVD_PREFS_ITEM_PROTO_HH
#define AVD_PREFS_ITEM_PROTO_HH

class avdAbstractPrefsCtrl;
class avdPrefsItemView;
class avdPrefsItem
: public QListBoxText
, public cRetainable
{
protected:
  avdAbstractPrefsCtrl *m_prefs_gui;
public:
  void setPrefsGUI(avdAbstractPrefsCtrl *prefs_gui);
  avdAbstractPrefsCtrl *getPrefsGUI(){ return m_prefs_gui; }
public:
  avdPrefsItem(
    QListBox *listbox,
    const QString &text = QString::null
  ):QListBoxText(listbox, text), m_prefs_gui(0){}
  avdPrefsItem(
    QListBox *listbox,
    const QString &text,
    QListBoxItem *after
  ):QListBoxText(listbox, text), m_prefs_gui(0){}
  ~avdPrefsItem(){ setPrefsGUI(0); }
};

#endif

// arch-tag: proto file for preferences item object
