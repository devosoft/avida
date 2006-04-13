/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// extended_type_info_typeid.cpp: specific implementation of type info
// that is based on typeid

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info_typeid.hpp>

namespace boost { 
namespace serialization { 
namespace detail {

BOOST_SERIALIZATION_DECL(bool)
extended_type_info_typeid_0::less_than(const extended_type_info &rhs) const
{
    return 0 != get_eti().before(
        static_cast<const extended_type_info_typeid_0 &>(rhs).get_eti()
    );
}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info_typeid_0::extended_type_info_typeid_0() :
    extended_type_info("extended_type_info_typeid")
{}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info_typeid_0::~extended_type_info_typeid_0()
{}

// this derivation is used for creating search arguments
class extended_type_info_typeid_arg : 
    public extended_type_info_typeid_0
{
private:
    const std::type_info & ti;
    virtual const std::type_info &get_eti() const
    {
        return ti;
    }
public:
    extended_type_info_typeid_arg(const std::type_info & ti_)
        : ti(ti_)
    { 
        // note absense of self register and key as this is used only as
        // search argument given a type_info reference and is not to 
        // be added to the map.
    }
};

BOOST_SERIALIZATION_DECL(const extended_type_info *)
extended_type_info_typeid_0::get_derived_extended_type_info(
    const std::type_info & ti
){
    detail::extended_type_info_typeid_arg etia(ti);
    return extended_type_info::find(& etia);
}

} // namespace detail
} // namespace serialization
} // namespace boost
