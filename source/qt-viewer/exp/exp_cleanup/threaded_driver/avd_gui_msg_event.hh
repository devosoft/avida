#ifndef AVD_GUI_MSG_EVENT_HH
#define AVD_GUI_MSG_EVENT_HH

#ifndef QEVENT_H
#include <qevent.h>
#endif


class avdGuiMsgEvent : public QCustomEvent {
public:
  enum eEventType { EventType = 1002 };
  avdGuiMsgEvent(void): QCustomEvent(EventType){}
};


#endif

// arch-tag: header file for debug-message display events
