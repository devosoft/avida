/*
 *  util/ArgParser.h
 *  avida-core
 *
 *  Created by David Bryson on 9/12/06.
 *  Copyright 2006-2013 Michigan State University. All rights reserved.
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

#ifndef AvidaUtilArgParser_h
#define AvidaUtilArgParser_h

#include "avida/core/Types.h"


namespace Avida {
  namespace Util {
    
    // Type Declarations
    // --------------------------------------------------------------------------------------------------------------
    
    enum ArgumentType { INT, DOUBLE, STRING, NONE };
    
    class ArgSchema;
    

    // ArgSchema
    // --------------------------------------------------------------------------------------------------------------
    
    class ArgSchema
    {
    private:
      struct Entry;
      
    private:      
      Apto::Map<Apto::String, Entry*> m_entries;
      Apto::Array<Entry*> m_ints;
      Apto::Array<Entry*> m_doubles;
      Apto::Array<Entry*> m_strings;
      
      bool m_case_sensitive;
      
      
    public:
      ArgSchema(bool case_sensitive = false) : m_case_sensitive(case_sensitive) { ; }
      ~ArgSchema();
      
      inline void AdjustArgName(Apto::String& in_name) const;
      
      bool IsCaseSensitive() const { return m_case_sensitive; }
      
      int Define(Apto::String in_name, ArgumentType in_type);                  // Required Argument (supplied type)
      int Define(Apto::String in_name, int def);                         // Optional Int Argument
      int Define(Apto::String in_name, int lower, int upper);           // Required Int Argument (with range limits)
      int Define(Apto::String in_name, int lower, int upper, int def);  // Optional Int Argument (with range limits)
      int Define(Apto::String in_name, double def);                     // Optional Double Argument
      int Define(Apto::String in_name, double lower, double upper);     // Required Double Argument (with range limits)
      int Define(Apto::String in_name, double lower, double upper, double def); // Optional Double (with range limits)
      int Define(Apto::String in_name, const Apto::String& def);             // Optional String Argument
      
      bool FindEntry(const Apto::String& in_name, ArgumentType& ret_type, int& ret_idx) const;
      
      inline int NumIntArgs() const { return m_ints.GetSize(); }
      inline int NumDoubleArgs() const { return m_doubles.GetSize(); }
      inline int NumStringArgs() const { return m_strings.GetSize(); }
      
      inline bool IsOptionalInt(int i) const;
      inline bool IsOptionalDouble(int i) const;
      inline bool IsOptionalString(int i) const;
      
      inline void SetDefaultInt(int i, int& v) const;
      inline void SetDefaultDouble(int i, double& v) const;
      inline void SetDefaultString(int i, Apto::String& v) const;
      
      inline bool IntName(int i, Apto::String& name) const;
      inline bool DoubleName(int i, Apto::String& name) const;
      inline bool StringName(int i, Apto::String& name) const;
      
      inline bool ValidateInt(int i, int v) const;
      inline bool ValidateDouble(int i, double v) const;
      
    private:
      struct Entry
      {
        Apto::String name;
        ArgumentType type;
        int index;
        bool optional;
        union {
          int def_int;
          double def_double;
          Apto::String* def_string;
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
        
        
        Entry() { ; }
        Entry(const Apto::String& in_name, int in_idx, ArgumentType in_type)  // Required Argument (supplied type)
        : name(in_name), type(in_type), index(in_idx), optional(false), has_range_limits(false) { ; }
        Entry(const Apto::String& in_name, int in_idx, int def)               // Optional Int Argument
        : name(in_name), type(INT), index(in_idx), optional(true), def_int(def), has_range_limits(false) { ; }
        Entry(const Apto::String& in_name, int in_idx, double def)            // Optional Double Argument
        : name(in_name), type(DOUBLE), index(in_idx), optional(true), def_double(def), has_range_limits(false) { ; }
        Entry(const Apto::String& in_name, int in_idx, Apto::String* def)     // Optional String Argument
        : name(in_name), type(STRING), index(in_idx), optional(true), def_string(def), has_range_limits(false) { ; }
        ~Entry() { if (type == STRING && optional) delete def_string; }       // Cleanup string object
      };
    };
    
    
    // Args - Container for parsed argument values
    // --------------------------------------------------------------------------------------------------------------
    
    class Args
    {
    private:
      const ArgSchema& m_schema;
      
      Apto::Array<int> m_ints;
      Apto::Array<double> m_doubles;
      Apto::Array<Apto::String> m_strings;
      
      Apto::String m_def_string;
      
      inline Args(const ArgSchema& s)
        : m_schema(s), m_ints(s.NumIntArgs()), m_doubles(s.NumDoubleArgs()), m_strings(s.NumStringArgs()) { ; }
      
    public:
      static Args* Load(Apto::String arg_string, const ArgSchema& schema, char e_sep = ',', char v_sep = '=',
                        Feedback* feedback = NULL);
      
      inline int Int(int i) const { return m_ints[i]; }
      inline double Double(int i) const { return m_doubles[i]; }
      inline const Apto::String& String(int i) const { return m_strings[i]; }

      inline int Int(const Apto::String& arg_name) const;
      inline double Double(const Apto::String& arg_name) const;
      inline const Apto::String& String(const Apto::String& arg_name) const;

      inline bool SetInt(const Apto::String& arg_name, int v);
      inline bool SetDouble(const Apto::String& arg_name, double v);
      inline bool SetString(const Apto::String& arg_name, const Apto::String& v);
    };
    
    
    // ArgSchema - Inline Method Implementations
    // --------------------------------------------------------------------------------------------------------------
    
    inline void ArgSchema::AdjustArgName(Apto::String& in_name) const
    {
      in_name.Trim();
      if (!m_case_sensitive) in_name.ToLower();
    }
    
    
    inline void ArgSchema::SetDefaultInt(int i, int& v) const
    {
      if (IsOptionalInt(i)) v = m_ints[i]->def_int;
    }
    
    inline void ArgSchema::SetDefaultDouble(int i, double& v) const
    {
      if (IsOptionalDouble(i)) v = m_doubles[i]->def_double;
    }
    
    inline void ArgSchema::SetDefaultString(int i, Apto::String& v) const
    {
      if (IsOptionalString(i)) v = *m_strings[i]->def_string;
    }
    
    
    inline bool ArgSchema::IsOptionalInt(int i) const
    {
      if (i < m_ints.GetSize() && m_ints[i]) return m_ints[i]->optional;
      return false;
    }
    
    inline bool ArgSchema::IsOptionalDouble(int i) const
    {
      if (i < m_doubles.GetSize() && m_doubles[i]) return m_doubles[i]->optional;
      return false;
    }
    
    inline bool ArgSchema::IsOptionalString(int i) const
    {
      if (i < m_strings.GetSize() && m_strings[i]) return m_strings[i]->optional;
      return false;
    }
    
    
    inline bool ArgSchema::IntName(int i, Apto::String& name) const
    {
      if (i < m_ints.GetSize() && m_ints[i]) {
        name = m_ints[i]->name;
        return true;
      }
      return false;
    }
    
    inline bool ArgSchema::DoubleName(int i, Apto::String& name) const
    {
      if (i < m_doubles.GetSize() && m_doubles[i]) {
        name = m_doubles[i]->name;
        return true;
      }
      return false;
    }
    
    inline bool ArgSchema::StringName(int i, Apto::String& name) const
    {
      if (i < m_strings.GetSize() && m_strings[i]) {
        name = m_strings[i]->name;
        return true;
      }
      return false;
    }
    
    inline bool ArgSchema::ValidateInt(int i, int v) const
    {
      if (i < m_ints.GetSize() && m_ints[i] &&
          (!m_ints[i]->has_range_limits || (v <= m_ints[i]->r_u_int && v >= m_ints[i]->r_l_int))) {
        return true;
      }
      
      return false;
    }
    
    inline bool ArgSchema::ValidateDouble(int i, double v) const
    {
      if (i < m_doubles.GetSize() && m_doubles[i] &&
          (!m_doubles[i]->has_range_limits || (v <= m_doubles[i]->r_u_double && v >= m_doubles[i]->r_l_double))) {
        return true;
      }
      
      return false;
    }
    
    
    // Args - Inline Method Implementations
    // --------------------------------------------------------------------------------------------------------------
    
    inline int Args::Int(const Apto::String& arg_name) const
    {
      int idx = -1;
      ArgumentType type = NONE;
      
      if (m_schema.FindEntry(arg_name, type, idx) && type == INT) {
        return m_ints[idx];
      }
      
      return 0;
    }

    inline double Args::Double(const Apto::String& arg_name) const
    {
      int idx = -1;
      ArgumentType type = NONE;
      
      if (m_schema.FindEntry(arg_name, type, idx) && type == DOUBLE) {
        return m_doubles[idx];
      }
      
      return 0.0;
    }

    inline const Apto::String& Args::String(const Apto::String& arg_name) const
    {
      int idx = -1;
      ArgumentType type = NONE;
      
      if (m_schema.FindEntry(arg_name, type, idx) && type == STRING) {
        return m_strings[idx];
      }
      
      return m_def_string;
    }

    
    inline bool Args::SetInt(const Apto::String& arg_name, int v)
    {
      int idx = -1;
      ArgumentType type = NONE;
      
      if (m_schema.FindEntry(arg_name, type, idx) && type == INT) {
        m_ints[idx] = v;
        return true;
      }
      
      return false;
    }
    
    inline bool Args::SetDouble(const Apto::String& arg_name, double v)
    {
      int idx = -1;
      ArgumentType type = NONE;
      
      if (m_schema.FindEntry(arg_name, type, idx) && type == DOUBLE) {
        m_doubles[idx] = v;
        return true;
      }
      
      return false;
    }
    
    inline bool Args::SetString(const Apto::String& arg_name, const Apto::String& v)
    {
      int idx = -1;
      ArgumentType type = NONE;
      
      if (m_schema.FindEntry(arg_name, type, idx) && type == STRING) {
        m_strings[idx] = v;
        return true;
      }
      
      return false;
    }

    
  };
};

#endif
