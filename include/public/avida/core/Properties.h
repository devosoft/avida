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

#include <typeinfo>


namespace Avida {
  
  // Property
  // --------------------------------------------------------------------------------------------------------------  
  
  class Property : public Apto::ClassAllocator<SmallObjectMalloc>
  {
  public:
    static PropertyTypeID Null;
    
  protected:
    PropertyID m_id;
    const PropertyTypeID& m_type_id;
    const PropertyDescriptionMap& m_desc;
    

  public:
    LIB_EXPORT inline Property(const PropertyID& p_id, const PropertyTypeID& t_id, const PropertyDescriptionMap& desc)
      : m_id(p_id), m_type_id(t_id), m_desc(desc) { ; }
    LIB_EXPORT inline Property(const Property& p) : Apto::ClassAllocator<SmallObjectMalloc>(p), m_id(p.m_id), m_type_id(p.m_type_id), m_desc(p.m_desc) { ; }
    LIB_EXPORT virtual ~Property() = 0;
    
    LIB_EXPORT inline const PropertyID& ID() const { return m_id; }
    LIB_EXPORT inline const PropertyTypeID& Type() const { return m_type_id; }
    LIB_EXPORT inline const Apto::String& Description() const { return m_desc.Get(m_id); }
    
    LIB_EXPORT virtual Apto::String StringValue() const = 0;
    LIB_EXPORT virtual int IntValue() const = 0;
    LIB_EXPORT virtual double DoubleValue() const = 0;
    LIB_EXPORT virtual bool SetValue(const Apto::String& value) = 0;
    LIB_EXPORT virtual bool SetValue(const int value) = 0;
    LIB_EXPORT virtual bool SetValue(const double value) = 0;
    
    LIB_EXPORT inline operator Apto::String() const { return StringValue(); }
    LIB_EXPORT inline operator int() const { return IntValue(); }
    LIB_EXPORT inline operator double() const { return DoubleValue(); }
    
  private:
    LIB_EXPORT virtual bool isEqual(const Property& rhs) const = 0;
    LIB_EXPORT friend bool operator==(const Property& lhs, const Property& rhs);
  };
    
  LIB_EXPORT inline bool operator==(const Property& lhs, const Property& rhs)
  {
    return (lhs.StringValue() == rhs.StringValue());
  }

