#ifndef TPREFSGUI_PROTO_HH
#define TPREFSGUI_PROTO_HH


class avdMCMediator;
class QWidget;

template <class Ctrl> class pNullConnectionPolicy2 {
protected:
  void setupConnections(avdMCMediator *, Ctrl *){}
};

template <
  class Ctrl,
  template <class> class ConnectionPolicy = pNullConnectionPolicy2
> class tPrefsGUI
: public QObject
, public avdAbstractPrefsCtrl
, public ConnectionPolicy<Ctrl>
{
protected:
  QGuardedPtr<Ctrl> m_ctrl;
protected:
  Ctrl *setCtrl(Ctrl *ctrl)
  { SETretainable(m_ctrl, ctrl);
    return getCtrl();
  }
public:
  Ctrl *getCtrl() { return m_ctrl; }
public:
  ~tPrefsGUI(){ setCtrl(0); }
  tPrefsGUI(QObject *parent = 0, const char *name = 0)
  : QObject(parent, name)
  { setCtrl(new Ctrl(0, "<tPrefsGUI(m_ctrl)>"))->release();
    connect(getCtrl(), SIGNAL(destroyed()), this, SLOT(deleteLater()));
  }
  void setup(avdMCMediator *mediator)
  { setupConnections(mediator, getCtrl());
    if(getCtrl()) getCtrl()->setup();
  }
  QWidget *getView(){ return (getCtrl())?(getCtrl()->getView()):(0); }
};

#endif

// arch-tag: proto file for generic preferences gui template
