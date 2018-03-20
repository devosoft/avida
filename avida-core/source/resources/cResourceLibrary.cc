//
//  cResourceLibrary.cc
//  avida-core (static)
//
//  Created by Matthew Rupp on 2/7/18.
//

#include "cResourceLibrary.h"
#include "cNonSpatialResource.h"
#include "cSpatialResource.h"
#include "cGradientResource.h"
#include "cCellResource.h"

using namespace Avida::Resource;

cResourceLibrary::cResourceLibrary(int num_demes, Avida::Feedback& fb)
{
  m_global_resreg = std::unique_ptr<cResourceRegistry>(new cResourceRegistry(fb));
  for (int k=0; k < num_demes; ++k)
  {
    m_deme_resreg.push_back(std::unique_ptr<cResourceRegistry>(new cResourceRegistry(fb)));
  }
}


void cResourceLibrary::AddResource(cNonSpatialResource* nonspat_res)
{
  if (nonspat_res->GetIsDemeResource()){
    for (auto& reg : m_deme_resreg){
        reg->AddResource(nonspat_res);
    }
  } else {
    m_global_resreg->AddResource(nonspat_res);
  }
}



void cResourceLibrary::AddResource(cSpatialResource* spat_res)
{
  if (spat_res->GetIsDemeResource()){
    for (auto& reg : m_deme_resreg){
        reg->AddResource(spat_res);
    }
  } else {
    m_global_resreg->AddResource(spat_res);
  }
}



void cResourceLibrary::AddResource(cGradientResource* grad_res, cPopulation* pop)
{
  if (grad_res->GetIsDemeResource()){
    for (auto& reg : m_deme_resreg){
        reg->AddResource(grad_res, pop);
    }
  } else {
    m_global_resreg->AddResource(grad_res, pop);
  }
}


void cResourceLibrary::AddResource(cCellResource* cell_res)
{
  if (cell_res->GetIsDemeResource()){
    for (auto& reg : m_deme_resreg){
        reg->AddResource(cell_res);
    }
  } else {
    m_global_resreg->AddResource(cell_res);
  }
}