  LIB_EXPORT inline bool operator!=(const Property& lhs, const Property& rhs) { return !(lhs == rhs); }
  

  
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

  
  // FunctorProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  template <typename T> class FunctorProperty : public Property
  {
  public:
    typedef Apto::Functor<T, Apto::NullType, SmallObjectMalloc> GetFunctor;
    typedef Apto::Functor<bool, Apto::TL::Create<const T&>, SmallObjectMalloc> SetFunctor;
  private:
    GetFunctor m_g_fun;
    SetFunctor m_s_fun;
    
  public:
    LIB_EXPORT FunctorProperty(const Avida::PropertyID& prop_id, const PropertyDescriptionMap& desc, GetFunctor gfun, SetFunctor sfun = SetFunctor(noSet))
      : Property(prop_id, PropertyTraits<T>::Type, desc), m_g_fun(gfun), m_s_fun(sfun) { ; }
    
    LIB_EXPORT Apto::String StringValue() const { return Apto::AsStr(m_g_fun()); }
    LIB_EXPORT int IntValue() const { return m_g_fun(); }
    LIB_EXPORT double DoubleValue() const { return m_g_fun(); }
    LIB_EXPORT bool SetValue(const Apto::String& value) { return m_s_fun(Apto::StrAs(value)); }
    LIB_EXPORT bool SetValue(const int value) { return m_s_fun(value); }
    LIB_EXPORT bool SetValue(const double value) { return m_s_fun(value); }
    
  private:
    LIB_EXPORT bool isEqual(const Property& rhs) const { return dynamic_cast<const FunctorProperty&>(rhs).m_g_fun() == m_g_fun(); }
    LIB_EXPORT static bool noSet(const T&) { return false; }
  };

    
  template <> class FunctorProperty<Apto::String> : public Property
  {
  public:
    typedef Apto::Functor<Apto::String, Apto::NullType, SmallObjectMalloc> GetFunctor;
    typedef Apto::Functor<bool, Apto::TL::Create<const Apto::String&>, SmallObjectMalloc> SetFunctor;
  private:
    GetFunctor m_g_fun;
    SetFunctor m_s_fun;
    
  public:
    LIB_EXPORT FunctorProperty(const Avida::PropertyID& prop_id, const PropertyDescriptionMap& desc, GetFunctor gfun, SetFunctor sfun = SetFunctor(noSet))
      : Property(prop_id, PropertyTraits<Apto::String>::Type, desc), m_g_fun(gfun), m_s_fun(sfun) { ; }
    
    LIB_EXPORT Apto::String StringValue() const { return Apto::AsStr(m_g_fun()); }
    LIB_EXPORT int IntValue() const { return Apto::StrAs(m_g_fun()); }
    LIB_EXPORT double DoubleValue() const { return Apto::StrAs(m_g_fun()); }
    LIB_EXPORT bool SetValue(const Apto::String& value) { return m_s_fun(value); }
    LIB_EXPORT bool SetValue(const int value) { return m_s_fun(Apto::AsStr(value)); }
    LIB_EXPORT bool SetValue(const double value) { return m_s_fun(Apto::AsStr(value)); }
    
  private:
    LIB_EXPORT bool isEqual(const Property& rhs) const { return m_g_fun() == rhs.StringValue(); }
    LIB_EXPORT static bool noSet(const Apto::String&) { return false; }
  };
    
  
  // ReferenceProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  template <typename T> class ReferenceProperty : public Property
  {
  private:
    T& m_value_ref;
    
  public:
    LIB_EXPORT ReferenceProperty(const Avida::PropertyID& prop_id, const PropertyDescriptionMap& desc, T& value_ref)
    : Property(prop_id, PropertyTraits<T>::Type, desc), m_value_ref(value_ref) { ; }
    
    LIB_EXPORT Apto::String StringValue() const { return Apto::AsStr(m_value_ref); }
    LIB_EXPORT int IntValue() const { return m_value_ref; }
    LIB_EXPORT double DoubleValue() const { return m_value_ref; }
    LIB_EXPORT bool SetValue(const Apto::String& value) { m_value_ref = Apto::StrAs(value); return true; }
    LIB_EXPORT bool SetValue(const int value) { m_value_ref = value; return true; }
    LIB_EXPORT bool SetValue(const double value) { m_value_ref = value; return true; }
    
  private:
    LIB_EXPORT bool isEqual(const Property& rhs) const { return dynamic_cast<const ReferenceProperty&>(rhs).m_value_ref == m_value_ref; }
  };

  
  template <> class ReferenceProperty<Apto::String> : public Property
  {
  private:
    Apto::String& m_value_ref;
    
  public:
    LIB_EXPORT ReferenceProperty(const Avida::PropertyID& prop_id, const PropertyDescriptionMap& desc, Apto::String& value_ref)
    : Property(prop_id, PropertyTraits<Apto::String>::Type, desc), m_value_ref(value_ref) { ; }
    
    LIB_EXPORT Apto::String StringValue() const { return Apto::AsStr(m_value_ref); }
    LIB_EXPORT int IntValue() const { return Apto::StrAs(m_value_ref); }
    LIB_EXPORT double DoubleValue() const { return Apto::StrAs(m_value_ref); }
    LIB_EXPORT bool SetValue(const Apto::String& value) { m_value_ref = Apto::StrAs(value); return true; }
    LIB_EXPORT bool SetValue(const int value) { m_value_ref = Apto::AsStr(value); return true; }
    LIB_EXPORT bool SetValue(const double value) { m_value_ref = Apto::AsStr(value); return true; }
    
  private:
    LIB_EXPORT bool isEqual(const Property& rhs) const { return dynamic_cast<const ReferenceProperty<Apto::String>&>(rhs).m_value_ref == m_value_ref; }
  };
    


  // StringProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  class StringProperty : public Property
  {
  private:
    Apto::String m_value;
    
  public:
    LIB_EXPORT inline explicit StringProperty(const Property& p) : Property(p), m_value(p.StringValue()) { ; }
    template <typename T> StringProperty(const Avida::PropertyID& prop_id, const PropertyDescriptionMap& desc, const T& prop_value)
      : Property(prop_id, PropertyTraits<T>::Type, desc), m_value(Apto::AsStr(prop_value)) { ; }
    LIB_EXPORT inline StringProperty(const Avida::PropertyID& prop_id, const PropertyTypeID& type_id, const PropertyDescriptionMap& desc, const Apto::String& prop_value)
      : Property(prop_id, type_id, desc), m_value(prop_value) { ; }
    
    LIB_EXPORT Apto::String StringValue() const;
    LIB_EXPORT int IntValue() const;
    LIB_EXPORT double DoubleValue() const;
    
    LIB_EXPORT bool SetValue(const Apto::String& value);
    LIB_EXPORT bool SetValue(const int value);
    LIB_EXPORT bool SetValue(const double value);    
    
  private:
    LIB_EXPORT bool isEqual(const Property& rhs) const;
  };


  
  // IntProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  class IntProperty : public Property
  {
  private:
    int m_value;
    
  public:
    LIB_EXPORT inline explicit IntProperty(const Property& p) : Property(p), m_value(p.IntValue()) { ; }
    template <typename T> IntProperty(const Avida::PropertyID& prop_id, const PropertyDescriptionMap& desc, const T& prop_value)
      : Property(prop_id, PropertyTraits<T>::Type, desc), m_value(prop_value) { ; }
    LIB_EXPORT inline IntProperty(const Avida::PropertyID& prop_id, const PropertyTypeID& type_id, const PropertyDescriptionMap& desc, int prop_value)
      : Property(prop_id, type_id, desc), m_value(prop_value) { ; }
    
    LIB_EXPORT Apto::String StringValue() const;
    LIB_EXPORT int IntValue() const;
    LIB_EXPORT double DoubleValue() const;
    
    LIB_EXPORT bool SetValue(const Apto::String& value);
    LIB_EXPORT bool SetValue(const int value);
    LIB_EXPORT bool SetValue(const double value);    
    
  private:
    LIB_EXPORT bool isEqual(const Property& rhs) const;
  };


  
  // DoubleProperty
  // --------------------------------------------------------------------------------------------------------------  
  
