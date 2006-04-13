/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_archive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.
#include <utility>
#include <boost/shared_ptr.hpp>

#define BOOST_ARCHIVE_SOURCE
#include <boost/archive/detail/auto_link_archive.hpp>
#include <boost/archive/detail/basic_archive_impl.hpp>

namespace boost {
namespace archive {
namespace detail {

BOOST_ARCHIVE_DECL(void)
basic_archive_impl::lookup_helper(
    const boost::serialization::extended_type_info * const eti,
    shared_ptr<void> & sph
){
    helper_iterator it;
    const helper_type ht(sph, eti);
    it = m_helpers.find(ht);
    if(it == m_helpers.end())
        sph.reset();
    else
        sph = it->m_helper;
}

BOOST_ARCHIVE_DECL(void)
basic_archive_impl::insert_helper(
    const boost::serialization::extended_type_info * const eti,
    shared_ptr<void> & sph
){
    const helper_type ht(sph, eti);
    std::pair<helper_iterator, bool> result = m_helpers.insert(ht);
}

} // namespace detail
} // namespace serialization
} // namespace boost
