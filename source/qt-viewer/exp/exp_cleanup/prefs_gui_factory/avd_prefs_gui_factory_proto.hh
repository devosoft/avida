#ifndef AVD_PREFS_GUI_FACTORY_PROTO_HH
#define AVD_PREFS_GUI_FACTORY_PROTO_HH

typedef
Loki::Factory<
  avdAbstractPrefsCtrl,
  QString,
  Loki::Functor<avdAbstractPrefsCtrl *>,
  avdPrefsControllerFactoryErrorPolicy
> __avdPrefsControllerFactory;

class _avdPrefsControllerFactoryPrv;
class _avdPrefsControllerFactory : public __avdPrefsControllerFactory {
  _avdPrefsControllerFactoryPrv *m_d;
public:
  _avdPrefsControllerFactory();
  ~_avdPrefsControllerFactory();
  bool Register(const QString &id, Loki::Functor<avdAbstractPrefsCtrl *> creator);
  bool Unregister(const QString &id);
  bool IsRegistered(const QString &id);
};

typedef Loki::SingletonHolder<_avdPrefsControllerFactory> avdPrefsControllerFactory;

#endif

// arch-tag: proto file for generic preferences gui controller factory
