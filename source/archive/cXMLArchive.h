#ifndef cXMLArchive_h
#define cXMLArchive_h

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/shared_ptr.hpp>

//#ifndef tPtr_h
//#include "tPtr.h"
//#endif

///*
//This causes the boost serialization library to treat tPtr as
//boost::shared_ptr.
//*/
//namespace boost { namespace serialization {
//template<class Archive, class T>
//void serialize(Archive & a, tPtr<T> &t, const unsigned int version)
//{ boost::serialization::split_free(a, t, version); }
//} // namespace serialization
//} // namespace boost


class cXMLIArchive : public boost::archive::xml_iarchive_impl<cXMLIArchive> {
public:
  cXMLIArchive(std::istream & is, unsigned int flags = 0)
  : boost::archive::xml_iarchive_impl<cXMLIArchive>(is, flags)
  {}
  ~cXMLIArchive(){};

  template<class T>
  cXMLIArchive & ArkvObj(const char * name, T & t){
    return *(this->This()) >> boost::serialization::make_nvp(name, t);
  }
  cXMLIArchive & ArkvBinary(const char * name, void * t, std::size_t size){
    return *(this->This()) >> boost::serialization::make_nvp(
      name, boost::serialization::make_binary_object(t, size)
    );
  }
  template<class Base, class Derived>
  cXMLIArchive & ArkvBase(const char * name, Base &, Derived & d){
    return *(this->This()) >> boost::serialization::make_nvp(
      name, boost::serialization::base_object<Base >(d)
    );
  }
  template<class Base, class Derived>
  cXMLIArchive & ArkvBase(const char * name, Derived & d){
    return *(this->This()) >> boost::serialization::make_nvp(
      name, boost::serialization::base_object<Base >(d)
    );
  }

  template<class T>
  void SplitLoadSave(T & t, const unsigned int version){
    boost::serialization::split_member(*this, t, version);
  }
};

class cXMLOArchive : public boost::archive::xml_oarchive_impl<cXMLOArchive> {
public:
  cXMLOArchive(std::ostream & os, unsigned int flags = 0)
  : boost::archive::xml_oarchive_impl<cXMLOArchive>(os, flags)
  {}
  ~cXMLOArchive(){}

  template<class T>
  cXMLOArchive & ArkvObj(const char * name, T & t){
    return *(this->This()) << boost::serialization::make_nvp(name, t);
  }
  cXMLOArchive & ArkvBinary(const char * name, void * t, std::size_t size){
    return *(this->This()) << boost::serialization::make_nvp(
      name, boost::serialization::make_binary_object(t, size)
    );
  }
  template<class Base, class Derived>
  cXMLOArchive & ArkvBase(const char * name, Base &, Derived & d){
    return *(this->This()) << boost::serialization::make_nvp(
      name, boost::serialization::base_object<Base >(d)
    );
  }
  template<class Base, class Derived>
  cXMLOArchive & ArkvBase(const char * name, Derived & d){
    return *(this->This()) << boost::serialization::make_nvp(
      name, boost::serialization::base_object<Base >(d)
    );
  }

  template<class T>
  void SplitLoadSave(T & t, const unsigned int version){
    boost::serialization::split_member(*this, t, version);
  }
};



#endif
