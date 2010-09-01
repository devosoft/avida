/*
 *  cSymbolTable.h
 *  Avida
 *
 *  Created by David on 2/2/06.
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

#ifndef cSymbolTable_h
#define cSymbolTable_h

#include "AvidaScript.h"
#include "ASTree.h"

#include "tArray.h"
#include "tDictionary.h"


// Active/inactive functions and variables within the scope of the current function (or globally)


class cSymbolTable
{
private:
  // --------  Internal Type Declarations  --------
  struct sSymbolEntry;
  struct sFunctionEntry;

  
  // --------  Internal Variables  --------
  tArray<sSymbolEntry*> m_sym_tbl;
  tDictionary<int> m_sym_dict;
  tArray<sFunctionEntry*> m_fun_tbl;
  tDictionary<int> m_fun_dict;

  int m_scope;
  int m_deactivate_cycle;
  
  bool m_return;
  
  
  // --------  Private Constructors  --------
  cSymbolTable(const cSymbolTable&); // @not_implemented
  cSymbolTable& operator=(const cSymbolTable&); // @not_implemented
  
  
public:
  cSymbolTable() : m_scope(0), m_deactivate_cycle(0), m_return(false) { ; }
  ~cSymbolTable();

  
  // --------  Add/Lookup Methods  --------
  bool AddVariable(const cString& name, const sASTypeInfo& type, int& var_id);
  bool AddFunction(const cString& name, const sASTypeInfo& type, int& fun_id);
  
  bool LookupVariable(const cString& name, int& var_id) { return m_sym_dict.Find(name, var_id); }
  bool LookupFunction(const cString& name, int& fun_id) { return m_fun_dict.Find(name, fun_id); }
  
  inline int GetNumVariables() const { return m_sym_tbl.GetSize(); }
  inline int GetNumFunctions() const { return m_fun_tbl.GetSize(); }
  
  inline cString VariableNearMatch(const cString& name) const { return m_sym_dict.NearMatch(name); }
  inline cString FunctionNearMatch(const cString& name) const { return m_fun_dict.NearMatch(name); }


  // --------  Scope Methods  --------
  inline void PushScope() { m_scope++; }
  void PopScope();
  inline int GetScope() const { return m_scope; }
  
  inline void SetScopeReturn() { m_return = true; }
  inline bool ScopeHasReturn() const { return m_return; }
  

  // --------  Variable Property Methods  --------
  inline const sASTypeInfo& GetVariableType(int var_id) const { return m_sym_tbl[var_id]->type; }


  // --------  Function Property Methods  --------
  inline const cString& GetFunctionName(int fun_id) const { return m_fun_tbl[fun_id]->name; }
  inline const sASTypeInfo& GetFunctionRType(int fun_id) const { return m_fun_tbl[fun_id]->type; }
  inline cSymbolTable* GetFunctionSymbolTable(int fun_id) { return m_fun_tbl[fun_id]->symtbl; }
  inline cASTVariableDefinitionList* GetFunctionSignature(int fun_id) { return m_fun_tbl[fun_id]->signature; }
  inline cASTNode* GetFunctionDefinition(int fun_id) { return m_fun_tbl[fun_id]->code; }
  inline int GetFunctionScope(int fun_id) const { return m_fun_tbl[fun_id]->scope; }
  inline bool IsFunctionActive(int fun_id) const { return !m_fun_tbl[fun_id]->deactivate; }
  
  inline void SetFunctionSymbolTable(int fun_id, cSymbolTable* symtbl) { m_fun_tbl[fun_id]->symtbl = symtbl; }
  inline void SetFunctionSignature(int fun_id, cASTVariableDefinitionList* vdl) { m_fun_tbl[fun_id]->signature = vdl; }
  inline void SetFunctionDefinition(int fun_id, cASTNode* code) { m_fun_tbl[fun_id]->code = code; }
  
  
  // --------  Externally Visible Type Declarations  --------
  class cFunctionIterator
  {
    friend class cSymbolTable;
  
  private:
    cSymbolTable* m_symtbl;
    int m_scope;
    int m_idx;
    
    cFunctionIterator(cSymbolTable* symtbl)
      : m_symtbl(symtbl), m_scope(symtbl->GetScope()), m_idx(symtbl->GetNumFunctions()) { ; }

  public:
    bool Next()
    {
      for (m_idx--; m_idx >= 0; m_idx--)
        if (m_symtbl->GetFunctionScope(m_idx) == m_scope && m_symtbl->IsFunctionActive(m_idx)) return true;
      
      return false;
    }
    
    inline bool HasCode() const { return m_symtbl->GetFunctionDefinition(m_idx); }
    inline const cString& GetName() const { return m_symtbl->GetFunctionName(m_idx); }
  };


  // --------  Externally Visible Type Dependent Methods  --------
  inline cFunctionIterator ActiveFunctionIterator() { return cFunctionIterator(this); }  
  

private:
  // --------  Internal Type Definitions  --------
  struct sSymbolEntry
  {
    cString name;
    sASTypeInfo type;
    
    int scope;
    int shadow;
    int deactivate;
    
    sSymbolEntry(const cString& in_name, const sASTypeInfo& in_type, int in_scope)
      : name(in_name), type(in_type), scope(in_scope), shadow(-1), deactivate(0) { ; }
  };
  
  struct sFunctionEntry
  {
    cString name;
    sASTypeInfo type;
    cASTVariableDefinitionList* signature;
    cSymbolTable* symtbl;
    cASTNode* code;
    
    int scope;
    int shadow;
    int deactivate;
    
    sFunctionEntry(const cString& in_name, const sASTypeInfo& in_type, int in_scope)
      : name(in_name), type(in_type), signature(NULL), symtbl(NULL), code(NULL), scope(in_scope), shadow(-1)
      , deactivate(0) { ; }
    ~sFunctionEntry() { delete signature; delete symtbl; delete code; }
  };
};


#endif
