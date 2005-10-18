#include "cOrgMessage.h"
#include <string>
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
