#ifndef AVD_CONNECTIONS_HH
#define AVD_CONNECTIONS_HH

class QObject;
class avdConnections {
private:
  /* disabled */ avdConnections();
  /* disabled */ avdConnections(const avdConnections &);
  /* disabled */ avdConnections& operator=(const avdConnections &);
protected:
  const QObject *m_sender, *m_receiver;
public:
  avdConnections(const QObject *sender, const QObject *receiver)
  : m_sender(sender), m_receiver(receiver){}
  bool add(const char *sender_signal, const char *receiver_member);
};

#endif
