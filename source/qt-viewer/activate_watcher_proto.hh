#ifndef ACTIVATE_WATCHER_PROTO_HH
#define ACTIVATE_WATCHER_PROTO_HH

class cActivateWatcher : public cRetainable {
private:
  /* disabled */ cActivateWatcher(const cActivateWatcher &);
  /* disabled */ cActivateWatcher operator =(const cActivateWatcher &);
protected:
  cWatchRetains *m_w;
protected:
  void setWatch(cWatchRetains *w){
    if(w) w->retain(); if(m_w) m_w->release(); m_w = w; }
  cWatchRetains *getWatch(){ return m_w; }
public:
  cActivateWatcher(cWatchRetains *w):m_w(0){
    setWatch(w); if(getWatch()) getWatch()->activate(); }
  ~cActivateWatcher(){
    if(getWatch()) getWatch()->deactivate(); setWatch(0); }
};

#endif
