#ifndef AVD_MESSAGE_TYPE_HH
#include "avd_message_type.hh"
#endif

#ifndef AVD_MESSAGE_CLASS_BASE_HH
#include "avd_message_class_base.hh"
#endif 

avdMessageType::avdMessageType(
  const char *type_name,
  avdMessageClassBase &message_class
):m_type_name(type_name)
, m_class_name(message_class.className())
, m_is_fatal(message_class.isFatal())
, m_is_prefix(message_class.isPrefix())
, m_no_prefix(message_class.noPrefix())
, m_msg_display(message_class.messageDisplay())
{
  setIsActive(true);
  setShowWhere(true);
  setShowFunction(true);
}
