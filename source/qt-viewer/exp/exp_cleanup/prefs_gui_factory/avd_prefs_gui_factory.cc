#include <iostream>
#ifndef AVD_PREFS_GUI_FACTORY_HH
#include "prefs_gui_factory/avd_prefs_gui_factory.hh"
#endif


struct _avdPrefsControllerFactoryPrv { QValueList<QString> m_keys; };

_avdPrefsControllerFactory::_avdPrefsControllerFactory():m_d(0){ m_d = new _avdPrefsControllerFactoryPrv; }
_avdPrefsControllerFactory::~_avdPrefsControllerFactory(){ delete m_d; }
bool _avdPrefsControllerFactory::Register(
  const QString &id, Loki::Functor<avdAbstractPrefsCtrl *> creator)
{
  bool retval = __avdPrefsControllerFactory::Register(id, creator);
  if(retval){
    std::cout
    << "<__avdPrefsControllerFactory::Register> succeeded registering \""
    << id << "\"." << std::endl;
    m_d->m_keys.push_back(id);
  } else {
    std::cout
    << "<__avdPrefsControllerFactory::Register> failed registering \""
    << id << "\"." << std::endl;
  }
  return retval;
}
bool _avdPrefsControllerFactory::Unregister(const QString &id){
  bool retval = __avdPrefsControllerFactory::Unregister(id);
  if(retval){
    QValueList<QString>::iterator it(m_d->m_keys.find(id));
    if(it != (m_d->m_keys.end())) m_d->m_keys.erase(it);
  }
  return retval;
}
bool _avdPrefsControllerFactory::IsRegistered(const QString &id)
{ return ((m_d->m_keys.contains(id))); }


// arch-tag: implementation file for generic preferences gui controller factory
