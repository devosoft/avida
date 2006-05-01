SET(libboost_serialization_a_SOURCES
  basic_archive.cpp
  basic_archive_impl.cpp
  basic_iarchive.cpp
  basic_oarchive.cpp
  basic_iserializer.cpp
  basic_oserializer.cpp
  basic_pointer_iserializer.cpp
  basic_pointer_oserializer.cpp
  basic_serializer_map.cpp
  basic_text_iprimitive.cpp
  basic_text_oprimitive.cpp
  basic_xml_archive.cpp
  binary_iarchive.cpp
  binary_oarchive.cpp
  extended_type_info.cpp
  extended_type_info_no_rtti.cpp
  extended_type_info_typeid.cpp
  polymorphic_iarchive.cpp
  polymorphic_oarchive.cpp
  stl_port.cpp
  text_iarchive.cpp
  text_oarchive.cpp
  void_cast.cpp
  xml_grammar.cpp
  xml_iarchive.cpp
  xml_oarchive.cpp
)

INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_LIBRARY(boost_serialization ${libboost_serialization_a_SOURCES})
