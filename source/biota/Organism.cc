/*
 *  biota/Organism.cc
 *  avida-core
 *
 *  Created by David on 1/25/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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

#include "avida/biota/Organism.h"

#include "avida/biota/Trait.h"
#include "avida/systematics/Group.h"


// Internal Organism Definitions
// --------------------------------------------------------------------------------------------------------------

namespace Avida {
  namespace Biota {
    namespace Internal {
      
      // Referenced external properties
      // --------------------------------------------------------------------------------------------------------------
      
      static const Apto::BasicString<Apto::ThreadSafe> s_ext_prop_name_instset("instset");
      
      
      // Internal Organism Properties
      // --------------------------------------------------------------------------------------------------------------
      
      static PropertyDescriptionMap s_prop_desc_map;
      
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_genome("genome");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_src_transmission_type("src_transmission_type");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_age("age");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_generation("generation");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_copied_size("last_copied_size");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_executed_size("last_exectuted_size");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_gestation_time("last_gestation_time");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_metabolic_rate("last_metabolic_rate");
      static const Apto::BasicString<Apto::ThreadSafe> s_prop_name_last_fitness("last_fitness");
      
      
      
      // OrgPropertyMap
      // --------------------------------------------------------------------------------------------------------------

      class OrgPropertyMap : public PropertyMap
      {
        friend class Organism;
      private:
        class OrgIntProp : public IntProperty
        {
        public:
          inline OrgIntProp(const PropertyDescriptionMap& desc_map) : IntProperty("", desc_map, 0) { ; }
          inline void SetPropertyID(const PropertyID& prop_id) { m_id = prop_id; }
        };
        
        class OrgDoubleProp : public DoubleProperty
        {
        public:
          inline OrgDoubleProp(const PropertyDescriptionMap& desc_map) : DoubleProperty("", desc_map, 0) { ; }
          inline void SetPropertyID(const PropertyID& prop_id) { m_id = prop_id; }
        };
        
        class OrgStringProp : public StringProperty
        {
        public:
          inline OrgStringProp(const PropertyDescriptionMap& desc_map) : StringProperty("", desc_map, Apto::String()) { ; }
          inline void SetPropertyID(const PropertyID& prop_id) { m_id = prop_id; }
        };
        
      private:
        Organism* m_organism;
        mutable OrgIntProp m_prop_int;
        mutable OrgDoubleProp m_prop_double;
        mutable OrgStringProp m_prop_string;
        
      public:
        LIB_LOCAL OrgPropertyMap(Organism* organism);
        LIB_LOCAL ~OrgPropertyMap();
        
        LIB_LOCAL int GetSize() const;
        
        LIB_LOCAL bool operator==(const PropertyMap& p) const;
        
        LIB_LOCAL bool Has(const PropertyID& p_id) const;
        
        LIB_LOCAL const Property& Get(const PropertyID& p_id) const;
        
        LIB_LOCAL bool SetValue(const PropertyID& p_id, const Apto::String& prop_value);
        LIB_LOCAL bool SetValue(const PropertyID& p_id, const int prop_value);
        LIB_LOCAL bool SetValue(const PropertyID& p_id, const double prop_value);
        
        
        LIB_LOCAL void Define(PropertyPtr p);
        LIB_LOCAL bool Remove(const PropertyID& p_id);
        
        LIB_LOCAL ConstPropertyIDSetPtr PropertyIDs() const;
        
        LIB_LOCAL bool Serialize(ArchivePtr ar) const;
        
        inline const Property& SetTempProp(const PropertyID& prop_id, int value) const
        {
          m_prop_int.SetPropertyID(prop_id); m_prop_int.SetValue(value); return m_prop_int;
        }
        inline const Property& SetTempProp(const PropertyID& prop_id, double value) const
        {
          m_prop_double.SetPropertyID(prop_id); m_prop_double.SetValue(value); return m_prop_double;
        }
        inline const Property& SetTempProp(const PropertyID& prop_id, const Apto::String& value) const
        {
          m_prop_string.SetPropertyID(prop_id); m_prop_string.SetValue(value); return m_prop_string;
        }
      };
      
      
      // OrgPropRetrievalContainer - base container class for the global property map
      // --------------------------------------------------------------------------------------------------------------
      
      class OrgPropRetrievalContainer
      {
      public:
        virtual ~OrgPropRetrievalContainer() { ; }
        
        virtual const Property& Get(Organism*, const OrgPropertyMap*) const = 0;
      };
      
      
      // OrgPropOfType - concrete implementations of OrgPropRetrievalContainer for each necessary type
      // --------------------------------------------------------------------------------------------------------------
      
      template <class T> class OrgPropOfType : public OrgPropRetrievalContainer
      {
      private:
        typedef T (Organism::*RetrieveFunction)();
        
        PropertyID m_prop_id;
        RetrieveFunction m_fun;
        
      public:
        OrgPropOfType(const PropertyID& prop_id, RetrieveFunction fun) : m_prop_id(prop_id), m_fun(fun) { ; }
        
        const Property& Get(Organism* org, const OrgPropertyMap* prop_map) const
        {
          return prop_map->SetTempProp(m_prop_id, (org->*m_fun)());
        }
      };
      
      
      // OrgGlobalPropMap and OrgGlobalPropMapSingletone - global singleton structure holding the global org prop map
      // --------------------------------------------------------------------------------------------------------------
      
      struct OrgGlobalPropMap
      {
        Apto::Map<Apto::String, OrgPropRetrievalContainer*> prop_map;
        
        ~OrgGlobalPropMap()
        {
          for (Apto::Map<Apto::String, OrgPropRetrievalContainer*>::ValueIterator it = prop_map.Values(); it.Next();) {
            delete *it.Get();
          }
        }
      };
      
      typedef Apto::SingletonHolder<OrgGlobalPropMap, Apto::CreateWithNew, Apto::DestroyAtExit, Apto::ThreadSafe> OrgGlobalPropMapSingleton;
    };
  };
};


// cOrganism::Intialize() - static method that sets up global data structures
// --------------------------------------------------------------------------------------------------------------

void Avida::Biota::Organism::Initialize()
{
#define DEFINE_PROP(NAME, TYPE, FUNCTION, DESC) Internal::s_prop_desc_map.Set(Internal::s_prop_name_ ## NAME, DESC); \
  Internal::OrgGlobalPropMapSingleton::Instance().prop_map.Set(Internal::s_prop_name_ ## NAME, \
  new Internal::OrgPropOfType<TYPE>(Internal::s_prop_name_ ## NAME, &Organism::FUNCTION));
  
  DEFINE_PROP(genome, Apto::String, getGenomeString, "Genome");
  DEFINE_PROP(src_transmission_type, int, getSrcTransmissionType, "Source Transmission Type");
  DEFINE_PROP(age, int, getAge, "Age");
  DEFINE_PROP(generation, int, getGeneration, "Generation");
  DEFINE_PROP(last_copied_size, int, getLastCopied, "Last Copied Size");
  DEFINE_PROP(last_executed_size, int, getLastExecuted, "Last Exectuted Size");
  DEFINE_PROP(last_gestation_time, int, getLastGestation, "Last Gestation Time");
  DEFINE_PROP(last_metabolic_rate, double, getLastMetabolicRate, "Last Metabolic Rage");
  DEFINE_PROP(last_fitness, double, getLastFitness, "Last Fitness");
  
#undef DEFINE_PROP
}


// Organism
// --------------------------------------------------------------------------------------------------------------

Avida::Biota::Organism::Organism(Universe* universe, const Genome& genome, Systematics::Source src, int parent_generation)
  : m_src(src), m_initial_genome(genome)
{
  
}


Avida::Biota::Organism::~Organism()
{
  for (int idx = 0; idx < m_traits.GetSize(); idx++) delete m_traits[idx];
}


Avida::Systematics::Source Avida::Biota::Organism::UnitSource() const { return m_src; }

const Avida::Genome& Avida::Biota::Organism::UnitGenome() const { return m_initial_genome; }

const Avida::PropertyMap& Avida::Biota::Organism::Properties() const { return *m_prop_map; }


// Internal::OrgPropertyMap implementation
// --------------------------------------------------------------------------------------------------------------

Avida::Biota::Internal::OrgPropertyMap::OrgPropertyMap(Organism* organism)
: m_organism(organism), m_prop_int(s_prop_desc_map), m_prop_double(s_prop_desc_map), m_prop_string(s_prop_desc_map) { ; }

Avida::Biota::Internal::OrgPropertyMap::~OrgPropertyMap() { ; }

int Avida::Biota::Internal::OrgPropertyMap::GetSize() const
{
  return OrgGlobalPropMapSingleton::Instance().prop_map.GetSize();
}

bool Avida::Biota::Internal::OrgPropertyMap::Has(const PropertyID& p_id) const
{
  return OrgGlobalPropMapSingleton::Instance().prop_map.Has(p_id);
}

const Avida::Property& Avida::Biota::Internal::OrgPropertyMap::Get(const PropertyID& p_id) const
{
  OrgPropRetrievalContainer* container = NULL;
  if (OrgGlobalPropMapSingleton::Instance().prop_map.Get(p_id, container)) {
    return container->Get(m_organism, this);
  }
  
  return *s_default_prop;
}


bool Avida::Biota::Internal::OrgPropertyMap::SetValue(const PropertyID& p_id, const Apto::String& prop_value) { return false; }
bool Avida::Biota::Internal::OrgPropertyMap::SetValue(const PropertyID& p_id, const int prop_value) { return false; }
bool Avida::Biota::Internal::OrgPropertyMap::SetValue(const PropertyID& p_id, const double prop_value) { return false; }


bool Avida::Biota::Internal::OrgPropertyMap::operator==(const PropertyMap& p) const
{
  // Build distinct key sets
  Apto::Set<PropertyID> pm1pids, pm2pids;
  Apto::Map<PropertyID, OrgPropRetrievalContainer*>::KeyIterator it = OrgGlobalPropMapSingleton::Instance().prop_map.Keys();
  while (it.Next()) pm1pids.Insert(*it.Get());
  
  PropertyIDSet::ConstIterator pidit = p.PropertyIDs()->Begin();
  while (pidit.Next()) pm2pids.Insert(*pidit.Get());
  
  // Compare key sets
  if (pm1pids != pm2pids) return false;
  
  // Compare values
  it = OrgGlobalPropMapSingleton::Instance().prop_map.Keys();
  while (it.Next()) {
    OrgPropRetrievalContainer* container = NULL;
    if (OrgGlobalPropMapSingleton::Instance().prop_map.Get(*it.Get(), container)) {
      if (container->Get(m_organism, this) != p.Get(*it.Get())) return false;
    } else {
      return false;
    }
  }
  
  return true;
}

void Avida::Biota::Internal::OrgPropertyMap::Define(PropertyPtr p) { ; }
bool Avida::Biota::Internal::OrgPropertyMap::Remove(const PropertyID& p_id) { return false; }

Avida::ConstPropertyIDSetPtr Avida::Biota::Internal::OrgPropertyMap::PropertyIDs() const
{
  PropertyIDSetPtr pidset(new PropertyIDSet);
  
  Apto::Map<PropertyID, OrgPropRetrievalContainer*>::KeyIterator it = OrgGlobalPropMapSingleton::Instance().prop_map.Keys();
  while (it.Next()) pidset->Insert(*it.Get());
  
  return pidset;
}


bool Avida::Biota::Internal::OrgPropertyMap::Serialize(ArchivePtr) const
{
  // @TODO
  assert(false);
  return false;
}



// Property Map Retrieival Functions
// --------------------------------------------------------------------------------------------------------------

Apto::String Avida::Biota::Organism::getGenomeString() { return m_initial_genome.AsString(); }
int Avida::Biota::Organism::getSrcTransmissionType() { return m_src.transmission_type; }
int Avida::Biota::Organism::getAge() { assert(false); return 0; /*return m_phenotype.GetAge();*/ }
int Avida::Biota::Organism::getGeneration() { assert(false); return 0; /*return m_phenotype.GetGeneration();*/ }
int Avida::Biota::Organism::getLastCopied() { assert(false); return 0; /*return m_phenotype.GetCopiedSize();*/ }
int Avida::Biota::Organism::getLastExecuted() { assert(false); return 0; /*return m_phenotype.GetExecutedSize();*/ }
int Avida::Biota::Organism::getLastGestation() { assert(false); return 0; /*return m_phenotype.GetGestationTime();*/ }
double Avida::Biota::Organism::getLastMetabolicRate() { assert(false); return 0; /*return m_phenotype.GetLastMerit();*/ }
double Avida::Biota::Organism::getLastFitness() { assert(false); return 0; /*return m_phenotype.GetFitness();*/ }

