#ifndef STRINGABLE_H
#define STRINGABLE_H

#include <string>

class cStringable
{
 public:
  virtual std::string toString( void ) const = 0;
};

#endif
