/*
 *  cOrgInterface.h
 *  Avida
 *
 *  Created by David on 3/4/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cOrgInterface_h
#define cOrgInterface_h

class cAvidaContext;
class cCodeLabel;
class cGenome;
class cOrganism;
class cOrgMessage;
class cOrgSinkMessage;
template <class T> class tArray;


class cOrgInterface
{
private:
  cOrgInterface(const cOrgInterface&); // @not_implemented
  cOrgInterface& operator=(const cOrgInterface&); // @not_implemented
  
public:
  cOrgInterface() { ; }
  virtual ~cOrgInterface() { ; }

  virtual int GetCellID() = 0;
  virtual void SetCellID(int in_id) = 0;

  virtual bool Divide(cAvidaContext& ctx, cOrganism* parent, cGenome& child_genome) = 0;
  virtual cOrganism* GetNeighbor() = 0;
  virtual int GetNumNeighbors() = 0;
  virtual void Rotate(int direction = 1) = 0;
  virtual void Breakpoint() = 0;
  virtual double TestFitness() = 0;
  virtual int GetInput() = 0;
  virtual int GetInputAt(int& input_pointer) = 0;
  virtual int Debug() = 0;
  virtual const tArray<double>& GetResources() = 0;
  virtual void UpdateResources(const tArray<double>& res_change) = 0;
  virtual void Die() = 0;
  virtual void Kaboom() = 0;
  virtual bool SendMessage(cOrgMessage& mess) = 0;
  virtual cOrgSinkMessage* NetReceive() = 0;
  virtual bool NetRemoteValidate(cAvidaContext& ctx, cOrgSinkMessage* msg) = 0;
  virtual int ReceiveValue() = 0;
  virtual bool InjectParasite(cOrganism* parent, const cGenome& injected_code) = 0;
  virtual bool UpdateMerit(double new_merit) = 0;
};

#endif
