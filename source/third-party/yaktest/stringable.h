#ifndef STRINGABLE_H
#define STRINGABLE_H

#include <string>

class stringable
{
 public:
  virtual std::string to_string( void ) const = 0;
};

#endif
