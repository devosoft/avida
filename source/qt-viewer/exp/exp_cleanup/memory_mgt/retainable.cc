#ifndef RETAINABLE_DBG_HH
#include "retainable_dbg.hh"
#endif
#ifndef WATCH_RETAINS_HH
#include "memory_mgt/watch_retains.hh"
#endif

#ifndef RETAINABLE_HH
#include "memory_mgt/retainable.hh"
#endif

// This is for displaying error messages.
cMessageType Error_cRetainable_Msg("Memory", MCError);


tRList<cWatchRetains> cRetainable::s_retain_watchlist;
cString cRetainable::s_noname_str("(unnamed)");
cString cRetainable::s_copyof_str("(copy of)");

void cRetainable::nameWatched(const cString &name){
  m_ret_name = name;
  if(!m_watcher){
    cRetainableError
    << "Warning: told to watch object \"" << watchedName() << "\"";
    cRetainableError
    << " for memory leaks, but no leak-watch is installed.";
} }
void cRetainable::endWatch(){
  if(m_watcher) m_watcher->m_watchset.erase(this);
  m_watcher = 0;
}
void cRetainable::beWatched(){
  if(0 < s_retain_watchlist.GetSize()){
    m_watcher = s_retain_watchlist.GetFirst();
    m_watcher->m_watchset.insert(this);
  } else m_watcher = 0;
}

// arch-tag: implementation file for retainable object base class
