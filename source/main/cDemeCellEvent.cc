#include "cDemeCellEvent.h"
#include "cDeme.h"
#include "cWorld.h"

cDemeCellEvent::cDemeCellEvent(int x1, int y1, int x2, int y2, int delay, int duration, int deme_width, int deme_height, bool static_pos, cDeme* deme, cWorld* world) : 
  m_delay(delay)
  , m_duration(duration)
  , m_iter(0)
  , m_deme_width(deme_width)
  , m_deme_height(deme_height)
  , m_event_width(x2-x1)
  , m_event_height(y2-y1)
  , m_active(false)
  , m_static_pos(static_pos)
  , m_dead(false)
  , m_id_set(false)
  , m_deme(deme)
  , m_world(world)
{
  assert(x1 <= x2);
  assert(y1 <= y2);
  assert(y1 < deme_width && y2 < deme_width);
  assert(x1 < deme_height && x2 < deme_height);
  assert(delay >= -1);
  assert(duration >= 0);
  m_event_cells.ResizeClear((x2-x1+1) * (y2-y1+1));
  int i = 0;
  
  if(m_delay == -1) {
    m_delay = m_world->GetRandom().GetInt(m_world->GetConfig().DEMES_MAX_AGE.Get() - m_duration);
  }
  
  if(m_static_pos) {
    for(int j = y1; j <= y2; j++) {
      for(int k = x1; k <= x2; k++) {
        m_event_cells[i] = j*m_deme_width+k;
        i++;
      }
    }
    pair<int, int> centerBottomleft = make_pair((int)floor(((x2-x1)/2.0)), (int)floor(((y2-y1)/2.0)));
    pair<int, int> centerTopRight = make_pair((int)ceil(((x2-x1)/2.0)), (int)ceil(((y2-y1)/2.0)));
    center = make_pair(centerBottomleft, centerTopRight);
  } else {
    //non-static event position
    m_event_width = x2-x1;
    m_event_height = y2-y1;
  }
}

int cDemeCellEvent::GetNextEventCellID() {
  if(!m_dead && m_active && m_iter < m_event_cells.GetSize()) {
    return m_event_cells[m_iter++];
  }
  m_iter = 0;
  return -1;
}

void cDemeCellEvent::ActivateEvent() {

  if(m_dead) {
    return;
  }
  
  if(!m_id_set)
    m_eventID = m_world->GetRandom().GetInt(0x7FFFFFFF);
  if(!m_static_pos) {
    //non-static event position
    int rand_x1 = m_world->GetRandom().GetInt(m_deme_width-m_event_width);
    int rand_y1 = m_world->GetRandom().GetInt(m_deme_height-m_event_height);
    int i = 0;
    for(int j = rand_y1; j <= rand_y1+m_event_height; j++) {
      for(int k = rand_x1; k <= rand_x1+m_event_width; k++) {
        m_event_cells[i] = j*m_deme_width+k;
        i++;
      }
    }
    pair<int, int> centerBottomleft = make_pair((int)floor((m_event_width/2.0)), (int)floor((m_event_height/2.0)));
    pair<int, int> centerTopRight = make_pair((int)ceil((m_event_width/2.0)), (int)ceil((m_event_height/2.0)));
    center = make_pair(centerBottomleft, centerTopRight);
  }
  
  if(!m_static_pos) {
    //non-static event position
    int rand_x1 = m_world->GetRandom().GetInt(m_deme_width-m_event_width);
    int rand_y1 = m_world->GetRandom().GetInt(m_deme_height-m_event_height);
    int i = 0;
    for(int j = rand_y1; j <= rand_y1+m_event_height; j++) {
      for(int k = rand_x1; k <= rand_x1+m_event_width; k++) {
        m_event_cells[i] = j*m_deme_width+k;
        i++;
      }
    }
  }
  m_active = true;
}

int cDemeCellEvent::GetDelay() {
  if(m_dead){
    return 0;
  }
  return m_delay;
}

int cDemeCellEvent::GetDuration() {
  if(m_dead){
    return 0;
  }
  return m_duration;
}

int cDemeCellEvent::GetEventID() {
  if(m_dead){
    return -1;
  }
  return m_eventID;
}

int cDemeCellEvent::GetEventIDDecay(pair<int, int> pos) {
  if(m_dead){
    return -1;
  }

  double maxd = sqrt(pow(m_deme_width-1.0 ,2) + pow(m_deme_height-1.0 ,2));//cdistance(1,1,wx,wy);
  double top = ceil(maxd);
  pair<double, double> centerPoint = GetCenter(); // # dx.dy -- coordinates of peak value  
  double cdistance = sqrt(pow(pos.first-centerPoint.first ,2) + pow(pos.second-centerPoint.second ,2));

  // return decayed value
  int decayed_value = (int)floor(linmap(cdistance,0.0,top,top*1.414,1.0));
  return decayed_value;
}

std::pair<double, double> cDemeCellEvent::GetCenter() const { 
  return make_pair((center.first.first+center.second.first)/2.0, (center.first.second+center.second.second)/2.0);
}

bool cDemeCellEvent::InCenter(pair<int, int> pos) const {
  if((center.first <= pos) && (pos <= center.second)) {
    return true;
  }
  return false;
}

bool cDemeCellEvent::OnEventCell(int demeRelative_cell_ID) const {
  for(int i = 0; i < m_event_cells.GetSize(); i++) {
    if(m_event_cells[i] == demeRelative_cell_ID)
      return true;
  }
  return false;
}

int cDemeCellEvent::GetTimeToLive() {
  if(m_dead){
    return 0;
  }
  return m_time_to_live;
}

bool cDemeCellEvent::IsActive() {
  if(m_dead){
    return false;
  }
  return m_active;
}

void cDemeCellEvent::DeactivateEvent() {
  m_active = false;
}

double cDemeCellEvent::linmap(const double dp, const double ds, const double de, const double rs, const double re) const {
  double rp;
  if (0.0 == (de - ds)) { // Domain has zero length; exception should be taken
    rp = 0.0;
  } else {
    rp = (dp - ds) * ((re - rs) / (de - ds)) + rs;
  }
  return rp;
}

// should be called before organisms in deme are running
void cDemeCellEvent::ConfineToTimeSlot(int min_delay, int max_delay) {
  assert(!m_active);
  assert(max_delay > min_delay);
  
  m_delay = (m_delay % (max_delay - min_delay)) + min_delay;
}
