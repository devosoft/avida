
#include "cXMLArchive.h"

#ifdef ENABLE_SERIALIZATION

// explicitly instantiate for this type of xml stream
#include <boost/archive/impl/basic_xml_iarchive.ipp>
#include <boost/archive/impl/archive_pointer_iserializer.ipp>
#include <boost/archive/impl/xml_iarchive_impl.ipp>
#include <boost/archive/impl/basic_xml_oarchive.ipp>
#include <boost/archive/impl/archive_pointer_oserializer.ipp>
#include <boost/archive/impl/xml_oarchive_impl.ipp>

template class boost::archive::basic_xml_iarchive<cXMLIArchive> ;
template class boost::archive::detail::archive_pointer_iserializer<cXMLIArchive> ;
template class boost::archive::xml_iarchive_impl<cXMLIArchive> ;
template class boost::archive::basic_xml_oarchive<cXMLOArchive> ;
template class boost::archive::detail::archive_pointer_oserializer<cXMLOArchive> ;
template class boost::archive::xml_oarchive_impl<cXMLOArchive> ;

#endif // ENABLE_SERIALIZATION
