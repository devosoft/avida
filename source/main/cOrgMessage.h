/*
 *  cOrgMessage.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cOrgMessage_h
#define cOrgMessage_h

#include <string>

class cOrgMessage
{
private:
  int time;
  int sender_id;
  int recipient_id;
  std::string label;
  std::string data;

public:
  cOrgMessage() : time(-1), sender_id(-1), recipient_id(-1), label(""), data("") { ; }
  cOrgMessage(std::string in_label, std::string in_data) { label = in_label; data = in_data; }
  
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


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
