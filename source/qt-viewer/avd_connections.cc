#ifndef QOBJECT_H
#include <qobject.h>
#endif

#ifndef AVD_CONNECTIONS_HH
#include "avd_connections.hh"
#endif

bool avdConnections::add(const char *sender_signal, const char *receiver_member)
{ return QObject::connect(m_sender, sender_signal, m_receiver, receiver_member); }

