//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cMerit.h"


using namespace std;


void cMerit::UpdateValue(double in_value){
  const int max_bits = sizeof(unsigned int)*8;
  static double mult[max_bits];
  static bool mult_initilalized = false;

  // Do not allow negative merits.
  if (in_value < 0.0) in_value = 0.0;

  // Initilize multipliers only once
  if( mult_initilalized == false ){
    //cout<<"initializing multipliers"<<endl;
    mult_initilalized = true;
    for( int i=0; i<max_bits; ++i ){
      mult[i] = pow((double)2,i);
      //cout<<"  mult["<<i<<"] = "<<mult[i]<<endl;
    }
  }


  value = in_value;

  double mant = frexp (value , &bits);

  //cout<<value<<" = "<<mant<<" * 2 ^ "<<bits<<endl;

  if( bits > max_bits ){
    offset = bits - max_bits;
  }else{
    offset = 0;
  }

  base = (unsigned int) (mant * mult[bits-offset-1] * 2 );

  //cout<<value<<" = "<<base<<" ["<<bits<<" bits] "<<" * 2 ^ "<<offset;
  //cout<<" = "<<(base * pow((double)2,offset))<<endl;
}


ostream & cMerit::BinaryPrint(ostream & os) const {
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




ostream & operator<<(ostream & os, const cMerit & merit){
  os<<merit.GetDouble();
  return os;
}


