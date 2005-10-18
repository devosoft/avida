/*
 *  cWorld.cc
 *  Avida2
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include "cWorld.h"

void cWorld::Setup() {
  // Setup Random Number Generator
  const int rand_seed = m_config->RANDOM_SEED.Get();
  cout << "Random Seed: " << rand_seed;
  m_rng.ResetSeed(rand_seed);
  if (rand_seed != m_rng.GetSeed()) cout << " -> " << m_rng.GetSeed();
  cout << endl;  
  
  // The default directory should end in a '/'.
  cString default_dir = m_config->DEFAULT_DIR.Get();
  char dir_tail = default_dir[default_dir.GetSize() - 1];
  if (dir_tail != '\\' && dir_tail != '/') {
    default_dir += "/";
    m_config->DEFAULT_DIR.Set(default_dir);
  }
}
