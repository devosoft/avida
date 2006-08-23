/*
 *  cOrgMessage.cc
 *  Avida
 *
 *  Called "org_message.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cOrgMessage.h"

#include <sstream>

void cOrgMessage::SetData(int in_data)
{
  std::stringstream ss;
  ss << in_data;
  ss >> data;
}

void cOrgMessage::SetData(double in_data)
{
  std::stringstream ss;
  ss << in_data;
  ss >> data;
}

void cOrgMessage::GetData(int & in_data)
{
  in_data=atoi(data.c_str());
}

void cOrgMessage::GetData(double & in_data)
{
  in_data=atof(data.c_str());
}
