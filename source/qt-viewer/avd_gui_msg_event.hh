#ifndef AVD_GUI_MSG_EVENT_HH
#define AVD_GUI_MSG_EVENT_HH

#ifndef QEVENT_H
#include <qevent.h>
#endif


class avd_GuiMsgEvent : public QCustomEvent {
public:
  enum { EventType = 1001 };
  avd_GuiMsgEvent(void): QCustomEvent(EventType){}
};


#endif /* !AVD_GUI_MSG_EVENT_HH */

