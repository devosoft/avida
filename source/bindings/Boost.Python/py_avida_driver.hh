#ifndef PY_AVIDA_DRIVER_HH
#define PY_AVIDA_DRIVER_HH

#ifndef AVIDA_DRIVER_POPULATION_HH
#include "cAvidaDriver_Population.h"
#endif
#ifndef ENVIRONMENT_HH
#include "cEnvironment.h"
#endif

class cChangeList;
class pyAvidaDriver : public cAvidaDriver_Population {
protected:
  bool (pyAvidaDriver::*m_update_mode_function)(const unsigned int bite_size);
  bool (pyAvidaDriver::*m_update_stage_function)(const unsigned int bite_size);
  unsigned int m_update_size;
  double m_step_size;
  unsigned int m_update_progress;
  int m_mutations_progress;
  // current cell id must be stored in case the organism in that cell
  // encounters a breakpoint (we don't have any other way of
  // communicating this to the breakpoint-handling function).
  int m_current_cell_id;
  int m_step_cell_id;
  cChangeList *m_change_list;
protected:
  bool preUpdate(const unsigned int);
  bool fastUpdate(const unsigned int bite_size);
  bool stepUpdate(const unsigned int bite_size);
  bool postUpdate(const unsigned int);
  bool ptMutations(const unsigned int);
  bool postPtMutations(const unsigned int);
public:
  pyAvidaDriver(cEnvironment & environment);
  virtual ~pyAvidaDriver();
  bool getDoneFlag(){ return done_flag; }
  void setDoneFlag(){ done_flag = true; }
public:
  bool ProcessSome(unsigned int bite_size){ return (this->*(m_update_stage_function))(bite_size); }
  cChangeList *GetChangeList();
};

#endif
