#ifndef AVD_PLOT_VIEWER_CONTROLLER_HH
#include "avd_plot_viewer_controller.hh"
#endif

#ifndef AVD_MISSION_CONTROL_HH
#include "avd_mission_control.hh"
#endif

/*
#ifndef AVD_MAIN_WINDOW_HH
#include "avd_main_window.hh"
#endif
*/
#ifndef AVIDA_PLOT_WIDGET_HH
#include "avida_plot_widget.hh"
#endif
#ifndef PLOT_SELECT_DIA_IMPL_HH
#include "plot_select_dia_impl.hh"
#endif

#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QMESSAGEBOX_H
#include <qmessagebox.h>
#endif

#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef MESSAGE_DISPLAY_HDRS_HH
#include "message_display_hdrs.hh"
#endif


using namespace std;


avd_PlotViewerController::avd_PlotViewerController(
  avd_MissionControl &mission_control,
  QObject *parent,
  const char *name
):QObject(parent, name),
  m_mission_control(mission_control)
{
  GenDebug("entered.");
  connect(
    &m_mission_control, SIGNAL(newPlotViewerSig(void)),
    this, SLOT(newPlotViewerSlot(void))
  );
  GenDebug("done.");
}

avd_PlotViewerController::~avd_PlotViewerController(){
  GenDebug("entered.");
  GenDebug("done.");
}

void
avd_PlotViewerController::wrapupInit(void){
  GenDebug("entered.");
  GenDebug("done.");
}


void
avd_PlotViewerController::newPlotViewerSlot(void){
  GenDebug("entered.");
  PlotSelectDiaImpl dia;

  if ( dia.exec() ){
    // The Avida Plot widgets are created with the flag
    // Qt::WDestructiveClose,
    // hence we don't have to worry about memory leaks here.
    //AvidaPlotWidget *w = new AvidaPlotWidget( m_pop_wrap, dia.result());
    AvidaPlotWidget *w = new AvidaPlotWidget( &m_mission_control, dia.result());
    //connect( this, SIGNAL( stateChanged() ), w, SLOT( updateData() ) );
    //connect( this, SIGNAL( avidaDestroyed() ), w , SLOT( avidaDestroyed() ) );
    connect(
      &m_mission_control, SIGNAL(avidaUpdatedSig()),
      w, SLOT(updateData())
    );
    //connect(
    //  &m_mission_control, SIGNAL(quitSig()),
    //  w , SLOT(avidaDestroyed())
    //);
    w->show();
  }

  GenDebug("done.");
}

/* vim: set ts=2 ai et: */
