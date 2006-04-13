/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_archive.cpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <boost/config.hpp> // msvc 6.0 needs this to suppress warnings

#include <cassert>
#include <set>
#include <list>
#include <vector>
#include <cstddef> // size_t

#include <boost/config.hpp>
#if defined(BOOST_NO_STDC_NAMESPACE)
namespace std{ 
    using ::size_t; 
} // namespace std
#endif

#define BOOST_ARCHIVE_SOURCE
#include <boost/archive/detail/auto_link_archive.hpp>

#include <boost/limits.hpp>
#include <boost/state_saver.hpp>
#include <boost/throw_exception.hpp>

#include <boost/archive/detail/basic_iserializer.hpp>
#include <boost/archive/detail/basic_pointer_iserializer.hpp>
#include <boost/archive/detail/basic_iarchive.hpp>
#include <boost/archive/detail/basic_archive_impl.hpp>
#include <boost/archive/archive_exception.hpp>

#include <boost/serialization/tracking.hpp>
#include <boost/serialization/extended_type_info.hpp>

using namespace boost::serialization;

namespace boost {
namespace archive {
namespace detail {

class basic_iserializer;
class basic_pointer_iserializer;

class basic_iarchive_impl :
    public basic_archive_impl
{
    friend class basic_iarchive;

    version_type m_archive_library_version;
    unsigned int m_flags;

    //////////////////////////////////////////////////////////////////////
    // information about each serialized object loaded
    // indexed on object_id
    struct aobject
    {
        void * address;
        class_id_type class_id;
        aobject(
            void *a,
            class_id_type class_id_
        ) :
            address(a),
            class_id(class_id_)
        {}
        aobject() : address(NULL), class_id(-2) {}
    };
    typedef std::vector<aobject> object_id_vector_type;
    object_id_vector_type object_id_vector;

    //////////////////////////////////////////////////////////////////////
    // used to implement the reset_object_address operation.
    // list of objects which might be moved. We use a vector for implemenation
    // in the hope the the truncation operation will be faster than either
    // with a list or stack adaptor
    object_id_type moveable_objects_start;
    object_id_type moveable_objects_end;
    object_id_type moveable_objects_recent;

    void reset_object_address(
        const void * new_address, 
        const void *old_address
    );

    //////////////////////////////////////////////////////////////////////
    // used by load object to look up class id given basic_serializer
    struct cobject_type
    {
        const basic_iserializer * bis;
        const class_id_type class_id;
        cobject_type(
            class_id_type class_id_,
            const basic_iserializer & bis_
        ) : 
            bis(& bis_),
            class_id(class_id_)
        {}
        cobject_type(const cobject_type & rhs) : 
            bis(rhs.bis),
            class_id(rhs.class_id)
        {}
        // the following cannot be defined because of the const
        // member.  This will generate a link error if an attempt
        // is made to assign.  This should never be necessary
        cobject_type & operator=(const cobject_type & rhs);
        bool operator<(const cobject_type &rhs) const
        {
            return *bis < *(rhs.bis);
        }
    };
    typedef std::set<cobject_type> cobject_info_set_type;
    cobject_info_set_type cobject_info_set;

    //////////////////////////////////////////////////////////////////////
    // information about each serialized class indexed on class_id
    class cobject_id 
    {
    public:
        cobject_id & operator=(const cobject_id & rhs){
            bis_ptr = rhs.bis_ptr;
            bpis_ptr = rhs.bpis_ptr;
            file_version = rhs.file_version;
            tracking_level = rhs.tracking_level;
            initialized = rhs.initialized;
            return *this;
        }
        const basic_iserializer * bis_ptr;
        const basic_pointer_iserializer * bpis_ptr;
        version_type file_version;
        tracking_type tracking_level;
        bool initialized;

        cobject_id(const basic_iserializer & bis_) :
            bis_ptr(& bis_),
            bpis_ptr(NULL),
            file_version(0),
            tracking_level(track_never),
            initialized(false)
        {}
        cobject_id(const cobject_id &rhs): 
            bis_ptr(rhs.bis_ptr),
            bpis_ptr(rhs.bpis_ptr),
            file_version(rhs.file_version),
            tracking_level(rhs.tracking_level),
            initialized(rhs.initialized)
        {}
    };
    typedef std::vector<cobject_id> cobject_id_vector_type;
    cobject_id_vector_type cobject_id_vector;

    //////////////////////////////////////////////////////////////////////
    // list of objects created by de-serialization.  Used to implement
    // clean up after exceptions.
    class created_pointer_type
    {
    public:
        created_pointer_type(
            class_id_type class_id_,
            void * address_
        ) :
            class_id(class_id_),
            address(address_)
        {}
        created_pointer_type(const created_pointer_type &rhs) :
            class_id(rhs.class_id),
            address(rhs.address)
        {}
        created_pointer_type & operator=(const created_pointer_type &){
            assert(false);
            return *this;
        }
        void * get_address() const {
            return address;
        }
        // object to which this item refers
        const class_id_type class_id;
    private:
        void * address;
    };

    std::list<created_pointer_type> created_pointers;

    //////////////////////////////////////////////////////////////////////
    // address of the most recent object serialized as a poiner
    // whose data itself is now pending serialization
    void * pending_object;
    const basic_iserializer * pending_bis;
    version_type pending_version;

    basic_iarchive_impl(unsigned int flags) :
        m_archive_library_version(ARCHIVE_VERSION()),
        m_flags(flags),
        moveable_objects_start(0),
        moveable_objects_end(0),
        pending_object(NULL),
        pending_bis(NULL),
        pending_version(0)
    {}
    ~basic_iarchive_impl(){}
    void set_library_version(unsigned int archive_library_version){
        m_archive_library_version = archive_library_version;
    }
    bool
    track(
        basic_iarchive & ar,
        void * & t
    );
    void
    load_preamble(
        basic_iarchive & ar,
        cobject_id & co
    );
    class_id_type register_type(
        const basic_iserializer & bis
    );

    // redirect through virtual functions to load functions for this archive
    template<class T>
    void load(basic_iarchive & ar, T & t){
        ar.vload(t);
    }

//public:
    void
    next_object_pointer(void * t){
        pending_object = t;
    }
    void delete_created_pointers();
    class_id_type register_type(
        const basic_pointer_iserializer & bpis
    );
    void load_object(
        basic_iarchive & ar,
        void * t,
        const basic_iserializer & bis
    );
    const basic_pointer_iserializer * load_pointer(
        basic_iarchive & ar,
        void * & t, 
        const basic_pointer_iserializer * bpis,
        const basic_pointer_iserializer * (*finder)(
            const boost::serialization::extended_type_info & type
        )
    );
};

inline void 
basic_iarchive_impl::reset_object_address(
    const void * new_address, 
    const void *old_address
){
    object_id_type i;
    // this code handles a couple of situations.
    // a) where reset_object_address is applied to an untracked object.
    //    In such a case the call is really superfluous and its really an
    //    an error.  But we don't have access to the types here so we can't
    //    know that.  However, this code will effectively turn this situation
    //    into a no-op and every thing will work fine - albeat with a small
    //    execution time penalty.
    // b) where the call to reset_object_address doesn't immediatly follow
    //    the << operator to which it corresponds.  This would be a bad idea
    //    but the code may work anyway.  Naturally, a bad practice on the part
    //    of the programmer but we can't detect it - as above.  So maybe we
    //    can save a few more people from themselves as above.
    for(i = moveable_objects_recent; i < moveable_objects_end; ++i){
        if(old_address == object_id_vector[i].address)
            break;
    }
    for(; i < moveable_objects_end; ++i){

        // calculate displacement from this level
        // warning - pointer arithmetic on void * is in herently non-portable
        // but expected to work on all platforms in current usage
        if(object_id_vector[i].address > old_address){
            std::size_t member_displacement
                = reinterpret_cast<std::size_t>(object_id_vector[i].address) 
                - reinterpret_cast<std::size_t>(old_address);
            object_id_vector[i].address = reinterpret_cast<void *>(
                reinterpret_cast<std::size_t>(new_address) + member_displacement
            );
        }
        else{
            std::size_t member_displacement
                = reinterpret_cast<std::size_t>(old_address)
                - reinterpret_cast<std::size_t>(object_id_vector[i].address); 
            object_id_vector[i].address = reinterpret_cast<void *>(
                reinterpret_cast<std::size_t>(new_address) - member_displacement
            );
       }
        ++i;
    }
}

inline void 
basic_iarchive_impl::delete_created_pointers()
{
    while(created_pointers.size() > 0){
        const created_pointer_type & cp = created_pointers.front();

        // figure out the class of the object to be deleted
        // note: extra line used to evade borland issue
        const int id = cp.class_id;
        const cobject_id & co = cobject_id_vector[id];
        // with the appropriate input serializer, 
        // delete the indicated object
        co.bis_ptr->destroy(cp.get_address());
        created_pointers.pop_front();
    }
}

inline class_id_type
basic_iarchive_impl::register_type(
    const basic_iserializer & bis
){
    class_id_type id(static_cast<int>(cobject_info_set.size()));
    cobject_type co(id, bis);
    std::pair<cobject_info_set_type::const_iterator, bool>
        result = cobject_info_set.insert(co);

    if(result.second){
        cobject_id_vector.push_back(cobject_id(bis));
        assert(cobject_info_set.size() == cobject_id_vector.size());
    }
    id = result.first->class_id;
    // borland complains without this minor hack
    const int tid = id;
    cobject_id & coid = cobject_id_vector[tid];
    coid.bpis_ptr = bis.get_bpis_ptr();
    return id;
}

void
basic_iarchive_impl::load_preamble(
    basic_iarchive & ar,
    cobject_id & co
){
    if(! co.initialized){
        if(co.bis_ptr->class_info()){
            class_id_optional_type cid;
            load(ar, cid);    // to be thrown away
            load(ar, co.tracking_level);
            load(ar, co.file_version);
        }
        else{
            // override tracking with indicator from class information
            co.tracking_level = co.bis_ptr->tracking(m_flags);
            co.file_version = version_type(
                co.bis_ptr->version()
            );
        }
        co.initialized = true;
    }
}

bool
basic_iarchive_impl::track(
    basic_iarchive & ar,
    void * & t
){
    object_id_type oid;
    load(ar, oid);

    // if its a reference to a old object
    if(object_id_type(object_id_vector.size()) > oid){
        // we're done
        t = object_id_vector[oid].address;
        return false;
    }
    return true;
}

inline void
basic_iarchive_impl::load_object(
    basic_iarchive & ar,
    void * t,
    const basic_iserializer & bis
){
    // if its been serialized through a pointer and the preamble's been done
    if(t == pending_object && & bis == pending_bis){
        // read data
        (bis.load_object_data)(ar, t, pending_version);
        return;
    }

    const class_id_type cid = register_type(bis);
    // note: extra line used to evade borland issue
    const int id = cid;
    cobject_id & co = cobject_id_vector[id];

    load_preamble(ar, co);

    // save the current move stack position in case we want to truncate it
    boost::state_saver<object_id_type> w(moveable_objects_start);

    // note: extra line used to evade borland issue
    const bool tracking = co.tracking_level;

    object_id_type this_id;
    moveable_objects_start =
    this_id = object_id_vector.size();

    // if we tracked this object when the archive was saved
    if(tracking){ 
        // if it was already read
        if(!track(ar, t))
            // we're done
            return;
        // add a new enty into the tracking list
        object_id_vector.push_back(aobject(t, cid));
        // and add an entry for this object
        moveable_objects_end = object_id_vector.size();
    }
    // read data
    (bis.load_object_data)(ar, t, co.file_version);
    moveable_objects_recent = this_id;
}

inline const basic_pointer_iserializer *
basic_iarchive_impl::load_pointer(
    basic_iarchive &ar,
    void * & t,
    const basic_pointer_iserializer * bpis_ptr,
    const basic_pointer_iserializer * (*finder)(
        const boost::serialization::extended_type_info & type_
    )
){
    class_id_type cid;
    load(ar, cid);

    if(NULL_POINTER_TAG == cid){
        t = NULL;
        return bpis_ptr;
    }

    // if its a new class type - i.e. never been registered
    if(class_id_type(cobject_info_set.size()) <= cid){
        // if its either abstract
        if(NULL == bpis_ptr
        // or polymorphic
        || bpis_ptr->get_basic_serializer().is_polymorphic()){
            // is must have been exported
            char key[BOOST_SERIALIZATION_MAX_KEY_SIZE];
            class_name_type class_name(key);
            load(ar, class_name);
            // if it has a class name
            const serialization::extended_type_info *eti = NULL;
            if(0 != key[0])
                eti = serialization::extended_type_info::find(key);
            if(NULL == eti)
                boost::throw_exception(
                    archive_exception(archive_exception::unregistered_class)
                );
            bpis_ptr = (*finder)(*eti);
        }
        assert(NULL != bpis_ptr);
        class_id_type new_cid = register_type(bpis_ptr->get_basic_serializer());
        int i = cid;
        cobject_id_vector[i].bpis_ptr = bpis_ptr;
        assert(new_cid == cid);
    }
    int i = cid;
    cobject_id & co = cobject_id_vector[i];
    bpis_ptr = co.bpis_ptr;

    load_preamble(ar, co);

    // extra line to evade borland issue
    const bool tracking = co.tracking_level;
    // if we're tracking and the pointer has already been read
    if(tracking && ! track(ar, t))
        // we're done
        return bpis_ptr;

    // save state
    state_saver<object_id_type> w(moveable_objects_start);

    if(! tracking){
        bpis_ptr->load_object_ptr(ar, t, co.file_version);
    }
    else{
        state_saver<void *> x(pending_object);
        state_saver<const basic_iserializer *> y(pending_bis);
        state_saver<version_type> z(pending_version);

        pending_bis = & bpis_ptr->get_basic_serializer();
        pending_version = co.file_version;

        // predict next object id to be created
        const unsigned int ui = object_id_vector.size();

        state_saver<object_id_type> w(moveable_objects_end);

        // because the following operation could move the items
        // don't use co after this
        // add to list of serialized objects so that we can properly handle
        // cyclic strucures
        object_id_vector.push_back(aobject(t, cid));

        bpis_ptr->load_object_ptr(
            ar, 
            object_id_vector[ui].address, 
            co.file_version
        );
        t = object_id_vector[ui].address;
        assert(NULL != t);

        // and add to list of created pointers
        created_pointers.push_back(created_pointer_type(cid, t));
    }

    return bpis_ptr;
}

//////////////////////////////////////////////////////////////////////
// implementation of basic_iarchive functions

BOOST_ARCHIVE_DECL(void)
basic_iarchive::next_object_pointer(void *t){
    pimpl->next_object_pointer(t);
}

BOOST_ARCHIVE_DECL(BOOST_PP_EMPTY())
basic_iarchive::basic_iarchive(unsigned int flags) : 
    pimpl(new basic_iarchive_impl(flags))
{}

BOOST_ARCHIVE_DECL(BOOST_PP_EMPTY())
basic_iarchive::~basic_iarchive()
{
    delete pimpl;
}

BOOST_ARCHIVE_DECL(void)
basic_iarchive::set_library_version(unsigned int archive_library_version){
    pimpl->set_library_version(archive_library_version);
}

BOOST_ARCHIVE_DECL(void)
basic_iarchive::reset_object_address(
    const void * new_address, 
    const void * old_address
){
    pimpl->reset_object_address(new_address, old_address);
}

BOOST_ARCHIVE_DECL(void)
basic_iarchive::load_object(
    void *t, 
    const basic_iserializer & bis
){
    pimpl->load_object(*this, t, bis);
}

// load a pointer object
BOOST_ARCHIVE_DECL(const basic_pointer_iserializer *)
basic_iarchive::load_pointer(
    void * &t, 
    const basic_pointer_iserializer * bpis_ptr,
    const basic_pointer_iserializer * (*finder)(
        const boost::serialization::extended_type_info & type_
    )
){
    return pimpl->load_pointer(*this, t, bpis_ptr, finder);
}

BOOST_ARCHIVE_DECL(void)
basic_iarchive::register_basic_serializer(const basic_iserializer & bis){
    pimpl->register_type(bis);
}

BOOST_ARCHIVE_DECL(void)
basic_iarchive::lookup_basic_helper(
    const boost::serialization::extended_type_info * const eti,
    shared_ptr<void> & sph
){
    pimpl->lookup_helper(eti, sph);
}

BOOST_ARCHIVE_DECL(void)
basic_iarchive::insert_basic_helper(
    const boost::serialization::extended_type_info * const eti,
    shared_ptr<void> & sph
){
    pimpl->insert_helper(eti, sph);
}

BOOST_ARCHIVE_DECL(void)
basic_iarchive::delete_created_pointers()
{
    pimpl->delete_created_pointers();
}

BOOST_ARCHIVE_DECL(unsigned int) 
basic_iarchive::get_library_version() const{
    return pimpl->m_archive_library_version;
}

BOOST_ARCHIVE_DECL(unsigned int) 
basic_iarchive::get_flags() const{
    return pimpl->m_flags;
}

} // namespace detail
} // namespace archive
} // namespace boost
