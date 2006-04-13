/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// void_cast.cpp: implementation of run-time casting of void pointers

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// <gennadiy.rozental@tfn.com>

//  See http://www.boost.org for updates, documentation, and revision history.

#if (defined _MSC_VER) && (_MSC_VER == 1200)
# pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <cassert>

// STL
#include <set>
#include <functional>
#include <algorithm>
#include <cassert>

// BOOST
#include <boost/shared_ptr.hpp>
#define BOOST_SERIALIZATION_SOURCE
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/void_cast.hpp>

namespace boost { 
namespace serialization {
namespace void_cast_detail {

struct void_caster_compare
{
    bool
    operator()(
        shared_ptr<const void_caster> lhs, 
        shared_ptr<const void_caster> rhs ) const
    {
        if( lhs.get()->m_derived_type < rhs.get()->m_derived_type )
            return true;
        
        if( rhs.get()->m_derived_type < lhs.get()->m_derived_type)
            return false;
        
        if( lhs.get()->m_base_type < rhs.get()->m_base_type )
            return true;

        return false;
    }
};

struct null_deleter
{
    void operator()(void const *) const
    {}
};

// it turns out that at least one compiler (msvc 6.0) doesn't guarentee
// to destroy static objects in exactly the reverse sequence that they
// are constructed.  To guarentee this, use a singleton pattern
class void_caster_registry
{
    typedef shared_ptr<const void_caster> value_type;
    typedef std::set<value_type, void_caster_compare> set_type;
    set_type m_set;
    static void_caster_registry * m_self;
    static void_caster_registry * 
    self(){
        if(NULL == m_self){
            static void_caster_registry instance;
            m_self = & instance;
        }
        return m_self;
    }
    void_caster_registry(){}
public:
    ~void_caster_registry(){
        m_self = 0;
    }
    typedef set_type::iterator iterator;
    typedef set_type::const_iterator const_iterator;
    static iterator 
    begin() {
        return self()->m_set.begin();
    }
    static iterator 
    end() {
        return self()->m_set.end();
    }
    static const_iterator 
    find(void_caster * vcp){
        return self()->m_set.find(value_type(vcp, null_deleter()));
    }
    static std::pair<iterator, bool> 
    insert(const value_type & vcp){
        return self()->m_set.insert(vcp);
    }
    static bool 
    empty(){
        if(NULL == m_self)
            return true;
        return m_self->m_set.empty();
    }
    static void 
    purge(const extended_type_info * eti);
};

void_caster_registry * void_caster_registry::m_self = NULL;

void 
void_caster_registry::purge(const extended_type_info * eti){
    if(NULL == m_self)
        return;
    if(! empty()){
        iterator i = m_self->m_set.begin();
        while(i != m_self->m_set.end()){
            // note that the erase might invalidate i so save it here
            iterator j = i++;
            if((*j)->includes(eti))
                m_self->m_set.erase(j);
        }
    }
}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY())
void_caster::void_caster(
    extended_type_info const & derived_type_,
    extended_type_info const & base_type_ 
) :
    m_derived_type( derived_type_),
    m_base_type(base_type_)
{}

BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY())
void_caster::~void_caster(){}

bool 
void_caster::includes(const extended_type_info * eti) const {
    return & m_derived_type == eti || & m_base_type == eti;
}

void BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY())
void_caster::static_register(const void_caster * vcp) 
{
    void_caster_registry::insert(shared_ptr<const void_caster>(vcp, null_deleter()));
}

class void_caster_derived : public void_caster
{
    std::ptrdiff_t difference;
    virtual void const*
    upcast( void const* t ) const{
        return static_cast<const char*> ( t ) + difference;
    }
    virtual void const*
    downcast( void const* t ) const{
        return static_cast<const char*> ( t ) - difference;
    }
public:
    void_caster_derived(
        extended_type_info const& derived_type_,
        extended_type_info const& base_type_,
        std::ptrdiff_t difference_
    ) :
        void_caster(derived_type_, base_type_),
        difference( difference_ )
    {}
};

// just used as a search key
class void_caster_argument : public void_caster
{
    virtual void const*
    upcast( void const* t ) const {
        assert(false);
        return NULL;
    }
    virtual void const*
    downcast( void const* t ) const {
        assert(false);
        return NULL;
    }
public:
    void_caster_argument(
        extended_type_info const& derived_type_,
        extended_type_info const& base_type_
    ) :
        void_caster(derived_type_, base_type_)
    {}
};

} // namespace void_cast_detail

