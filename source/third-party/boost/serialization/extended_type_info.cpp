/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// extended_type_info.cpp: implementation for portable version of type_info

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.


#include <algorithm>
#include <set>
#include <cassert>

#include <boost/config.hpp> // msvc needs this to suppress warning

#include <cstring>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ using ::strcmp; }
#endif

#include <boost/detail/no_exceptions_support.hpp>
#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info.hpp>

namespace boost { 
namespace serialization {

// remove all registrations corresponding to a given type
void unregister_void_casts(extended_type_info *eti);

namespace detail {

// it turns out that at least one compiler (msvc 6.0) doesn't guarentee
// to destroy static objects in exactly the reverse sequence that they
// are constructed.  To guarentee this, use a singleton pattern

// map for finding the unique global extended type entry for a given type
class tkmap {
    struct type_info_compare
    {
        bool
        operator()(const extended_type_info * lhs, const extended_type_info * rhs) const
        {
            assert(! lhs->is_destructing());
            assert(! rhs->is_destructing());
            return *lhs < *rhs;
        }
    };
//    typedef std::multiset<const extended_type_info *, type_info_compare> type;
    typedef std::set<const extended_type_info *, type_info_compare> type;
    type m_map;
    static tkmap * m_self;
    tkmap(){}
    static tkmap::type::iterator
    lookup(const extended_type_info * eti){
        return m_self->m_map.find(eti);
    }
public:
    ~tkmap(){
        m_self = NULL;
    }
    static void
    insert(const extended_type_info * eti){
        if(NULL == m_self){
            static tkmap instance;
            m_self = & instance;
        }
        // make sure that attempt at registration is done only once
        assert(lookup(eti) == m_self->m_map.end());
        m_self->m_map.insert(eti);
    }
    static const extended_type_info * 
    find(const extended_type_info * eti){
        if(NULL == m_self)
            return NULL;
        tkmap::type::const_iterator it;
        it = m_self->m_map.find(eti);
        if(it == m_self->m_map.end())
            return NULL;
        return *it;
    }
    static void 
    purge(const extended_type_info * eti){
        if(NULL == m_self)
            return;
        // note: the following can't be used as this function
        // is called from a destructor of extended_type_info.
        // This will generate an error on some machines - which
        // makes sense be cause by this time the derived class data
        // might be gone.  Leave this in as a reminder not to do this
        #if 0
        tkmap::type::iterator it;
        it = lookup(eti);
        // it should be in there
        assert(it != m_self->m_map.end());
        m_self->m_map.erase(it);
        #endif

        tkmap::type::iterator i = m_self->m_map.begin();
        tkmap::type::iterator k = m_self->m_map.end();
        while(i != k){
            // note that the erase might invalidate i so save it here
            tkmap::type::iterator j = i++;
            if(*j == eti)
                m_self->m_map.erase(j);
        }
    }
};

tkmap * tkmap::m_self = NULL;

// map for finding the unique global extended type info entry given its GUID
class ktmap {
    struct key_compare
    {
        bool
        operator()(const extended_type_info * lhs, const extended_type_info * rhs) const
        {
            // shortcut to exploit string pooling
            if(lhs->get_key() == rhs->get_key())
                return false;
            if(NULL == lhs->get_key())
                return true;
            if(NULL == rhs->get_key())
                return false;
            return std::strcmp(lhs->get_key(), rhs->get_key()) < 0; 
        }
    };
//    typedef std::multiset<const extended_type_info *, key_compare> type;
    typedef std::set<const extended_type_info *, key_compare> type;
    type m_map;
    static ktmap * m_self;
    ktmap(){}
    class extended_type_info_arg : public extended_type_info
    {
    public:
        extended_type_info_arg(const char * key) :
            extended_type_info(NULL)
        {
            m_key = key;
        }
        virtual bool
        less_than(const extended_type_info &rhs) const
        {
            assert(false);
            return false;   // to prevent a syntax error
        }
    };
    static ktmap::type::iterator
    lookup(const char *key){
        extended_type_info_arg arg(key);
        return m_self->m_map.find(&arg);
    }

public:
    ~ktmap(){
        m_self = NULL;
    }
    static void
    insert(const extended_type_info * eti){
        if(NULL == m_self){
            static ktmap instance;
            m_self = & instance;
        }
        // make sure that all GUIDs are unique
        assert(lookup(eti->get_key()) == m_self->m_map.end());
        m_self->m_map.insert(eti);
    }
    static const extended_type_info * 
    find(const char *key)
    {
        if(NULL == m_self)
            return NULL;
        extended_type_info_arg arg(key);
        ktmap::type::const_iterator it;
        it = m_self->m_map.find(&arg);
        if(it == m_self->m_map.end())
            return NULL;
        return *it;
    }
    static void 
    purge(const extended_type_info * eti){
        if(NULL == m_self)
            return;
        // note: the following can't be used as this function
        // is called from a destructor of extended_type_info.
        // This will generate an error on some machines - which
        // makes sense be cause by this time the derived class data
        // might be gone.  Leave this in as a reminder not to do this
        #if 0
        ktmap::type::iterator it;
        it = lookup(eti->get_key());
        // expect it to be in there !
        assert(it != m_self->m_map.end());
        m_self->m_map.erase(it);
        #endif

        ktmap::type::iterator i = m_self->m_map.begin();
        ktmap::type::iterator k = m_self->m_map.end();
        while(i != k){
            // note that the erase might invalidate i so save it here
            ktmap::type::iterator j = i++;
            if(*j == eti)
                m_self->m_map.erase(j);
        }
    }
};

ktmap * ktmap::m_self = NULL;

} // namespace detail

BOOST_SERIALIZATION_DECL(const extended_type_info *) 
extended_type_info::find(const char *key)
{
    return detail::ktmap::find(key);
}

BOOST_SERIALIZATION_DECL(void) 
extended_type_info::self_register()
{
    detail::tkmap::insert(this);
    m_self_registered = true;
}

BOOST_SERIALIZATION_DECL(void)  
extended_type_info::key_register(const char *key_) {
    if(NULL == key_)
        return;
    m_key = key_;
    detail::ktmap::insert(this);
    m_key_registered = true;
}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info::extended_type_info(
    const char * type_info_key
) :
    m_type_info_key(type_info_key),
    m_self_registered(false),
    m_key_registered(false),
    m_is_destructing(false)
{}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY()) 
extended_type_info::~extended_type_info(){
    // remove entries in maps which correspond to this type
    m_is_destructing = true;
    BOOST_TRY{
        if(m_self_registered)
            detail::tkmap::purge(this);
        if(m_key_registered)
            detail::ktmap::purge(this);
        unregister_void_casts(this);
    }
    BOOST_CATCH(...){}
    BOOST_CATCH_END
}

BOOST_SERIALIZATION_DECL(int)
extended_type_info::type_info_key_cmp(const extended_type_info & rhs) const {
    if(m_type_info_key == rhs.m_type_info_key)
        return 0;
    //return strcmp(lhs.type_info_key, rhs.type_info_key);
    // all we require is that the type_info_key be unique
    // so just compare the addresses
    return m_type_info_key < rhs.m_type_info_key ? -1 : 1;
}

BOOST_SERIALIZATION_DECL(const extended_type_info *) 
extended_type_info::find(const extended_type_info * t)
{
    return detail::tkmap::find(t);
}

BOOST_SERIALIZATION_DECL(bool)
extended_type_info::operator<(const extended_type_info &rhs) const {
    int i = type_info_key_cmp(rhs);
    if(i < 0)
        return true;
    if(i > 0)
        return false;
    assert(! is_destructing());
    assert(! rhs.is_destructing());
    return less_than(rhs);
}

} // namespace serialization
} // namespace boost
