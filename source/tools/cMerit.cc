/*
 *  cMerit.cc
 *  Avida
 *
 *  Called "merit.cc" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology
 *
 */

#include "cMerit.h"

using namespace std;


void cMerit::UpdateValue(double in_value)
{
  const int max_bits = sizeof(unsigned int)*8;
  static double mult[max_bits];
  static bool mult_initilalized = false;

  // Do not allow negative merits.
  if (in_value < 0.0) in_value = 0.0;

  // Initilize multipliers only once
  if( mult_initilalized == false ){
    mult_initilalized = true;
    for( int i=0; i<max_bits; ++i ){
      mult[i] = pow((double)2,i);
    }
  }

  value = in_value;

  double mant = frexp (value , &bits);

  if( bits > max_bits ){
    offset = bits - max_bits;
  }else{
    offset = 0;
  }

  base = (unsigned int) (mant * mult[bits-offset-1] * 2 );
}


ostream& cMerit::BinaryPrint(ostream& os) const {
  for( int i=GetNumBits()-1; i>=0; --i ){
    os<<GetBit(i);
  }
  return os;
}


bool cMerit::OK() const {
  double test_value = (double)base * pow((double)2,(int)offset);
  int test_bits = (int)(log(value)/log((double)2)) + 1;
  if( base == 0 ) test_bits = 0;

  // Uncomment block for debugging output and assertion of OK
  /*
  cout<<"cMerit Ok? ";
  cout<<base<<" * 2^"<<offset<<" = "<<test_value<<" ~= "<<value<<endl;
  cout<<"\tnum_bits="<<GetNumBits()<<" ?= "<<test_bits<<endl;
  BinaryPrint(cout)<<endl;

  */
  assert ( test_bits == bits &&
	   ( test_value <= value * (1 + 1/UINT_MAX) ||
	     test_value >= value / (1 + 1/UINT_MAX) ) );

  return ( test_bits == bits &&
	   ( test_value <= value * (1 + 1/UINT_MAX) ||
	     test_value >= value / (1 + 1/UINT_MAX) ) );
}




ostream& operator<<(ostream& os, const cMerit & merit){
  os<<merit.GetDouble();
  return os;
}


