/*
 *  core/Properties.h
 *  avida-core
 *
 *  Created by David on 12/7/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaCoreProperties_h
#define AvidaCoreProperties_h

#include "apto/platform.h"
#include "apto/core/StringUtils.h"
#include "avida/core/Types.h"


namespace Avida {
  
  // Property
  // --------------------------------------------------------------------------------------------------------------  
  
  class Property
  {
  public:
    static PropertyTypeID Null;
    
  protected:
    PropertyID m_id;
    const PropertyTypeID& m_type_id;
    Apto::String m_desc;
    

  public:
    LIB_EXPORT inline Property(const PropertyID& p_id, const PropertyTypeID& t_id, const Apto::String& desc)
      : m_id(p_id), m_type_id(t_id), m_desc(desc) { ; }
    LIB_EXPORT virtual ~Property();
    
    LIB_EXPORT inline const PropertyID& ID() const { return m_id; }
    LIB_EXPORT inline const PropertyTypeID& Type() const { return m_type_id; }
    LIB_EXPORT inline const Apto::String& Description() const { return m_desc; }
    LIB_EXPORT virtual Apto::String Value() const;
    LIB_EXPORT virtual bool SetValue(const Apto::String& value);
    
    LIB_EXPORT inline operator Apto::String() const { return Value(); }
  };


  // FunctorProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  template <typename T> class FunctorProperty : public Property
  {
  public:
    typedef Apto::Functor<T, Apto::NullType> GetFunctor;
    typedef Apto::Functor<bool, Apto::TL::Create<const Apto::String&> > SetFunctor;
  private:
    GetFunctor m_g_fun;
    SetFunctor m_s_fun;
    
  public:
    LIB_EXPORT FunctorProperty(const Avida::PropertyID& prop_id, const Apto::String& desc, GetFunctor gfun, SetFunctor sfun = SetFunctor(noSet))
      : Property(prop_id, PropertyTraits<T>::Type, desc), m_g_fun(gfun), m_s_fun(sfun) { ; }
    
    LIB_EXPORT Apto::String Value() const { return Apto::AsStr(m_g_fun()); }
    LIB_EXPORT bool SetValue(const Apto::String& value) { return m_s_fun(value); }
    
  private:
    static bool noSet(const Apto::String&) { return false; }
  };
  
  
  // ReferenceProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  template <typename T> class ReferenceProperty : public Property
  {
  private:
    T& m_value_ref;
    
  public:
    LIB_EXPORT ReferenceProperty(const Avida::PropertyID& prop_id, const Apto::String& desc, T& value_ref)
    : Property(prop_id, PropertyTraits<T>::Type, desc), m_value_ref(value_ref) { ; }
    
    LIB_EXPORT Apto::String Value() const { return Apto::AsStr(m_value_ref); }
    LIB_EXPORT bool SetValue(const Apto::String& value) { m_value_ref = Apto::StrAs(value); return true; }
  };
  

  // StringProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  class StringProperty : public Property
  {
  private:
    Apto::String m_value;
    
  public:
    LIB_EXPORT inline explicit StringProperty(const Property& p) : Property(p.ID(), p.Type(), p.Description()), m_value(p.Value()) { ; }
    template <typename T> LIB_EXPORT StringProperty(const Avida::PropertyID& prop_id, const Apto::String& desc, const T& prop_value)
      : Property(prop_id, PropertyTraits<T>::Type, desc), m_value(Apto::AsStr(prop_value)) { ; }
    LIB_EXPORT inline StringProperty(const Avida::PropertyID& prop_id, const PropertyTypeID& type_id, const Apto::String& desc, const Apto::String& prop_value)
      : Property(prop_id, type_id, desc), m_value(prop_value) { ; }
    
    LIB_EXPORT Apto::String Value() const;
    LIB_EXPORT bool SetValue(const Apto::String& value);
  };


  
  // PropertyTraits Specializations
  // --------------------------------------------------------------------------------------------------------------
  
  template <> struct PropertyTraits<bool>
  {
    static const PropertyTypeID Type;
  };
  
  template <> struct PropertyTraits<int>
  {
    static const PropertyTypeID Type;
  };
  
  template <> struct PropertyTraits<double>
  {
    static const PropertyTypeID Type;
  };
  
  template <> struct PropertyTraits<const char*>
  {
    static const PropertyTypeID Type;
  };

  template <> struct PropertyTraits<Apto::String>
  {
    static const PropertyTypeID Type;
  };

  
  // PropertyMap
  // --------------------------------------------------------------------------------------------------------------  
  
  class PropertyMap
  {
  public:
    typedef Apto::Map<PropertyID, PropertyPtr, Apto::DefaultHashBTree, Apto::ExplicitDefault>::KeyIterator PropertyIDIterator;
    
  private:
    Apto::Map<PropertyID, PropertyPtr, Apto::DefaultHashBTree, Apto::ExplicitDefault> m_prop_map;
    PropertyPtr m_default;

  public:
    LIB_EXPORT inline PropertyMap() : m_default(new Property("", Property::Null, "")) { ; }
    
    LIB_EXPORT inline int GetSize() const { return m_prop_map.GetSize(); }
    
    LIB_EXPORT bool operator==(const PropertyMap& p) const;
    LIB_EXPORT inline bool operator!=(const PropertyMap& p) const { return !operator==(p); } 
    
    LIB_EXPORT inline bool Has(const PropertyID& p_id) const { return m_prop_map.Has(p_id); }
    
    LIB_EXPORT inline Property& Get(const PropertyID& p_id) { return *m_prop_map.GetWithDefault(p_id, PropertyPtr(new StringProperty(p_id, "", (const char*)""))); }
    LIB_EXPORT inline const Property& Get(const PropertyID& p_id) const { return *m_prop_map.GetWithDefault(p_id, m_default); }
    LIB_EXPORT inline Property& operator[](const PropertyID& p_id) { return Get(p_id); }
    LIB_EXPORT inline const Property& operator[](const PropertyID& p_id) const { return Get(p_id); }
    
    LIB_EXPORT inline void Set(PropertyPtr p) { m_prop_map.Set(p->ID(), p); }
    LIB_EXPORT inline bool Remove(const PropertyID& p_id) { return m_prop_map.Remove(p_id); }
    
    LIB_EXPORT PropertyIDIterator PropertyIDs() const { return m_prop_map.Keys(); }
    
    LIB_EXPORT bool Serialize(ArchivePtr ar) const;
    
  private:
    // Disallow copying
    PropertyMap(const PropertyMap&);
    PropertyMap& operator=(const PropertyMap&);
  };

};

#endif
