#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef ANALYZE_HH
#include "main/analyze.hh"
#endif

#ifndef AVD_GUI_FACTORY_HH
#include "gui_factory/avd_gui_factory.hh"
#endif
#ifndef AVD_ANALYZER_VIEW_HH
#include "avd_analyzer_view.hh"
#endif
#ifndef AVD_ANALYZER_CONTROLLER_DATA_HH
#include "avd_analyzer_controller_data.hh"
#endif
#ifndef AVD_ANALYZER_MODEL_HH
#include "avd_analyzer_model.hh"
#endif
#ifndef AVD_ANALYZER_UTILS_HH
#include "avd_analyzer_utils.hh"
#endif

#ifndef AVD_ANALYZER_CONTROLLER_HH
#include "avd_analyzer_controller.hh"
#endif


namespace {
  avdAbstractCtrl *createAnalyzerController(void){
    avd_o_AnalyzerCtrl *ac = new avd_o_AnalyzerCtrl;
    return ac;
  }
  QString s_key("avd_o_AnalyzerCtrl");
  const bool s_registered =
    avdControllerFactory::Instance().Register(s_key, createAnalyzerController);
}

avd_o_AnalyzerCtrl::avd_o_AnalyzerCtrl(){
  AnGuiDebug("entered; creating controller data.");
  d = new avd_o_AnalyzerCtrlData;
  if(d){
    connect( d->m_analyzer_view, SIGNAL(destroyed()),
      this, SLOT(deleteLater()));
  }
  AnGuiDebug("created controller data; done.");
}
avd_o_AnalyzerCtrl::~avd_o_AnalyzerCtrl(){
  AnGuiDebug("entered destructor; deleting controller data.");
  if(d){ delete d; }
  AnGuiDebug("deleted controller data; done.");
}

bool avd_o_AnalyzerCtrl::setup(avdMCMediator *mediator)
{
  AnGuiDebug("entered a stub function; done.");
  if(d && d->m_analyzer_view){ d->m_analyzer_view->show(); }
  return true;
}

// arch-tag: implementation file for old analyzer gui controller
