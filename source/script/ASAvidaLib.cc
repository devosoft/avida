/*
 *  ASAvidaLib.cc
 *  Avida
 *
 *  Created by David on 10/10/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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

#include "ASAvidaLib.h"

#include "ASAvidaNativeObjects.h"

#include "cASCPPParameter_NativeObjectSupport.h"
#include "cASLibrary.h"

#include "cAnalyzeGenotype.h"
#include "cAvidaConfig.h"
#include "cDefaultRunDriver.h"
#include "cGenotypeBatch.h"
#include "cWorld.h"

#include <cstring>


static void setupNativeObjects()
{
#define REGISTER_S_METHOD(CLASS, NAME, METHOD, SIGNATURE) \
  tASNativeObject<CLASS>::RegisterMethod(new tASNativeObjectMethod<CLASS, SIGNATURE>(&CLASS::METHOD), NAME);
#define REGISTER_C_METHOD(CLASS, NAME, METHOD, SIGNATURE) \
  tASNativeObject<CLASS>::RegisterMethod(new tASNativeObjectMethodConst<CLASS, SIGNATURE>(&CLASS::METHOD), NAME);

  
  tASNativeObject<cAnalyzeGenotype>::InitializeMethodRegistrar();
  REGISTER_C_METHOD(cAnalyzeGenotype, "Name", GetName, const cString& ());

  
  tASNativeObject<cAvidaConfig>::InitializeMethodRegistrar();
  REGISTER_C_METHOD(cAvidaConfig, "Get", GetAsString, cString (const cString&));
  REGISTER_C_METHOD(cAvidaConfig, "HasEntry", HasEntry, bool (const cString&));
  REGISTER_S_METHOD(cAvidaConfig, "Load", Load, void (const cString&));
  REGISTER_S_METHOD(cAvidaConfig, "Set", Set, bool (const cString&, const cString&));
  
  
  tASNativeObject<cDefaultRunDriver>::InitializeMethodRegistrar();
  REGISTER_S_METHOD(cDefaultRunDriver, "Run", Run, void ());

  
  tASNativeObject<cGenotypeBatch>::InitializeMethodRegistrar();
  REGISTER_C_METHOD(cGenotypeBatch, "Name", GetName, const cString& ());
  REGISTER_S_METHOD(cGenotypeBatch, "GetSize", GetSize, int ());
  REGISTER_S_METHOD(cGenotypeBatch, "IsAligned", IsAligned, bool ());
  REGISTER_S_METHOD(cGenotypeBatch, "IsLineage", IsLineage, bool ());
  REGISTER_S_METHOD(cGenotypeBatch, "MergeWith", MergeWith, void (cGenotypeBatch*));
  REGISTER_C_METHOD(cGenotypeBatch, "FindGenotypeNumCPUs", FindGenotypeNumCPUs, cAnalyzeGenotype* ());
  REGISTER_S_METHOD(cGenotypeBatch, "PopGenotypeNumCPUs", PopGenotypeNumCPUs, cAnalyzeGenotype* ());
  REGISTER_C_METHOD(cGenotypeBatch, "FindGenotypeTotalCPUs", FindGenotypeTotalCPUs, cAnalyzeGenotype* ());
  REGISTER_S_METHOD(cGenotypeBatch, "PopGenotypeTotalCPUs", PopGenotypeTotalCPUs, cAnalyzeGenotype* ());
  REGISTER_C_METHOD(cGenotypeBatch, "FindGenotypeMetabolicRate", FindGenotypeMetabolicRate, cAnalyzeGenotype* ());
  REGISTER_S_METHOD(cGenotypeBatch, "PopGenotypeMetabolicRate", PopGenotypeMetabolicRate, cAnalyzeGenotype* ());
  REGISTER_C_METHOD(cGenotypeBatch, "FindGenotypeFitness", FindGenotypeFitness, cAnalyzeGenotype* ());
  REGISTER_S_METHOD(cGenotypeBatch, "PopGenotypeFitness", PopGenotypeFitness, cAnalyzeGenotype* ());
  REGISTER_C_METHOD(cGenotypeBatch, "FindGenotypeID", FindGenotypeID, cAnalyzeGenotype* (int));
  REGISTER_S_METHOD(cGenotypeBatch, "PopGenotypeID", PopGenotypeID, cAnalyzeGenotype* (int));
//  REGISTER_C_METHOD(cGenotypeBatch, "FindGenotypeRandom", FindGenotypeRandom, cAnalyzeGenotype* (cRandom*));
//  REGISTER_S_METHOD(cGenotypeBatch, "PopGenotypeRandom", PopGenotypeRandom, cAnalyzeGenotype* (cRandom*));
//  REGISTER_C_METHOD(cGenotypeBatch, "FindOrganismRandom", FindOrganismRandom, cAnalyzeGenotype* (cRandom*));
//  REGISTER_S_METHOD(cGenotypeBatch, "PopOrganismRandom", PopOrganismRandom, cAnalyzeGenotype* (cRandom*));
  REGISTER_S_METHOD(cGenotypeBatch, "FindLastCommonAncestor", FindLastCommonAncestor, cAnalyzeGenotype* ());
  REGISTER_C_METHOD(cGenotypeBatch, "FindLineage", FindLineage, cGenotypeBatch* (cAnalyzeGenotype*));
  REGISTER_C_METHOD(cGenotypeBatch, "FindSexLineage", FindSexLineage, cGenotypeBatch* (cAnalyzeGenotype*, bool));
  REGISTER_C_METHOD(cGenotypeBatch, "FindClade", FindClade, cGenotypeBatch* (cAnalyzeGenotype*));
  REGISTER_S_METHOD(cGenotypeBatch, "RemoveClade", RemoveClade, void (cAnalyzeGenotype*));
  REGISTER_S_METHOD(cGenotypeBatch, "PruneExtinctGenotypes", PruneExtinctGenotypes, void ());
  REGISTER_S_METHOD(cGenotypeBatch, "PruneNonViableGenotypes", PruneNonViableGenotypes, void ());
  

  tASNativeObject<cResourceHistory>::InitializeMethodRegistrar();

  tASNativeObject<cWorld>::InitializeMethodRegistrar();
  
  
#undef REGISTER_METHOD
#undef REGISTER_METHOD_CONST
};


template<class FunctionType> class tASNativeObjectInstantiate;

template<class NativeClass> 
class tASNativeObjectInstantiate<NativeClass ()> : public cASFunction
{
private:
  sASTypeInfo m_signature;
  
public:
  tASNativeObjectInstantiate() : cASFunction(AvidaScript::TypeOf<NativeClass*>().info)
  {
    m_rtype = AvidaScript::TypeOf<NativeClass*>();
    m_signature = AvidaScript::TypeOf<void>();
  }
  
  int GetArity() const { return 0; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set(new tASNativeObject<NativeClass>(new NativeClass()));
    return rvalue;
  }
};


template<class NativeClass, typename Arg1Type> 
class tASNativeObjectInstantiate<NativeClass (Arg1Type)> : public cASFunction
{
private:
  sASTypeInfo m_signature;
  
public:
  tASNativeObjectInstantiate() : cASFunction(AvidaScript::TypeOf<NativeClass*>().info)
  {
    m_rtype = AvidaScript::TypeOf<NativeClass*>();
    m_signature = AvidaScript::TypeOf<Arg1Type>();
  }
  
  int GetArity() const { return 1; }
  const sASTypeInfo& GetArgumentType(int arg) const { return m_signature; }
  
  cASCPPParameter Call(cASCPPParameter args[]) const
  {
    cASCPPParameter rvalue;
    rvalue.Set(new tASNativeObject<NativeClass>(new NativeClass(args[0].Get<Arg1Type>())));
    return rvalue;
  }
};



void RegisterASAvidaLib(cASLibrary* lib)
{
  setupNativeObjects();
  
  lib->RegisterFunction(new tASNativeObjectInstantiate<cAvidaConfig ()>());
  lib->RegisterFunction(new tASNativeObjectInstantiate<cDefaultRunDriver (cWorld*)>());
  lib->RegisterFunction(new tASNativeObjectInstantiate<cWorld (cAvidaConfig*)>());
    // @AS_TODO - world takes ownership of config, but I don't handle that here... world could delete it without AS knowing
}
