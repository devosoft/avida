#ifndef RETAINABLE_DBG_HH
#include "retainable_dbg.hh"
#endif

#ifndef WATCH_RETAINS_HH
#include "watch_retains.hh"
#endif

cWatchRetains::~cWatchRetains(){
  cRetainable::s_retain_watchlist.Remove(this);
  for(std::set<cRetainable *>::const_iterator i(m_watchset.begin());
      i != m_watchset.end(); i++){
    cRetainableError
    << "Warning: possible memory leak of object \""
    << (*i)->watchedName() << "\"";
    cRetainableError
    << " (retain count " << (*i)->refs()
    << " at finish of leak-watch \"" << watchedName() << "\").";
    (*i)->m_watcher = 0;
} }
void cWatchRetains::nameWatched(const cString &name){ m_ret_name = name; }


