#ifndef QAPPLICATION_H
#include <qapplication.h>
#endif
#ifndef QDATETIME_H
#include <qdatetime.h>
#endif
#ifndef QEVENT_H
#include <qevent.h>
#endif
#ifndef QSTRINGLIST_H
#include <qstringlist.h>
#endif
#ifndef QTIMER_H
#include <qtimer.h>
#endif

#ifndef GENEBANK_HH
#include "main/genebank.hh"
#endif
#ifndef GENOTYPE_HH
#include "main/genotype.hh"
#endif
#ifndef POPULATION_CELL_HH
#include "main/population_cell.hh"
#endif
#ifndef HARDWARE_FACTORY_HH
#include "cpu/hardware_factory.hh"
#endif
#ifndef ORGANISM_HH
#include "main/organism.hh"
#endif
#ifndef HARDWARE_BASE_HH
#include "cpu/hardware_base.hh"
#endif
#ifndef STRING_HH
#include "tools/string.hh"
#endif
#ifndef CONFIG_HH
#include "main/config.hh"
#endif
#ifndef POPULATION_HH
#include "main/population.hh"
#endif

#ifndef AVD_AVIDA_THREAD_DRVR_DBG_HH
#include "avd_avida_thread_drvr_dbg.hh"
#endif
#ifndef AVD_GUI_MSG_EVENT_HH
#include "avd_gui_msg_event.hh"
#endif

#ifndef AVD_AVIDA_DRIVER_HH
#include "avd_avida_driver.hh"
#endif

using namespace std;


avdAvidaThreadDrvr::avdAvidaThreadDrvr(cEnvironment &environment)
: cAvidaDriver_Population(environment)
{
  m_mode_function = &avdAvidaThreadDrvr::fastMode;
  m_fast_stage_function = &avdAvidaThreadDrvr::preUpdate;
  m_step_stage_function = &avdAvidaThreadDrvr::preUpdate;
  m_emit_function = &avdAvidaThreadDrvr::emitUpdatedSig;
  m_run_continuously = false;
  m_step_mode = false;
  m_step_cell_id = 0;
  m_breakpoint_cell_id = 0;
  m_mutex = new QMutex(false);
}

avdAvidaThreadDrvr::~avdAvidaThreadDrvr(void){
  doCloseAvidaSlot();
  delete m_mutex;
}

void avdAvidaThreadDrvr::Run(void){
  /*
  Qt/X11 doesn't check for non-X11 events except in passing when an X11
  event occurs.  The 50-millisecond timer below doesn't connect to
  anything, but timer events are really X11 events, and they make Qt
  check for notification events from the Avida processing thread to the
  gui thread.
  
  OSX doesn't require this; probably neither does Windows.  We'll see.
  */
  startTimer(50);
  start(); /* Starts the processing thread. */
}

void avdAvidaThreadDrvr::doStartAvidaSlot(){ wakeProcessingThread(true, false); }
void avdAvidaThreadDrvr::doUpdateAvidaSlot(){ wakeProcessingThread(false, false); }
void avdAvidaThreadDrvr::doStepAvidaSlot(int cell_id){
  m_mutex->lock();
  m_step_cell_id = cell_id;
  m_mutex->unlock();
  wakeProcessingThread(false, true);
}
void avdAvidaThreadDrvr::doPauseAvidaSlot(){
  m_mutex->lock();
  m_run_continuously = false;
  m_mutex->unlock();
}
void avdAvidaThreadDrvr::doCloseAvidaSlot(){
  m_mutex->lock();
  m_mode_function = &avdAvidaThreadDrvr::closingMode;
  m_mutex->unlock();
  m_wait_condition.wakeAll();
  wait();
}
void avdAvidaThreadDrvr::wakeProcessingThread(bool continuous, bool step){
  m_mutex->lock();
  m_run_continuously = continuous;
  m_step_mode = step;
  m_mutex->unlock();
  m_wait_condition.wakeAll();
}

/* Main loop of the Avida processing thread. */
void avdAvidaThreadDrvr::run(void){
  m_mutex->lock();
  for(;;){
    m_wait_condition.wait(m_mutex);
    while((this->*(m_mode_function))())
    { m_mutex->unlock(); m_mutex->lock(); }
    QApplication::postEvent(this, new avdGuiMsgEvent);
} }

bool avdAvidaThreadDrvr::fastMode(){ return (this->*(m_fast_stage_function))(); }
bool avdAvidaThreadDrvr::stepMode(){ return (this->*(m_step_stage_function))(); }
bool avdAvidaThreadDrvr::closingMode(){
  m_emit_function = &avdAvidaThreadDrvr::emitClosedSig;
  m_mutex->unlock();
  QApplication::postEvent(this, new avdGuiMsgEvent);
  exit(); /* Terminates the processing thread. */
  return false; /* never reached. */
}

