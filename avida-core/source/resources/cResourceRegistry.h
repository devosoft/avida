/*
 *  cResourceRegistry.h
 *  Avida
 *
 *  Called "resource_lib.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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
 */

#ifndef cResourceRegistry_h
#define cResourceRegistry_h

#include "avida/core/Feedback.h"
#include "avida/core/Types.h"
#include "cNonSpatialResource.h"


class cAvidaContext;
class cResourceHistory;
class cString;

class cResource;
class cResourceAcct;

class cSpatialResourceAcct;
class cNonSpatialResourceAcct;

class cCellResource;
class cNonSpatialResource;
class cSpatialResource;
class cGradientResource;
class Feedback;


typedef int ResourceID;

namespace Avida{
  namespace Resource{
    
    class cResourceRegistry
    {
    protected:
      
      Avida::Feedback& m_feedback;
      Apto::Array<cResource*> m_resources;  // We own these pointers
      Apto::Map<cString, cResource*> m_name_map;
      
      void AddResource(cResource* res);
      
      cResourceRegistry(const cResourceRegistry&); // @not_implemented
      cResourceRegistry& operator=(const cResourceRegistry&); // @not_implemented
      
    public:
      cResourceRegistry(Avida::Feedback& fb) 
      : m_feedback(fb) 
      {}
      
      ~cResourceRegistry();
      
      int GetSize() const { return m_resources.GetSize(); }
      
      void AddResource(cCellResource* res);
      void AddResource(cNonSpatialResource* nonspat_res);
      void AddResource(cSpatialResource* spat_res);
      void AddResource(cGradientResource* grad_res);
      
      cResource* GetResource(const cString& res_name) const;
      inline cResource* GetResource(int id) const;
      
      cResourceAcct* GetResourceAcct(const cString& res_name);
      cResourceAcct* GetResourceAcct(int res_id);
      cSpatialResourceAcct* GetSpatialResoureAcct(const cString& res_name);
      cSpatialResourceAcct* GetSpatialResoureAcct(int res_id);
      cNonSpatialResourceAcct* GetNonSpatialResourceAcct(int res_id);
      cNonSpatialResourceAcct* GetNonSpatialResourceAcct(const cString& res_name);
      
      bool DoesResourceExist(const cString& res_name) const;
      
      
      double GetResourceAbundance(cAvidaContext& ctx, int res_id) const;
      Apto::Array<double> GetResAbundances(cAvidaContext& ctx) const;
      Apto::Array<double> GetResAbundances(cAvidaContext& ctx, int cell_id);
      Apto::Array<double> GetFrozenResources(cAvidaContext& ctx, int cell_id) const;  
      
      
      void TriggerDoUpdates(cAvidaContext& ctx); 
      
      //const Apto::Array< Apto::Array<int> >& GetCellIdLists() const; 
      
      
      void UpdateResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
      void UpdateRandomResources(cAvidaContext& ctx, const Apto::Array<double>& res_change);
      void UpdateResource(cAvidaContext& ctx, int id, double change);
      void UpdateCellResources(cAvidaContext& ctx, 
                               const Apto::Array<double>& res_change, const int cell_id);
      
    };
  } //namespace Resource
} //namespace Avida
#endif
