//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ORG_MESSAGE_HH
#define ORG_MESSAGE_HH

#include <string>

class cOrgMessage {
private:
  int time;
  int sender_id;
  int recipient_id;
  std::string label;
  std::string data;

public:
  cOrgMessage() { time=sender_id=recipient_id=-1; label=data=""; }
  cOrgMessage(std::string in_label, std::string in_data) 
  { label = in_label; data = in_data; }
  
  void SetTime(int in_time) { time = in_time; }
  void SetSenderID(int in_id) { sender_id = in_id; }
  void SetRecipientID(int in_id) { recipient_id = in_id; }
  void SetLabel(int in_label) { label = in_label; }
  
  void SetData(std::string in_data) { data = in_data; }
  void SetData(int in_data);
  void SetData(double in_data);

  void GetData(std::string & in_data) { in_data=data; }
  void GetData(int & in_data);
  void GetData(double & in_data);
};

#endif
