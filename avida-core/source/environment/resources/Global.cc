/*
 *  environment/resources/Global.cc
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

#include "avida/private/environment/resources/Global.h"

#include "avida/environment/Library.h"


static const double UPDATE_STEP(1.0 / 10000.0);
static const double EPSILON(1.0e-15);
static const int PRECALC_DISTANCE(100);

enum { ARGD_INITIAL = 0, ARGD_INFLOW, ARGD_OUTFLOW };


Avida::Environment::Resources::Global::Global(ResourceDefinition& def) : Resource(def), m_last_updated(0)
{
  m_quantity = m_def.Arguments().Double(ARGD_INITIAL);
  m_inflow_precalc.Resize(PRECALC_DISTANCE + 1);
  m_outflow_precalc.Resize(PRECALC_DISTANCE + 1);
  DefinitionChanged(m_last_updated);
}

Avida::Environment::Resources::Global::~Global() { ; }


void Avida::Environment::Resources::Global::Initialize()
{
  static Util::ArgSchema schema;
  
  schema.Define("initial", 0.0);
  schema.Define("inflow", 0.0);
  schema.Define("outflow", 0.0);
  
  Library::Instance().RegisterResourceType("global", schema, Create);
}


Avida::Environment::ResourceQuantity Avida::Environment::Resources::Global::AmountAt(const Structure::Coord& location,
                                                                                     Update current_update)
{
  (void)location;
  
  updateTo(current_update);
  
  return m_quantity;
}


void Avida::Environment::Resources::Global::SetAmountAt(ResourceQuantity amount, const Structure::Coord& location,
                                                        Update current_update)
{
  (void)location;
  
  m_quantity = amount;
  m_last_updated = current_update;  
}


Avida::Environment::ResourceQuantity Avida::Environment::Resources::Global::ModifyAmountAt(ResourceQuantity adjust_amount,
                                                                                           const Structure::Coord& location,
                                                                                           Update current_update)
{
  (void)location;
  
  updateTo(current_update);
  m_quantity += adjust_amount;
  m_last_updated = current_update;
  
  return m_quantity;
}


void Avida::Environment::Resources::Global::PerformUpdate(Avida::Context& ctx, Update current_update)
{
  ;
}

void Avida::Environment::Resources::Global::DefinitionChanged(Update current_update)
{
  updateTo(current_update);
  
  double step_inflow = m_def.Arguments().Double(ARGD_INFLOW) * UPDATE_STEP;
  double step_outflow = pow(m_def.Arguments().Double(ARGD_OUTFLOW), UPDATE_STEP);
  
  m_inflow_precalc[0] = 0.0;
  m_outflow_precalc[0] = 0.0;
  for (int i = 1; i <= PRECALC_DISTANCE; i++) {
    m_inflow_precalc[i] = m_inflow_precalc[i - 1] * step_outflow + step_inflow;
    m_outflow_precalc[i] = m_outflow_precalc[i - 1] * step_outflow;
  }
}

Avida::Environment::Resources::Global* Avida::Environment::Resources::Global::Create(ResourceDefinition& def,
                                                                                     Structure::Controller& structure)
{
  (void)structure;
  
  return new Global(def);
}


void Avida::Environment::Resources::Global::updateTo(Update current_update)
{
  // Determine how many update steps have progressed
  int num_steps = static_cast<int>(static_cast<double>(current_update - m_last_updated) / UPDATE_STEP);
  
  // Set last updated based on the actual resource steps that will be calculated (may be remaining time)
  m_last_updated += Update(num_steps * UPDATE_STEP);
  
  
  while (num_steps > PRECALC_DISTANCE) {
    m_quantity *= m_outflow_precalc[PRECALC_DISTANCE];
    m_quantity += m_inflow_precalc[PRECALC_DISTANCE];
    num_steps -= PRECALC_DISTANCE;
  }
  
  m_quantity *= m_outflow_precalc[num_steps];
  m_quantity += m_inflow_precalc[num_steps];
}
