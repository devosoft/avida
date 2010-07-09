/*
 *  cArgSchema.h
 *  Avida
 *
 *  Created by David Bryson on 9/12/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cArgSchema_h
#define cArgSchema_h

#ifndef tDictionary_h
#include "tDictionary.h"
#endif


class cArgSchema
{
public:
  enum tType { SCHEMA_INT, SCHEMA_DOUBLE, SCHEMA_STRING };

private:
  struct sArgSchemaEntry
  {
    cString name;
    tType type;
    int index;
    bool optional;
    union {
      int def_int;
      double def_double;
      cString* def_string;
    };
    bool has_range_limits;
    union {
      int r_l_int;
      double r_l_double;
    };
    union {
      int r_u_int;
      double r_u_double;
    };
    
    
    sArgSchemaEntry() { ; }
    sArgSchemaEntry(const cString& in_name, int in_idx, tType in_type)  // Required Argument (supplied type)
      : name(in_name), type(in_type), index(in_idx), optional(false), has_range_limits(false) { ; }
    sArgSchemaEntry(const cString& in_name, int in_idx, int def)        // Optional Int Argument
      : name(in_name), type(SCHEMA_INT), index(in_idx), optional(true), def_int(def), has_range_limits(false) { ; }
    sArgSchemaEntry(const cString& in_name, int in_idx, double def)     // Optional Double Argument
      : name(in_name), type(SCHEMA_DOUBLE), index(in_idx), optional(true), def_double(def), has_range_limits(false) { ; }
    sArgSchemaEntry(const cString& in_name, int in_idx, cString* def)   // Optional String Argument
      : name(in_name), type(SCHEMA_STRING), index(in_idx), optional(true), def_string(def), has_range_limits(false) { ; }
    ~sArgSchemaEntry() { if (type == SCHEMA_STRING && optional) delete def_string; }  // Cleanup string object
  };
  
  
  tDictionary<sArgSchemaEntry*> m_entries;
  tArray<sArgSchemaEntry*> m_ints;
  tArray<sArgSchemaEntry*> m_doubles;
  tArray<sArgSchemaEntry*> m_strings;
  
  char m_sep_entry;
  char m_sep_value;
  
  bool m_case_sensitive;

  
public:
  cArgSchema(char entry = ',', char value = '=', bool case_sensitive = false)
    : m_sep_entry(entry), m_sep_value(value), m_case_sensitive(case_sensitive) { ; }
  ~cArgSchema();
  
  inline void AdjustArgName(cString& in_name) const;

  char GetEntrySeparator() const { return m_sep_entry; }
  char GetValueSeparator() const { return m_sep_value; }
  bool IsCaseSensitive() const { return m_case_sensitive; }
  
  bool AddEntry(cString in_name, int in_idx, tType in_type);                  // Required Argument (supplied type)
  bool AddEntry(cString in_name, int in_idx, int def);                         // Optional Int Argument
  bool AddEntry(cString in_name, int in_idx, int lower, int upper);           // Required Int Argument (with range limits)
  bool AddEntry(cString in_name, int in_idx, int lower, int upper, int def);  // Optional Int Argument (with range limits)
  bool AddEntry(cString in_name, int in_idx, double def);                     // Optional Double Argument
  bool AddEntry(cString in_name, int in_idx, double lower, double upper);     // Required Double Argument (with range limits)
  bool AddEntry(cString in_name, int in_idx, double lower, double upper, double def); // Optional Double (with range limits)
  bool AddEntry(cString in_name, int in_idx, const cString& def);             // Optional String Argument
  
  bool FindEntry(const cString& in_name, tType& ret_type, int& ret_idx) const;
  
  inline int GetNumIntArgs() const { return m_ints.GetSize(); }
  inline int GetNumDoubleArgs() const { return m_doubles.GetSize(); }
  inline int GetNumStringArgs() const { return m_strings.GetSize(); }
  
  inline bool IsOptionalInt(int i) const;
  inline bool IsOptionalDouble(int i) const;
  inline bool IsOptionalString(int i) const;
  
  inline void SetDefaultInt(int i, int& v) const;
  inline void SetDefaultDouble(int i, double& v) const;
  inline void SetDefaultString(int i, cString& v) const;
  
  inline bool GetIntName(int i, cString& name) const;
  inline bool GetDoubleName(int i, cString& name) const;
  inline bool GetStringName(int i, cString& name) const;
  
  inline bool ValidateInt(int i, int v) const;
  inline bool ValidateDouble(int i, double v) const;
};


inline void cArgSchema::AdjustArgName(cString& in_name) const
{
  in_name.Trim();
  if (!m_case_sensitive) in_name.ToLower();
}


inline void cArgSchema::SetDefaultInt(int i, int& v) const
{
  if (IsOptionalInt(i)) v = m_ints[i]->def_int;
}

inline void cArgSchema::SetDefaultDouble(int i, double& v) const
{
  if (IsOptionalDouble(i)) v = m_doubles[i]->def_double;
}

inline void cArgSchema::SetDefaultString(int i, cString& v) const
{
  if (IsOptionalString(i)) v = *m_strings[i]->def_string;
}


inline bool cArgSchema::IsOptionalInt(int i) const
{
  if (i < m_ints.GetSize() && m_ints[i]) return m_ints[i]->optional;
  return false;
}

inline bool cArgSchema::IsOptionalDouble(int i) const
{
  if (i < m_doubles.GetSize() && m_doubles[i]) return m_doubles[i]->optional;
  return false;
}

inline bool cArgSchema::IsOptionalString(int i) const
{
  if (i < m_strings.GetSize() && m_strings[i]) return m_strings[i]->optional;
  return false;
}


inline bool cArgSchema::GetIntName(int i, cString& name) const
{
  if (i < m_ints.GetSize() && m_ints[i]) {
    name = m_ints[i]->name;
    return true;
  }
  return false;
}

inline bool cArgSchema::GetDoubleName(int i, cString& name) const
{
  if (i < m_doubles.GetSize() && m_doubles[i]) {
    name = m_doubles[i]->name;
    return true;
  }
  return false;
}

inline bool cArgSchema::GetStringName(int i, cString& name) const
{
  if (i < m_strings.GetSize() && m_strings[i]) {
    name = m_strings[i]->name;
    return true;
  }
  return false;
}

inline bool cArgSchema::ValidateInt(int i, int v) const
{
  if (i < m_ints.GetSize() && m_ints[i] &&
      (!m_ints[i]->has_range_limits || (v <= m_ints[i]->r_u_int && v >= m_ints[i]->r_l_int))) {
    return true;
  }
  
  return false;
}

inline bool cArgSchema::ValidateDouble(int i, double v) const
{
  if (i < m_doubles.GetSize() && m_doubles[i] &&
      (!m_doubles[i]->has_range_limits || (v <= m_doubles[i]->r_u_double && v >= m_doubles[i]->r_l_double))) {
    return true;
  }
  
  return false;
}


#endif