bool avdAvidaThreadDrvr::preUpdate() {
  GetEvents();
  if (done_flag == true){
    m_emit_function = &avdAvidaThreadDrvr::emitClosedSig;
    return false; }
  // Increment the Update.
  cStats & stats = population->GetStats();
  stats.IncCurrentUpdate();
  // Handle all data collection for previous update.
  if (stats.GetUpdate() > 0) {
    // Tell the stats object to do update calculations and printing.
    stats.ProcessUpdate();
    // Update all the genotypes for the end of this update.
    cGenebank & genebank = population->GetGenebank();
    for(cGenotype * cur_genotype = genebank.ResetThread(0);
	      cur_genotype != NULL && cur_genotype->GetThreshold();
	      cur_genotype = genebank.NextGenotype(0)){
      cur_genotype->UpdateReset();
  } }
  m_UD_size = cConfig::GetAveTimeslice() * population->GetNumOrganisms();
  m_step_size = 1. / (double) m_UD_size;
  m_UD_progress = 0;
  m_fast_stage_function = &avdAvidaThreadDrvr::fastUpdate;
  m_step_stage_function = &avdAvidaThreadDrvr::stepUpdate;
  return true;
}
bool avdAvidaThreadDrvr::fastUpdate() {
  const int bite_size = cConfig::GetAveTimeslice();
  for(int i = 0; i < bite_size; i++){
    if(m_UD_progress < m_UD_size){
      m_UD_progress++;
      m_current_cell_id = population->ScheduleOrganism();
      population->ProcessStep(m_step_size, m_current_cell_id); }
    else {
      m_fast_stage_function = &avdAvidaThreadDrvr::postUpdate;
      m_step_stage_function = &avdAvidaThreadDrvr::postUpdate;
      return true;
  } }
  return true;
}
bool avdAvidaThreadDrvr::stepUpdate() {
  const int bite_size = cConfig::GetAveTimeslice();
  for(int i = 0; i < bite_size; i++){
    if(m_UD_progress < m_UD_size){
      m_UD_progress++;
      m_current_cell_id = population->ScheduleOrganism();
      population->ProcessStep(m_step_size, m_current_cell_id);
      if (m_current_cell_id == m_step_cell_id){
        //cout << "Stepping through instructions in cell "
        //  << m_current_cell_id << "." << endl;
        Message << "Stepping through instructions in cell "
                << m_current_cell_id << ".";
        m_emit_function = &avdAvidaThreadDrvr::emitUpdatedSig;
        return false;
    } }
    else {
      m_fast_stage_function = &avdAvidaThreadDrvr::postUpdate;
      m_step_stage_function = &avdAvidaThreadDrvr::postUpdate;
      return true;
  } }
  return true;
}
bool avdAvidaThreadDrvr::postUpdate() {
  // end of update stats...
  population->CalcUpdateStats();
  // No viewer; print out status for this update....
  cStats & stats = population->GetStats();
  //cout << "UD: "  << stats.GetUpdate() << "\t"
  //     << "Gen: " << stats.SumGeneration().Average() << "\t"
  //     << "Fit: " << stats.GetAveFitness() << "\t"
  //     << "Size: " << population->GetNumOrganisms()
  //     << endl;
  Message << "UD: "  << stats.GetUpdate() << "\t"
          << "Gen: " << stats.SumGeneration().Average() << "\t"
          << "Fit: " << stats.GetAveFitness() << "\t"
          << "Size: " << population->GetNumOrganisms();
  // Check whether to do point mutations
  if (cConfig::GetPointMutProb() > 0 ) {
    m_mutations_progress = 0;
    m_fast_stage_function = &avdAvidaThreadDrvr::ptMutations;
    m_step_stage_function = &avdAvidaThreadDrvr::ptMutations; }
  else {
    m_fast_stage_function = &avdAvidaThreadDrvr::postPtMutations;
    m_step_stage_function = &avdAvidaThreadDrvr::postPtMutations; }
  return true;
}
bool avdAvidaThreadDrvr::ptMutations() {
  if (m_mutations_progress < population->GetSize()) {
    if (population->GetCell(m_mutations_progress).IsOccupied()) {
	    population->GetCell(m_mutations_progress).GetOrganism()->
	      GetHardware().PointMutate(cConfig::GetPointMutProb()); }
    m_mutations_progress++; }
  else {
    m_fast_stage_function = &avdAvidaThreadDrvr::postPtMutations;
    m_step_stage_function = &avdAvidaThreadDrvr::postPtMutations; }
  return true;
}
bool avdAvidaThreadDrvr::postPtMutations() {
#ifdef DEBUG_CHECK_OK
  // If we're in a debug mode, make sure the population is OK.
  if (population->OK() == false)
  { g_debug.Warning("Population::OK() is failing."); }
#endif
  // Do any cleanup in the hardware factory...
  cHardwareFactory::Update();
  // Exit conditons...
  if (population->GetNumOrganisms() == 0){
    done_flag = true;
    m_emit_function = &avdAvidaThreadDrvr::emitClosedSig;
  } else m_emit_function = &avdAvidaThreadDrvr::emitUpdatedSig;
  m_fast_stage_function = &avdAvidaThreadDrvr::preUpdate;
  m_step_stage_function = &avdAvidaThreadDrvr::preUpdate;
  return false;
}

void avdAvidaThreadDrvr::SignalBreakpoint(void){
  m_breakpoint_cell_id = m_current_cell_id;
  m_emit_function = &avdAvidaThreadDrvr::emitBreakpointSig;
  QApplication::postEvent(this, new avdGuiMsgEvent);
  m_wait_condition.wait(m_mutex);
}

void avdAvidaThreadDrvr::emitUpdatedSig(){
  emit(avidaUpdatedSig(this));
  if(m_run_continuously){ emit(doStartAvidaSig()); }
}
void avdAvidaThreadDrvr::emitSteppedSig()
{ emit(avidaSteppedSig(this, m_step_cell_id)); }
void avdAvidaThreadDrvr::emitBreakpointSig()
{ emit(avidaBreakSig(this, m_breakpoint_cell_id)); }
void avdAvidaThreadDrvr::emitClosedSig()
{ emit(avidaClosedSig(this)); }
void avdAvidaThreadDrvr::customEvent(QCustomEvent *custom_event){
  if (custom_event->type() == static_cast<int>(avdGuiMsgEvent::EventType)){
    (this->*(m_emit_function))();
} }

// arch-tag: implementation file for avida processing thread
