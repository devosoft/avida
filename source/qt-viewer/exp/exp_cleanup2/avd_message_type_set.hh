#ifndef AVD_MESSAGE_TYPE_SET_HH
#define AVD_MESSAGE_TYPE_SET_HH

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

#ifndef AVD_MESSAGE_TYPE_BASE_HH
#include "avd_message_type_base.hh"
#endif

class avdMessageTypeSet{
public:
  bool Register(const std::string &id, const avdMessageTypeBase *mt)
  { return m_associations.insert(IdToMTMap::value_type(id, mt)).second; }
  bool Unregister(const std::string &id)
  { return m_associations.erase(id) == 1; }
  const avdMessageTypeBase *GetTypeTracker(const std::string &id){
    IdToMTMap::iterator i = m_associations.find(id);
    if (i != m_associations.end()) return i->second;
    return 0;
  }
  Loki::AssocVector<std::string, const avdMessageTypeBase *>::iterator begin()
  { return m_associations.begin(); }
  Loki::AssocVector<std::string, const avdMessageTypeBase *>::iterator end()
  { return m_associations.end(); }
  Loki::AssocVector<std::string, const avdMessageTypeBase *>::size_type size()
  { return m_associations.size(); }
protected:
  typedef Loki::AssocVector<std::string, const avdMessageTypeBase *> IdToMTMap;
  typedef Loki::AssocVector<std::string, const avdMessageTypeBase *>::iterator Iterator;
  IdToMTMap m_associations;
};

#endif
