#ifndef AVD_MESSAGE_DISPLAY_TYPETRACK_PROTO_HH
#define AVD_MESSAGE_DISPLAY_TYPETRACK_PROTO_HH


class avdMessageTypeSet{
public:
  bool Register(const std::string &id, avdMessageType *mt)
  { return m_associations.insert(IdToMTMap::value_type(id, mt)).second; }
  bool Unregister(const std::string &id)
  { return m_associations.erase(id) == 1; }
  avdMessageType *GetTypeTracker(const std::string &id){
    IdToMTMap::iterator i = m_associations.find(id);
    if (i != m_associations.end()) return i->second;
    return 0;
  }
  Loki::AssocVector<std::string, avdMessageType *>::iterator
  begin(){ return m_associations.begin(); }
  Loki::AssocVector<std::string, avdMessageType *>::iterator
  end(){ return m_associations.end(); }
  Loki::AssocVector<std::string, avdMessageType *>::size_type
  size(){ return m_associations.size(); }
protected:
  typedef Loki::AssocVector<std::string, avdMessageType *> IdToMTMap;
  typedef Loki::AssocVector<std::string, avdMessageType *>::iterator Iterator;
  IdToMTMap m_associations;
};
class _avdMessageSetTracker{
public:
  bool Register(const std::string &id, avdMessageTypeSet *ts)
  { return m_associations.insert(IdToTSMap::value_type(id, ts)).second; }
  bool Unregister(const std::string &id)
  { return m_associations.erase(id) == 1; }
  avdMessageTypeSet *GetTypeTracker(const std::string &id){
    IdToTSMap::iterator i = m_associations.find(id);
    if (i != m_associations.end()) return i->second;
    return 0;
  }
  Loki::AssocVector<std::string, avdMessageTypeSet *>::iterator
  begin(){ return m_associations.begin(); }
  Loki::AssocVector<std::string, avdMessageTypeSet *>::iterator
  end(){ return m_associations.end(); }
  Loki::AssocVector<std::string, avdMessageTypeSet *>::size_type
  size(){ return m_associations.size(); }
protected:
  typedef Loki::AssocVector<std::string, avdMessageTypeSet *> IdToTSMap;
  typedef Loki::AssocVector<std::string, avdMessageTypeSet *>::iterator Iterator;
  IdToTSMap m_associations;
};
typedef Loki::AssocVector<std::string, avdMessageType *>::iterator
  avdMessageTypeIterator;
typedef Loki::AssocVector<std::string, avdMessageTypeSet *>::iterator
  avdMessageSetTrackerIterator;
typedef Loki::SingletonHolder<_avdMessageSetTracker> avdMessageSetTracker;

#endif

// arch-tag: proto file for debug-message display-type tracking
