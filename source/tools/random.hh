//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/******************************************************************************

cRandom

Random number generator
Random variables from various statistical distributions

******************************************************************************/

#ifndef RANDOM_HH
#define RANDOM_HH

#include <time.h>
#include <limits.h>
#include <math.h>

#ifndef UINT
#define UINT unsigned int
#endif

/**
 * A versatile and fast pseudo random number generator.
 **/

template <class T> class tArray;

class cRandom{
public:
  /**
   * Set up the random generator object.
   * @param in_seed The seed of the random number generator. 
   * A negative seed means that the random number generator gets its
   * seed from the actual system time.
   **/
  cRandom(const int in_seed=-1);
  
  inline int GetUseCount() { return use_count; }
  
  
  /**
   * @return The seed that was actually used to start the random sequence.
   **/
  inline int GetSeed(){ return seed; }
  
  /**
   * @return The seed that was originally provided by the user.
   **/
  inline int GetOriginalSeed(){ return original_seed; }
  
  /**
   * Starts a new sequence of pseudo random numbers.
   *
   * @param new_seed The seed for the new sequence.
   * A negative seed means that the random number generator gets its
   * seed from the actual system time.
   **/
  void ResetSeed(const int new_seed);
  
  
  // Random Number Generation /////////////////////////////////////////////////
  
  /**
   * Generate a double between 0 and 1.
   *
   * @return The pseudo random number.
   **/
  inline double GetDouble(){ return Get()*_RAND_FAC; }
  
  /**
   * Generate a double between 0 and a given number.
   *
   * @return The pseudo random number.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  inline double GetDouble(const double max){ return GetDouble() * max;}
  
  /**
   * Generate a double out of a given interval.
   *
   * @return The pseudo random number.
   * @param min The lower bound for the random numbers.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  inline double GetDouble(const double min, const double max){
    return GetDouble()*(max-min)+min;}
  
  /**
   * Generate an unsigned int.
   *
   * @return The pseudo random number.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  inline unsigned int GetUInt(const unsigned int max){
    return (int) (GetDouble()*max);}
  
  /**
   * Generate an unsigned int out of an interval.
   *
   * @return The pseudo random number.
   * @param min The lower bound for the random numbers.
   * @param max The upper bound for the random numbers (will never be returned).
     **/
  inline unsigned int GetUInt(const unsigned int min, const unsigned int max){
    return GetUInt(max-min+1)+min; }
  
  /**
   * Generate an int out of an interval.
   *
   * @return The pseudo random number.
   * @param min The lower bound for the random numbers.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  inline int GetInt(const int max){
    return (int)GetUInt(max); }
  inline int GetInt(const int min, const int max){
    return ((int)GetUInt(max-min+1))+min; }
  
  
  // Random Event Generation //////////////////////////////////////////////////
  
  // P(p) => if p < [0,1) random variable
  inline bool P(const double _p){
    return (Get()<(_p*_RAND_MBIG));}
  inline bool mP(const double _p){	// p = _p*10^-3
    return (Get()<_RAND_mP_FAC && Get()<(_p*_RAND_MBIG));}
  inline bool uP(const double _p){	// p = _p*10^-6
    return (Get()<_RAND_uP_FAC && Get()<(_p*_RAND_MBIG));}
  inline bool pP(const double _p){	// p = _p*10^-6
    return (Get()<_RAND_uP_FAC && Get()<_RAND_uP_FAC &&
	    Get()<(_p*_RAND_MBIG));}


  // Other neat stuff /////////////////////////////////////////////////////////
  inline UINT MutateByte(UINT value);
  inline UINT ClearByte(UINT value);
  inline UINT MutateBit(UINT value);
  inline UINT MutateBit(UINT value, int in_byte);

  bool Choose(int num_in, tArray<int> & out_array);


  // Statistical functions ////////////////////////////////////////////////////

  // Distributions //

  /**
   * Generate a random variable drawn from a unit normal distribution.
   **/
  double GetRandNormal();
  /**
   * Generate a random variable drawn from a distribution with given
   * mean and variance.
   **/
  inline double GetRandNormal(const double mean, const double variance){
    return mean+GetRandNormal()*sqrt(variance);
  }
  
  /**
   * Generate a random variable drawn from a Poisson distribution.
   **/
  inline unsigned int GetRandPoisson(const double n, double p) {
    // Optimizes for speed and calculability using symetry of the distribution
    if( p>.5 ) return (unsigned int)n-GetRandPoisson(n*(1-p));
    else return GetRandPoisson(n*p);
  }
  
  /**
   * Generate a random variable drawn from a Poisson distribution.
   *
   * @param mean The mean of the distribution.
   **/
  unsigned int GetRandPoisson(const double mean);
  
  /**
   * Generate a random variable drawn from a Binomial distribution.
   * 
   * This function is exact, but slow. 
   * @see cRandom::GetRandBinomial
   **/
  unsigned int GetFullRandBinomial(const double n, const double p); // Exact
  
  /**
   * Generate a random variable drawn from a Binomial distribution.
   * 
   * This function is faster than @ref cRandom::GetFullRandBinomial(), but 
   * uses some approximations.
   *
   * @see cRandom::GetFullRandBinomial
   **/  
  unsigned int GetRandBinomial(const double n, const double p); // Approx
  
  
  // Internals ////////////////////////////////////////////////////////////////
private:
  // Internal memebers
  int seed;
  int original_seed;
  int inext;
  int inextp;
  int ma[56];
  int use_count;

  // Constants ////////////////////////////////////////////////////////////////
  // Statistical Approximation
  static const unsigned int _BINOMIAL_TO_NORMAL;  //if < n*p*(1-p)
  static const unsigned int _BINOMIAL_TO_POISSON; //if < n && !Normal approx
  // Engine
  static const unsigned int _RAND_MBIG;
  static const unsigned int _RAND_MSEED;
  // Number Generation
  static const double _RAND_FAC;
  static const double _RAND_mP_FAC;
  static const double _RAND_uP_FAC;

  // Members & functions for stat functions
  double expRV; // Exponential Random Variable for the randNormal function

  // Internal functions
  void init();	// Setup  (called by ResetSeed(in_seed);
  void initStatFunctions();

  // Basic Random number
  // Returns a random number [0,_RAND_MBIG)
  inline unsigned int Get(){
    // use_count++;  // Turn this on if random uses need to be tracked.

    if (++inext == 56) inext = 0;
    if (++inextp == 56) inextp = 0;
    int mj = ma[inext] - ma[inextp];
    if (mj < 0) mj += _RAND_MBIG;
    ma[inext] = mj;
    return mj;
  }

};

inline UINT cRandom::MutateByte(UINT value) {
  int byte_pos = 8 * GetUInt(4);
  int new_byte = GetUInt(256);
  value &= ~(255 << byte_pos);
  value |= new_byte << byte_pos;
  return value;
}

inline UINT cRandom::ClearByte(UINT value) {
  int byte_pos = 8 * GetUInt(4);
  value &= ~(255 << byte_pos);
  return value;
}

inline UINT cRandom::MutateBit(UINT value) {
  int bit_pos = GetUInt(32);
  value ^= (1 << bit_pos);
  return value;
}

inline UINT cRandom::MutateBit(UINT value, int in_byte) {
  int bit_pos = (in_byte) * 8 + GetUInt(8);
  value ^= (1 << bit_pos);
  return value;
}

#endif
