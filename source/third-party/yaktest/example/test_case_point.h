#ifndef TEST_POINT_H
#define TEST_POINT_H

#ifndef TEST_CASE_H
#include "cTestCase.hh"
#endif
#ifndef POINT_H
#include "point.h"
#endif

class point_test : public cTestCase
{
 protected:
  yak::int_point top_left_point;
  yak::int_point bottom_right_point;
 public:
  virtual void set_up( void );
};

class point_test_suite : public cTestCase
{
 public:
  point_test_suite( void );
};

#endif
