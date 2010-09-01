/*
 *  tDataEntry.h
 *  Avida
 *
 *  Called "tDataEntry.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef tDataEntry_h
#define tDataEntry_h

#include <iostream>

#ifndef cFlexVar_h
#include "cFlexVar.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef cStringList_h
#include "cStringList.h"
#endif
#ifndef cStringUtil_h
#include "cStringUtil.h"
#endif


template <class TargetType> class tDataEntry
{
private:
  cString m_name;            // Short Name
  cString m_desc;            // Descriptive Name
  int m_compare_type;        // ID to indicate how values should be compared.
  cString m_null_value;      // Value when "off", such as "0", "Inf.", or "N/A"
  cString m_html_table_flags; // String to include in <td> entry in html mode.
  cStringList m_default_args;
  
public:
  tDataEntry(const cString& name, const cString& desc, int compare_type = 0,
             const cString& null = "0", const cString& html_cell = "align=center")
  : m_name(name), m_desc(desc), m_compare_type(compare_type), m_null_value(null), m_html_table_flags(html_cell) { ; }
  virtual ~tDataEntry() { ; }
  
  const cString& GetName() const { return m_name; }
  virtual cString GetDesc(const TargetType* target, const cFlexVar& idx) const { return m_desc; }
  int GetCompareType() const { return m_compare_type; }
  const cString& GetNull() const { return m_null_value; }
  const cString& GetHtmlCellFlags() const { return m_html_table_flags; }
  
  virtual bool Set(TargetType* target, const cFlexVar&, const cStringList&, const cString&) const { return false; }
  virtual cFlexVar Get(const TargetType* target, const cFlexVar& idx, const cStringList& args) const = 0;
  virtual cFlexVar Get(const TargetType* target) const { return Get(target, 0, m_default_args); }
};

template <class TargetType, class EntryType> class tDataEntryOfType;

template <class TargetType, class EntryType>
  class tDataEntryOfType<TargetType, EntryType ()> : public tDataEntry<TargetType>
{
protected:
  EntryType  (TargetType::*DataGet)() const;
  void (TargetType::*DataSet)(EntryType);

public:
  tDataEntryOfType(const cString& name, const cString& desc,
                   EntryType (TargetType::*_funR)() const, void (TargetType::*_funS)(EntryType _val) = NULL,
                   int compare_type = 0, const cString& null = "0", const cString& html_cell = "align=center")
  : tDataEntry<TargetType>(name, desc, compare_type, null, html_cell), DataGet(_funR), DataSet(_funS) { ; }

  bool Set(TargetType* target, const cFlexVar&, const cStringList&, const cString& value) const
  {
    assert(target != NULL);
    EntryType new_value(0);
    if (DataSet == 0) return false;
    (target->*DataSet)(cStringUtil::Convert(value, new_value));
    return true;
  }

  cFlexVar Get(const TargetType* target, const cFlexVar&, const cStringList&) const
  {
    assert(target != NULL);
    return cFlexVar((target->*DataGet)());
  }
};


template <class TargetType, class EntryType, class IdxType>
  class tDataEntryOfType<TargetType, EntryType (IdxType)> : public tDataEntry<TargetType>
{
protected:
  EntryType (TargetType::*DataRetrieval)(IdxType) const;
  cString (TargetType::*DescFunction)(IdxType) const;
  
public:
  tDataEntryOfType(const cString& name,
                   cString (TargetType::*_funD)(IdxType) const,
                   EntryType (TargetType::*_funR)(IdxType) const,
                   int compare_type = 0, const cString& null = "0", const cString& html_cell = "align=center")
  : tDataEntry<TargetType>(name, name, compare_type, null, html_cell), DataRetrieval(_funR), DescFunction(_funD) { ; }
  
  cString GetDesc(const TargetType* target, const cFlexVar& idx) const
  {
    return (target->*DescFunction)(idx.As<IdxType>());
  }
  
  
  cFlexVar Get(const TargetType* target, const cFlexVar& idx, const cStringList&) const
  {
    assert(target != NULL);
    return cFlexVar((target->*DataRetrieval)(idx.As<IdxType>()));
  }
};


template <class TargetType, class EntryType, class IdxType>
  class tDataEntryOfType<TargetType, EntryType (IdxType, const cStringList&)> : public tDataEntry<TargetType>
{
protected:
  EntryType (TargetType::*DataRetrieval)(IdxType, const cStringList&) const;
  cString (TargetType::*DescFunction)(IdxType) const;
  
public:
  tDataEntryOfType(const cString& name,
                   cString (TargetType::*_funD)(IdxType) const,
                   EntryType (TargetType::*_funR)(IdxType, const cStringList&) const,
                   int compare_type = 0, const cString& null = "0", const cString& html_cell = "align=center")
  : tDataEntry<TargetType>(name, name, compare_type, null, html_cell), DataRetrieval(_funR), DescFunction(_funD) { ; }
  
  cString GetDesc(const TargetType* target, const cFlexVar& idx) const
  {
    return (target->*DescFunction)(idx.As<IdxType>());
  }

  cFlexVar Get(const TargetType* target, const cFlexVar& idx, const cStringList& args) const
  {
    assert(target != NULL);
    return cFlexVar((target->*DataRetrieval)(idx.As<IdxType>(), args));
  }
};


template <class TargetType, class IdxType> class tDataEntryProxy;

template <class TargetType>
class tDataEntryProxy<TargetType, cFlexVar ()> : public tDataEntry<TargetType>
{
protected:
  cFlexVar (*DataRetrieval)(const TargetType*);
  
public:
  tDataEntryProxy(const cString& name, const cString& desc,
                  cFlexVar (*_funR)(const TargetType*),
                  int compare_type = 0, const cString& null = "0", const cString& html_cell = "align=center")
  : tDataEntry<TargetType>(name, desc, compare_type, null, html_cell), DataRetrieval(_funR) { ; }
  
  cFlexVar Get(const TargetType* target, const cFlexVar& idx, const cStringList& args) const
  {
    assert(target != NULL);
    return (*DataRetrieval)(target);
  }
};

template <class TargetType, class IdxType>
class tDataEntryProxy<TargetType, cFlexVar (IdxType)> : public tDataEntry<TargetType>
{
protected:
  cFlexVar (*DataRetrieval)(const TargetType*, IdxType);
  cString (*DescFunction)(const TargetType*, IdxType);
  
public:
  tDataEntryProxy(const cString& name,
                  cString (*_funD)(const TargetType*, IdxType),
                  cFlexVar (*_funR)(const TargetType*, IdxType),
                  int compare_type = 0, const cString& null = "0", const cString& html_cell = "align=center")
  : tDataEntry<TargetType>(name, name, compare_type, null, html_cell), DataRetrieval(_funR), DescFunction(_funD) { ; }
  
  cString GetDesc(const TargetType* target, const cFlexVar& idx) const
  {
    return (*DescFunction)(target, idx.As<IdxType>());
  }
  
  cFlexVar Get(const TargetType* target, const cFlexVar& idx, const cStringList& args) const
  {
    assert(target != NULL);
    return (*DataRetrieval)(target, idx.As<IdxType>());
  }
};

template <class TargetType, class IdxType>
class tDataEntryProxy<TargetType, cFlexVar (IdxType, const cStringList&)> : public tDataEntry<TargetType>
{
protected:
  cFlexVar (*DataRetrieval)(const TargetType*, IdxType, const cStringList&);
  cString (*DescFunction)(const TargetType*, IdxType);
  
public:
  tDataEntryProxy(const cString& name,
                  cString (*_funD)(const TargetType*, IdxType),
                  cFlexVar (*_funR)(const TargetType*, IdxType, const cStringList&),
                  int compare_type = 0, const cString& null = "0", const cString& html_cell = "align=center")
  : tDataEntry<TargetType>(name, name, compare_type, null, html_cell), DataRetrieval(_funR), DescFunction(_funD) { ; }
  
  cString GetDesc(const TargetType* target, const cFlexVar& idx) const
  {
    return (*DescFunction)(target, idx.As<IdxType>());
  }
  
  cFlexVar Get(const TargetType* target, const cFlexVar& idx, const cStringList& args) const
  {
    assert(target != NULL);
    return (*DataRetrieval)(target, idx.As<IdxType>(), args);
  }
};


#endif
