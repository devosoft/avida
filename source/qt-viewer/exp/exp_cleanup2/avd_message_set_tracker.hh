#ifndef AVD_MESSAGE_SET_TRACKER_HH
#define AVD_MESSAGE_SET_TRACKER_HH

#include <string>

/* XXX disables the Loki small-object allocator; it crashes under OSX10. */
#ifndef MAX_SMALL_OBJECT_SIZE
#define MAX_SMALL_OBJECT_SIZE 0
#endif
#ifndef ASSOCVECTOR_INC_
#include "third-party/Loki/AssocVector.h"
#endif
#ifndef SINGLETON_INC_
#include "third-party/Loki/Singleton.h"
#endif

#ifndef AVD_MESSAGE_TYPE_SET_HH
#include "avd_message_type_set.hh"
#endif

class avdMessageTypeBase;
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
typedef Loki::AssocVector<std::string, avdMessageTypeBase *>::iterator
  avdMessageTypeIterator;
typedef Loki::SingletonHolder<_avdMessageSetTracker> avdMessageSetTracker;
typedef Loki::AssocVector<std::string, avdMessageTypeSet *>::iterator
  avdMessageSetTrackerIterator;

#endif
