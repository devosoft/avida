//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "population_cell.hh"
#include "population.hh"
#include "messaging_population_wrapper.hh"

#include "population_cell_wrapper.hh"


#ifndef DEBUG
# define LOCAL_DEBUG 0
#else
//# define LOCAL_DEBUG 1
# define LOCAL_DEBUG 0
#endif


using namespace std;


PopulationCellWrapper::PopulationCellWrapper(
    QObject *parent,
    const char *name
)
: QObject(parent, name)
{
  #if LOCAL_DEBUG
  cout
  << "<PopulationCellWrapper::PopulationCellWrapper>\n"
  << " --- created new PopulationCellWrapper.\n";
  #endif

  setPopulationWrapper(0);
  m_population_cell = 0;
}

void
PopulationCellWrapper::setPopulationWrapper(
  MessagingPopulationWrapper *pop_wrap
){
  #if LOCAL_DEBUG
  cout
  << "<PopulationCellWrapper::setPopulationWrapper>\n"
  << " --- entered.\n";
  #endif

  m_pop_wrap = pop_wrap;

  if(pop_wrap == 0){
    #if LOCAL_DEBUG
    cout
    << "<PopulationCellWrapper::setPopulationWrapper>\n"
    << " --- WARNING:  pop_wrap == 0.\n";
    #endif

    return;
  }
  connect(
    this, SIGNAL(temporaryStartSig(void)),
    m_pop_wrap, SIGNAL(startAvidaSig(void))
  );
  connect(
    this, SIGNAL(temporaryStopSig(void)),
    m_pop_wrap, SIGNAL(stopAvidaSig(void))
  );
  connect(
    this, SIGNAL(temporaryStepSig(int)),
    m_pop_wrap, SLOT(stepAvidaSlot(int))
  );
  connect(
    this, SIGNAL(temporaryUpdateSig(void)),
    m_pop_wrap, SLOT(updateAvidaSlot(void))
  );
}

cPopulationCell *
PopulationCellWrapper::getPopulationCell(void){
  #if LOCAL_DEBUG
  cout
  << "<PopulationCellWrapper::getPopulationCell>\n"
  << " --- entered.\n";
  #endif

  return m_population_cell;
}

void
PopulationCellWrapper::updateState(void){
  #if LOCAL_DEBUG
  cout
  << "<PopulationCellWrapper::updateState>\n"
  << " --- caught signal;\n"
  << " --- emitting signal stateChanged_sig(PopulationCellWrapper *cw).\n";
  #endif

  emit(stateChanged_sig(this));
}

void
PopulationCellWrapper::setPopulationCell(int cell_number){
  #if LOCAL_DEBUG
  cout
  << "<PopulationCellWrapper::setPopulationCell>\n"
  << " --- caught signal.\n";
  #endif
  
  if(m_pop_wrap){
    m_pop_wrap->lock();
    m_population_cell =
        (m_pop_wrap->getPopulation())?(
          &(m_pop_wrap->getPopulation()->GetCell(cell_number))
        ):(0);
    m_pop_wrap->unlock();
  } else {
    m_population_cell = 0;
  }

  updateState();
  return;
}

