#include <iostream>
#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif


struct _avdControllerFactoryPrv { QValueList<QString> m_keys; };

_avdControllerFactory::_avdControllerFactory():m_d(0){ m_d = new _avdControllerFactoryPrv; }
_avdControllerFactory::~_avdControllerFactory(){ delete m_d; }
bool _avdControllerFactory::Register(
  const QString &id, Loki::Functor<avdAbstractCtrl *> creator)
{
  bool retval = __avdControllerFactory::Register(id, creator);
  if(retval){
    std::cout
    << "<__avdControllerFactory::Register> succeeded registering \""
    << id << "\"." << std::endl;
    m_d->m_keys.push_back(id);
  } else {
    std::cout
    << "<__avdControllerFactory::Register> failed registering \""
    << id << "\"." << std::endl;
  }
  return retval;
}
bool _avdControllerFactory::Unregister(const QString &id){
  bool retval = __avdControllerFactory::Unregister(id);
  if(retval){
    QValueList<QString>::iterator it(m_d->m_keys.find(id));
    if(it != (m_d->m_keys.end())) m_d->m_keys.erase(it);
  }
  return retval;
}
bool _avdControllerFactory::IsRegistered(const QString &id)
{ return ((m_d->m_keys.contains(id))); }

// arch-tag: implementation file for generic gui object factory
