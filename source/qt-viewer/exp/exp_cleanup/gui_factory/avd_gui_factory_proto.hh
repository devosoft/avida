#ifndef AVD_GUI_FACTORY_PROTO_HH
#define AVD_GUI_FACTORY_PROTO_HH

typedef
Loki::Factory<
  avdAbstractCtrl,
  QString,
  Loki::Functor<avdAbstractCtrl *>,
  avdControllerFactoryErrorPolicy
> __avdControllerFactory;

class _avdControllerFactoryPrv;
class _avdControllerFactory : public __avdControllerFactory {
  _avdControllerFactoryPrv *m_d;
public:
  _avdControllerFactory();
  ~_avdControllerFactory();
  bool Register(const QString &id, Loki::Functor<avdAbstractCtrl *> creator);
  bool Unregister(const QString &id);
  bool IsRegistered(const QString &id);
};

typedef Loki::SingletonHolder<_avdControllerFactory> avdControllerFactory;

#endif

// arch-tag: proto file for generic gui object factory
