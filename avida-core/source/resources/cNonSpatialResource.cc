//
//  cNonSpatialResource.cpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#include "cNonSpatialResource.h"


const double cNonSpatialResourceAcct::UPDATE_STEP(1.0 / 10000.0);
const double cNonSpatialResourceAcct::EPSILON (1.0e-15);
const int cNonSpatialResourceAcct::PRECALC_DISTANCE(100);

cNonSpatialResourceAcct::cNonSpatialResourceAcct(const cNonSpatialResource& res)
: m_resource(res)
{
  double step_decay = pow(res.GetDecay(), UPDATE_STEP);
  double step_inflow = res.GetInflow() * UPDATE_STEP;

  m_decay_precalc.Resize(PRECALC_DISTANCE);
  m_inflow_precalc.Resize(PRECALC_DISTANCE);
  m_decay_precalc[0] = 1.0;
  m_inflow_precalc[0] = 0.0;
  for (int i = 1; i <= PRECALC_DISTANCE; i++) {
    m_decay_precalc[i]  = m_decay_precalc[i-1] * step_decay;
    m_inflow_precalc[i] = m_inflow_precalc[i-1] * step_decay + step_inflow;
  }
}


void cNonSpatialResourceAcct::AddTime(double tt)
{
  m_update_time += tt;
  m_calc_steps = (int) (m_update_time / UPDATE_STEP);
  m_update_time -= m_calc_steps * UPDATE_STEP;
}

void cNonSpatialResourceAcct::Update()
{
  int num_steps = m_calc_steps;
  while (num_steps > PRECALC_DISTANCE) {
    m_current *= m_decay_precalc[PRECALC_DISTANCE];
    m_current += m_inflow_precalc[PRECALC_DISTANCE];
    num_steps -= PRECALC_DISTANCE;
  }
  
  // Calculate our remaining number of steps
  m_current *= m_decay_precalc[num_steps];
  m_current += m_inflow_precalc[num_steps];
}
