/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_archive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#define BOOST_ARCHIVE_SOURCE
#include <boost/archive/basic_xml_archive.hpp>

namespace boost {
namespace archive {

BOOST_ARCHIVE_DECL(const char *)
OBJECT_ID(){
    return "object_id";
}
BOOST_ARCHIVE_DECL(const char *)
OBJECT_REFERENCE(){
    return "object_id_reference";
}
BOOST_ARCHIVE_DECL(const char *)
CLASS_ID(){
    return "class_id";
}
BOOST_ARCHIVE_DECL(const char *)
CLASS_ID_REFERENCE(){
    return "class_id_reference";
}
BOOST_ARCHIVE_DECL(const char *)
CLASS_NAME(){
    return "class_name";
}
BOOST_ARCHIVE_DECL(const char *)
TRACKING(){
    return "tracking_level";
}
BOOST_ARCHIVE_DECL(const char *)
VERSION(){
    return "version";
}
BOOST_ARCHIVE_DECL(const char *)
SIGNATURE(){
    return "signature";
}

}// namespace archive
}// namespace boost
