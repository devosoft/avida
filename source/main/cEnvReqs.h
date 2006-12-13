/*
 *  cEnvReqs.h
 *  Avida
 *
 *  Created by David Bryson on 12/12/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cEnvReqs_h
#define cEnvReqs_h

class cEnvReqs
{
private:
  int m_min_inputs;
  int m_min_outputs;


public:
  cEnvReqs() : m_min_inputs(0), m_min_outputs(0) { ; }
  
  int GetMinInputs() { return m_min_inputs; }
  void SetMinInputs(int v) { m_min_inputs = v; }
  int GetMinOutputs() { return m_min_outputs; }
  void SetMinOutputs(int v) { m_min_outputs = v; }
};

#endif
