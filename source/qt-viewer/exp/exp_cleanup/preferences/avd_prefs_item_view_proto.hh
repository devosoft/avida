#ifndef AVD_PREFS_ITEM_VIEW_PROTO_HH
#define AVD_PREFS_ITEM_VIEW_PROTO_HH

class avdPrefsItemView : public QWidget, public cRetainable {
public:
  explicit avdPrefsItemView(
    QWidget *parent = 0,
    const char *name = 0,
    WFlags f = 0
  ):QWidget(parent, name, f){}
};

#endif

// arch-tag: proto file for preferences item gui view
