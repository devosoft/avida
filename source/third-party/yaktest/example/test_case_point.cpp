#include "test_case_point.h"


class default_constructor_test : public point_test
{
  virtual void test( void ) 
    {
      yak::int_point test_point;
      test_is_equal( test_point, yak::int_point( 0, 0 ) );
    }
};

class specific_constructor_test : public point_test
{
  virtual void test( void ) 
    {
      test_is_equal( top_left_point, yak::int_point( 1, 2 ) );
    }
};

class copy_constructor_test : public point_test
{
  virtual void test( void )
    {
      yak::int_point test_point( top_left_point );
      test_is_equal( test_point, yak::int_point( 1, 2 ) );
    }
};

class equality_operator_test : public point_test
{
  virtual void test(void )
    {
      yak::int_point test_equal_point( 1, 2 );
      test_is_true( top_left_point == top_left_point );
      test_is_true( top_left_point == test_equal_point );
    }
};

class inequality_operator_test : public point_test
{
  virtual void test( void )
    {
      yak::int_point first_equal_point( 1, 9 );
      yak::int_point second_equal_point( 9, 2 );
      test_is_true( top_left_point != first_equal_point );
      test_is_true( top_left_point != second_equal_point );
      test_is_true( top_left_point != bottom_right_point );
    }
};

class self_addition_test : public point_test
{
  virtual void test( void )
    {
      yak::int_point test_point( 10, 20 );
      test_point += top_left_point;
      test_is_equal( test_point, yak::int_point( 11, 22 ) );
    }
};

class self_subtraction_test : public point_test
{
  virtual void test( void )
    {
      yak::int_point test_point( 10, 20 );
      test_point -= top_left_point;
      test_is_equal( test_point, yak::int_point( 9, 18 ) );
    }
};

class addition_test : public point_test
{
virtual void test( void )
    {
      yak::int_point test_point = top_left_point + bottom_right_point;
      test_is_equal( test_point, yak::int_point( 4, 6 ) );
    }
};

class subtraction_test : public point_test
{
  virtual void test( void )
    {
      yak::int_point test_point = top_left_point - bottom_right_point;
      test_is_equal( test_point, yak::int_point( -2, -2 ) );
    }
};

class copy_operator_test : public point_test
{
  virtual void test( void )
    {
      yak::int_point test_point;
      test_point = top_left_point;
      test_is_equal( test_point, top_left_point );
    }
};

point_test_suite::point_test_suite( void ) :
cTestCase()
{
  adoptTestCase( new default_constructor_test );
  adoptTestCase( new specific_constructor_test );
  adoptTestCase( new copy_constructor_test );
  adoptTestCase( new equality_operator_test );
  adoptTestCase( new inequality_operator_test );
  adoptTestCase( new self_addition_test );
  adoptTestCase( new self_subtraction_test );
  adoptTestCase( new addition_test );
  adoptTestCase( new subtraction_test );
  adoptTestCase( new copy_operator_test );
}

void 
point_test::set_up( void ) 
{
  top_left_point = yak::int_point( 1, 2 );
  bottom_right_point = yak::int_point( 3, 4 );
}