  class DoubleProperty : public Property
  {
  private:
    double m_value;
    
  public:
    LIB_EXPORT inline explicit DoubleProperty(const Property& p) : Property(p), m_value(p.DoubleValue()) { ; }
    template <typename T> DoubleProperty(const Avida::PropertyID& prop_id, const PropertyDescriptionMap& desc, const T& prop_value)
      : Property(prop_id, PropertyTraits<T>::Type, desc), m_value(prop_value) { ; }
    LIB_EXPORT inline DoubleProperty(const Avida::PropertyID& prop_id, const PropertyTypeID& type_id, const PropertyDescriptionMap& desc, double prop_value)
      : Property(prop_id, type_id, desc), m_value(prop_value) { ; }
    
    LIB_EXPORT Apto::String StringValue() const;
    LIB_EXPORT int IntValue() const;
    LIB_EXPORT double DoubleValue() const;
    
    LIB_EXPORT bool SetValue(const Apto::String& value);
    LIB_EXPORT bool SetValue(const int value);
    LIB_EXPORT bool SetValue(const double value);    
    
  private:
    LIB_EXPORT bool isEqual(const Property& rhs) const;
  };


  
  // PropertyMap
  // --------------------------------------------------------------------------------------------------------------  
  
  class PropertyMap
  {
  protected:
    static PropertyPtr s_default_prop;
    static PropertyDescriptionMap s_null_desc_map;

  public:
    LIB_EXPORT inline PropertyMap() { ; }
    LIB_EXPORT virtual ~PropertyMap();
    
    LIB_EXPORT virtual int GetSize() const = 0;
    
    LIB_EXPORT virtual bool operator==(const PropertyMap& p) const = 0;
    LIB_EXPORT inline bool operator!=(const PropertyMap& p) const { return !operator==(p); }
    
    LIB_EXPORT virtual bool Has(const PropertyID& p_id) const = 0;
    
    LIB_EXPORT virtual const Property& Get(const PropertyID& p_id) const = 0;
    LIB_EXPORT inline const Property& operator[](const PropertyID& p_id) const { return Get(p_id); }
    
    LIB_EXPORT virtual bool SetValue(const PropertyID& p_id, const Apto::String& prop_value) = 0;
    LIB_EXPORT virtual bool SetValue(const PropertyID& p_id, const int prop_value) = 0;
    LIB_EXPORT virtual bool SetValue(const PropertyID& p_id, const double prop_value) = 0;
    
    
    LIB_EXPORT virtual void Define(PropertyPtr p) = 0;
    LIB_EXPORT virtual bool Remove(const PropertyID& p_id) = 0;
    
    LIB_EXPORT virtual ConstPropertyIDSetPtr PropertyIDs() const = 0;
    
    LIB_EXPORT virtual bool Serialize(ArchivePtr ar) const = 0;
    
  private:
    // Disallow copying
    PropertyMap(const PropertyMap&);
    PropertyMap& operator=(const PropertyMap&);
  };

  
  // HashPropertyMap
  // --------------------------------------------------------------------------------------------------------------
  
  class HashPropertyMap : public PropertyMap
  {
    template <class K, class V> class PropertyMapStorage
    : public Apto::HashStaticTableLinkedList<K, V, 5, Apto::HashKey, SmallObjectMalloc> { ; };
    
  private:
    Apto::Map<PropertyID, PropertyPtr, PropertyMapStorage, Apto::ExplicitDefault> m_prop_map;
    
  public:
    LIB_EXPORT inline HashPropertyMap() { ; }
    LIB_EXPORT ~HashPropertyMap();
    
    LIB_EXPORT int GetSize() const;
    
    LIB_EXPORT bool operator==(const PropertyMap& p) const;
    
    LIB_EXPORT bool Has(const PropertyID& p_id) const;
    
    LIB_EXPORT const Property& Get(const PropertyID& p_id) const;
    
    LIB_EXPORT bool SetValue(const PropertyID& p_id, const Apto::String& prop_value);
    LIB_EXPORT bool SetValue(const PropertyID& p_id, const int prop_value);
    LIB_EXPORT bool SetValue(const PropertyID& p_id, const double prop_value);
    
    
    LIB_EXPORT void Define(PropertyPtr p);
    LIB_EXPORT bool Remove(const PropertyID& p_id);
    
    LIB_EXPORT ConstPropertyIDSetPtr PropertyIDs() const;
    
    LIB_EXPORT bool Serialize(ArchivePtr ar) const;    
  };

};

#endif
