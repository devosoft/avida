#ifndef PLOT_SELECT_DIA_IMPL_HH
#define PLOT_SELECT_DIA_IMPL_HH

#include "plot_select_dia.h"

#ifndef AVIAD_DATA_HH
#include "avida_data.hh"
#endif

class PlotSelectDiaImpl : public PlotSelectDia
{
  Q_OBJECT

public:
  /**
   *  Constructs a PlotSelectDiaImpl which is a child of 'parent', with the
   *  name 'name' and widget flags set to 'f'
   *
   **/
  PlotSelectDiaImpl( QWidget* parent = 0, const char* name = 0, bool modal = true, WFlags fl = 0 );

  /**
   * Destroys the object and frees any allocated resources
   **/
  ~PlotSelectDiaImpl();
  
  /**
   * @return The type of the item selected (starting with 0).
   **/
  AvidaData::eTypes result() const;
};

#endif // PLOTSELECTDIAIMPL_H

