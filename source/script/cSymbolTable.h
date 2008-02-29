/*
 *  cSymbolTable.h
 *  Avida
 *
 *  Created by David on 2/2/06.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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
  struct sSymbolEntry
  {
    cString name;
    ASType_t type;

    bool active;
    
    sSymbolEntry(const cString& in_name, ASType_t in_type) : name(in_name), type(in_type), active(true) { ; }
  };
  tArray<sSymbolEntry*> m_sym_tbl;
  tDictionary<int> m_sym_dict;
  
  struct sFunctionEntry
  {
    cString name;
    ASType_t type;
    cASTVariableDefinitionList* signature;
    cSymbolTable* symtbl;
    cASTNode* code;
    
    bool active;
    
    sFunctionEntry(const cString& in_name, ASType_t in_type)
      : name(in_name), type(in_type), signature(NULL), symtbl(NULL), code(NULL), active(true) { ; }
    ~sFunctionEntry() { delete signature; delete symtbl; delete code; }
  };
  tArray<sFunctionEntry*> m_fun_tbl;
  tDictionary<int> m_fun_dict;
  
  
  cSymbolTable(const cSymbolTable&); // @not_implemented
  cSymbolTable& operator=(const cSymbolTable&); // @not_implemented
  
  
public:
  cSymbolTable() { ; }
  ~cSymbolTable();

  
  bool AddVariable(const cString& name, ASType_t type, int& var_id);
  bool AddFunction(const cString& name, ASType_t type, int& fun_id);
  
  bool LookupVariable(const cString& name, int& var_id) { return m_sym_dict.Find(name, var_id); }
  bool LookupFunction(const cString& name, int& fun_id) { return m_fun_dict.Find(name, fun_id); }
  
  ASType_t GetVariableType(int var_id) const { return m_sym_tbl[var_id]->type; }
  
  ASType_t GetFunctionRType(int fun_id) const { return m_fun_tbl[fun_id]->type; }
  cSymbolTable* GetFunctionSymbolTable(int fun_id) { return m_fun_tbl[fun_id]->symtbl; }
  cASTVariableDefinitionList* GetFunctionSignature(int fun_id) { return m_fun_tbl[fun_id]->signature; }
  cASTNode* GetFunctionDefinition(int fun_id) { return m_fun_tbl[fun_id]->code; }
  
  inline void SetFunctionSymbolTable(int fun_id, cSymbolTable* symtbl) { m_fun_tbl[fun_id]->symtbl = symtbl; }
  inline void SetFunctionSignature(int fun_id, cASTVariableDefinitionList* vdl) { m_fun_tbl[fun_id]->signature = vdl; }
  inline void SetFunctionDefinition(int fun_id, cASTNode* code) { m_fun_tbl[fun_id]->code = code; }
  
  inline cString VariableNearMatch(const cString& name) const { return m_sym_dict.NearMatch(name); }
  inline cString FunctionNearMatch(const cString& name) const { return m_fun_dict.NearMatch(name); }
};



#endif
