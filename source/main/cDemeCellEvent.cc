#include "cDemeCellEvent.h"
#include "cWorld.h"

cDemeCellEvent::cDemeCellEvent(int x1, int y1, int x2, int y2, int delay, int duration, int width) : 
  m_delay(delay)
  , m_duration(duration)
  , m_iter(0)
  , m_active(false)
{
  assert(x1 <= x2);
  assert(y1 <= y2);
  assert(y1 < width && y2 < width);
  assert(delay >= 0);
  assert(duration >= 0);
  m_event_cells.ResizeClear((x2-x1+1) * (y2-y1+1));
  
  int i = 0;
  for(int j = y1; j <= y2; j++) {
    for(int k = x1; k <= x2; k++) {
      m_event_cells[i] = j*width+k;
      i++;
    }
  }
}

int cDemeCellEvent::GetNextEventCellID() {
  if(m_active && m_iter < m_event_cells.GetSize()) {
    return m_event_cells[m_iter++];
  }
  m_iter = 0;
  return -1;
}

void cDemeCellEvent::ActivateEvent(cWorld* m_world) {
  m_eventID = m_world->GetRandom().GetInt(0x7FFFFFFF);
  m_active = true;
}

void cDemeCellEvent::DeactivateEvent() {
  m_active = false;
}
