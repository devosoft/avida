//
//  cAbstractAccountant.hpp
//  avida-core (static)
//
//  Created by Matthew Rupp on 1/14/18.
//

#ifndef cAbstractResAcct_h
#define cAbstractResAcct_h

#include "cSpatialCountElem.h"
#include "cOffsetLinearGrid.h"

typedef Apto::Array<cSpatialCountElem> cCellElements;
typedef Apto::Array<double> cResourceGrid;



class cAbstractResourceAcct
{ 
  public:
    virtual double GetTotalAbundance() const = 0;
    virtual double GetCellAbundance(int cell_id) const = 0;
    virtual double operator[](int cell_id) const = 0;
    virtual double operator()(int cell_id) const = 0;
    virtual double operator()(int x, int y) const = 0;
    
    virtual void Update() = 0;
    
};

class cAbstractSpatialResourceAcct : public cAbstractResourceAcct
{
  protected:
    int m_world_size_x;
    int m_world_size_y;
    int m_size_x;
    int m_size_y;
    cCellBox m_cellbox;
    cOffsetLinearGrid<double> m_abundance;

  
  public:
    cAbstractSpatialResourceAcct(int size_x, int size_y, const cCellBox& cellbox)
    : m_world_size_x(size_x)
    , m_world_size_y(size_y)
    , m_size_x(cellbox.GetWidth())
    , m_size_y(cellbox.GetHeight())
    , m_cellbox(cellbox)
    , m_abundance(size_x, size_y, cellbox, 0.0)
    {
    }
    
    virtual double GetTotalAbundance() const
    {
      double sum = 0.0;
      for (int k = 0; k < m_abundance.GetSize(); k++){
        sum += m_abundance[k];
      }
      return sum;
    }
    
    double SumAll() const
    {
      return this->GetTotalAbundance();
    }
    
    virtual double GetAmount(int cell_id) const
    {
      return m_abundance[cell_id];
    }
    
    void SetCellAmount(int cell_id, double amount) 
    { 
      m_abundance(cell_id) = amount;  //Will the compiler chose the one with the reference?
    }
    
    virtual double operator[](int cell_id) const
    {
      return m_abundance[cell_id];
    }
    
    virtual double operator()(int x, int y) const
    {
      return m_abundance(x,y);
    }
    
};

#endif /* cAbstractAccountant_h */
