#ifndef TGUI_PROTO_HH
#define TGUI_PROTO_HH

/*
The main purpose of these two connection classes is decoupling of your
gui classes from class avdMCMediator and class avdAbstractCtrl.

The second purpose is giving means to easily select which avdMCMediator
signals to connect to your gui classes.  Here's how it works.

You provide a gui controller class that looks like this (the arguments
to the constructor, and the return types and arguments to setup(),
wrapupInit(), and menuBar() are the important parts):

  class MyController {
    Q_OBJECT
  public:
    MyController(QObject *parent = 0, const char *name = 0);
    void setup(avdAvidaThreadDrvr *driver = 0);
    void wrapupInit(void);
    QMenuBar *menuBar(void);

  // You fill in the variables, methods, signals and slots as needed:
  protected;
    cFoo m_my_foo;
  protected:
    void myMethod();
  signals:
    void aSignal();
    void anotherSignal(int cell_id);
  public slots:
    void aSlot();
  };

You provide a signal/slot connection policy that looks like this (the
return type and arguments to setupConnections() are important, and the
class has to be a template class):

  template <class MyController> class MyConnectionPolicy {
  protected:
    void setupConnections(avdMCMediator *mediator, MyController *controller){

      // You fill-in your version of setupConnections() to connect signals
      // and slots between the mediator and the controller, as needed:
      QObject::connect(
        mediator, SIGNAL(avidaStateChangedSig()),
        controller, SLOT(aSlot())
      );
      QObject::connect(
        controller, SIGNAL(aSignal()),
        mediator, SIGNAL(doStartAvidaSig())
      );
      QObject::connect(
        controller, SIGNAL(anotherSignal(int)),
        mediator, SIGNAL(doStepAvidaSig(int))
      );
    }
  };

Finally you define your gui datatype, for use by the gui factory, as
follows:

  typedef tGUI<MyController, MyConnectionPolicy> myGUI;

****

Here's how to register the class myGUI with the gui factor:

Write a creation function for use by the gui factor that looks something
like this:

  avdAbstractCtrl *createMyGUI(void){
    myGUI *g = new myGUI(myParentObject, "myName");
    return g;
  }

Create a key for the factory to use to identify createMyGUI():

  QString s_key("myGUIKey");

Now register the key and the creation function:

  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createMyGUI);

Finally, if you like, head over to the file "avd_menubar_hdlr_prv.cc" to
add an menu item for creating myGUI objects, using the key "myGUIKey".
*/


class avdAvidaThreadDrvr;
class avdMCMediator;

/*
This null policy doesn't connect mediator to gui in any way.  If you
provide one, your connection policy should provide the function

- setupConnections(avdMCMediator *mediator, avdAbstractCtrl *gui)

in which you can connect mediator to gui as you like.  Your version of
setupConnections() will be called in the template class below.
*/
template <class Ctrl> class pNullConnectionPolicy {
protected:
  void setupConnections(avdMCMediator *, Ctrl *){}
};

template <
  class Ctrl,
  template <class> class ConnectionPolicy = pNullConnectionPolicy
> class tGUI
: public QObject
, public avdAbstractCtrl
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
  ~tGUI(){ setCtrl(0); }
  tGUI(QObject *parent = 0, const char *name = 0)
  : QObject(parent, name)
  { setCtrl(new Ctrl(0, "<tGUI(m_ctrl)>"))->release();
    connect(getCtrl(), SIGNAL(destroyed()), this, SLOT(deleteLater()));
  }
  bool setup(avdMCMediator *mediator){
    if(!getCtrl()){ return false; }
    if(!getCtrl()->setup()){ return false; }
    setupConnections(mediator, getCtrl()); return true;
  }
};

#endif

// arch-tag: proto file for generic gui object factory template
