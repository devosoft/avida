#ifndef cDemeCellEvent_h
#define cDemeCellEvent_h

#include "tArray.h"
class cWorld;

class cDemeCellEvent {
private:
  tArray<int> m_event_cells;
  int m_delay, m_duration, m_eventID, m_iter;
  bool m_active;
  
public:
  cDemeCellEvent() { cDemeCellEvent(-1, -1, -1, -1, 0, 0, 0); }
  cDemeCellEvent(int x1, int y1, int x2, int y2, int delay, int duration, int width);
  int GetNextEventCellID();
  int GetDelay() { return m_delay; }
  int GetDuration() { return m_duration; }
  int GetEventID() { return m_eventID; }

  void ActivateEvent(cWorld* m_world);
  void DeactivateEvent();
};
#endif
