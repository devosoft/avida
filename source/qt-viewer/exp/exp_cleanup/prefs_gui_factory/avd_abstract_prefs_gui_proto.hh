#ifndef AVD_ABSTRACT_PREFS_GUI_PROTO_HH
#define AVD_ABSTRACT_PREFS_GUI_PROTO_HH

class avdMCMediator;
class avdPrefsItemView;
class QWidget;
class avdAbstractPrefsCtrl : public cRetainable {
public:
  virtual void setup(avdMCMediator *mediator) = 0;
  virtual QWidget *getView() = 0;
};

#endif

// arch-tag: proto file for generic preferences gui controller
