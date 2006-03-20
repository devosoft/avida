/*
 *  cMutationRates.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cMutationRates.h"


void cMutationRates::Clear()
{
  exec.point_mut_prob = 0.0;
  copy.copy_mut_prob = 0.0;
  divide.ins_mut_prob = 0.0;
  divide.del_mut_prob = 0.0;
  divide.div_mut_prob = 0.0;
  divide.divide_mut_prob = 0.0;
  divide.divide_ins_prob = 0.0;
  divide.divide_del_prob = 0.0;
  divide.parent_mut_prob = 0.0;
  divide.crossover_prob = 0.0;
  divide.aligned_cross_prob = 0.0;
}

void cMutationRates::Copy(const cMutationRates & in_muts)
{
  exec.point_mut_prob = in_muts.exec.point_mut_prob;
  copy.copy_mut_prob = in_muts.copy.copy_mut_prob;
  divide.ins_mut_prob = in_muts.divide.ins_mut_prob;
  divide.del_mut_prob = in_muts.divide.del_mut_prob;
  divide.div_mut_prob = in_muts.divide.div_mut_prob;
  divide.divide_mut_prob = in_muts.divide.divide_mut_prob;
  divide.divide_ins_prob = in_muts.divide.divide_ins_prob;
  divide.divide_del_prob = in_muts.divide.divide_del_prob;
  divide.parent_mut_prob = in_muts.divide.parent_mut_prob;
  divide.crossover_prob = in_muts.divide.crossover_prob;
  divide.aligned_cross_prob = in_muts.divide.aligned_cross_prob;
}
