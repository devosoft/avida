#ifndef cDemeCellEvent_h
#define cDemeCellEvent_h

#include <utility>
#include "tArray.h"

class cDeme;
class cWorld;

class cDemeCellEvent {
private:
  tArray<int> m_event_cells;
  int m_eventID;
  int m_delay, m_duration, m_iter, m_deme_width, m_deme_height;
  int m_event_width, m_event_height, m_time_to_live;
  bool m_use_gradient, m_active, m_static_pos, m_dead, m_id_set;
  std::pair<std::pair<int, int>, std::pair<int, int> > center;
  cDeme* m_deme; 
  cWorld* m_world;
  
  std::pair<double, double> GetCenter() const;
  
public:
  cDemeCellEvent() { cDemeCellEvent(-1, -1, -1, -1, 0, 0, 0, 0, true, NULL, NULL); }
  cDemeCellEvent(int x1, int y1, int x2, int y2, int delay, int duration, int deme_width, int deme_height, bool static_pos, cDeme* deme, cWorld* world);
  int GetNextEventCellID();
  int GetDelay();
  cDeme* GetDeme() const { return m_deme; }
  int GetDuration();
  int GetEventID();
  int GetEventIDDecay(std::pair<int, int> pos);
  int GetTimeToLive();
  bool IsActive();
  bool IsDead() const { return m_dead; }
  bool IsDecayed() { return m_use_gradient; }
  bool InCenter(std::pair<int, int> pos) const;
  bool OnEventCell(int demeRelative_cell_ID) const;
  
  void SetEventID(int ID) { m_eventID = ID; m_id_set = true;}

  void DecayEventIDFromCenter() { m_use_gradient = true; }
  void ActivateEvent();
  void DeactivateEvent();
  void Terminate() { m_dead = true; }
  
  // gradient generators
  double linmap(const double dp, const double ds, const double de, const double rs, const double re) const;
  
  void ConfineToTimeSlot(int min_delay, int max_delay);
};
#endif