void  BOOST_SERIALIZATION_DECL(BOOST_PP_EMPTY())
unregister_void_casts(extended_type_info *eti)
{
    void_cast_detail::void_caster_registry::purge(eti);
}

// Given a void *, assume that it really points to an instance of one type
// and alter it so that it would point to an instance of a related type.
// Return the altered pointer. If there exists no sequence of casts that
// can transform from_type to to_type, return a NULL.  

BOOST_SERIALIZATION_DECL(void const *)
void_upcast(
    extended_type_info const & derived_type,
    extended_type_info const & base_type,
    void const * const t,
    bool top
){
    // same types - trivial case
    if (derived_type == base_type)
        return t;
    
    // check to see if base/derived pair is found in the registry
    void_cast_detail::void_caster_argument ca(derived_type, base_type );
    void_cast_detail::void_caster_registry::const_iterator it;
    it = void_cast_detail::void_caster_registry::find( &ca );
    
    const void * t_new = NULL;

    // if so
    if (it != void_cast_detail::void_caster_registry::end())
        // we're done
        return (*it)->upcast(t);

    // try to find a chain that gives us what we want
    for(
        it = void_cast_detail::void_caster_registry::begin();
        it != void_cast_detail::void_caster_registry::end();
        ++it
    ){
        // if the current candidate doesn't cast to the desired target type
        if ((*it)->m_base_type == base_type){
            // if the current candidate casts from the desired source type
            if ((*it)->m_derived_type == derived_type){
                // we have a base/derived match - we're done
                // cast to the intermediate type
                t_new = (*it)->upcast(t);
                break;
            }
            t_new = void_upcast(derived_type, (*it)->m_derived_type, t, false);
            if (NULL != t_new){
                t_new = (*it)->upcast(t_new);
                assert(NULL != t_new);
                if(top){
                    // register the this pair so we will have to go through
                    // keep this expensive search process more than once.
                    void_cast_detail::void_caster * vcp = 
                        new void_cast_detail::void_caster_derived( 
                            derived_type,
                            base_type,
                            static_cast<const char*>(t_new) - static_cast<const char*>(t)
                        );
                    void_cast_detail::void_caster_registry::insert(
                        shared_ptr<const void_cast_detail::void_caster>(vcp)
                    );
                }
                break;
            }
        }
    }
    return t_new;
}

BOOST_SERIALIZATION_DECL(void const *)
void_downcast(
    const extended_type_info & derived_type,
    const extended_type_info & base_type,
    const void * const t,
    bool top
){
    // same types - trivial case
    if (derived_type == base_type)
        return t;
    
    // check to see if base/derived pair is found in the registry
    void_cast_detail::void_caster_argument ca(derived_type, base_type );
    void_cast_detail::void_caster_registry::const_iterator it;
    it = void_cast_detail::void_caster_registry::find( &ca );
    
    // if so
    if (it != void_cast_detail::void_caster_registry::end())
        // we're done
        return (*it)->downcast(t);

    const void * t_new = NULL;
    // try to find a chain that gives us what we want
    for(
        it = void_cast_detail::void_caster_registry::begin();
        it != void_cast_detail::void_caster_registry::end();
        ++it
    ){
        // if the current candidate doesn't casts from the desired target type
        if ((*it)->m_derived_type == derived_type){
            // if the current candidate casts to the desired source type
            if ((*it)->m_base_type == base_type){
                // we have a base/derived match - we're done
                // cast to the intermediate type
                t_new = (*it)->downcast(t);
                break;
            }
            t_new = void_downcast((*it)->m_base_type, base_type, t, false);
            if (NULL != t_new){
                t_new = (*it)->downcast(t_new);
                assert(NULL != t_new);
                if(top){
                    // register the this pair so we will have to go through
                    // keep this expensive search process more than once.
                    void_cast_detail::void_caster * vcp = 
                        new void_cast_detail::void_caster_derived( 
                            derived_type,
                            base_type,
                            static_cast<const char*>(t) - static_cast<const char*>(t_new)
                        );
                    void_cast_detail::void_caster_registry::insert(
                        shared_ptr<const void_cast_detail::void_caster>(vcp)
                    );
                }
                break;
            }
        }
    }
    return t_new;
}

} // namespace serialization
} // namespace boost

// EOF
