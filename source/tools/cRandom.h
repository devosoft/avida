/*
 *  cRandom.h
 *  Avida
 *
 *  Called "random.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2000 California Institute of Technology
 *
 */

#ifndef cRandom_h
#define cRandom_h

#include "cMutex.h"

#include <algorithm>
#include <ctime>
#include <climits>
#include <cmath>
#include <iterator>
#include <vector>

/**
 * A versatile and fast pseudo random number generator.
 **/

template <class T> class tArray;

class cRandom
{
protected:
  // Internal members
  int seed;
  int original_seed;
  int inext;
  int inextp;
  int ma[56];

  // Members & functions for stat functions
  double expRV; // Exponential Random Variable for the randNormal function

#ifdef DEBUG_CRANDOM
  long m_call_count;
#endif
  
  // Constants ////////////////////////////////////////////////////////////////
  // Statistical Approximation
  static const unsigned int _BINOMIAL_TO_NORMAL;  //if < n*p*(1-p)
  static const unsigned int _BINOMIAL_TO_POISSON; //if < n && !Normal approx Engine
  static const unsigned int _RAND_MBIG;
  static const unsigned int _RAND_MSEED;
  // Number Generation
  static const double _RAND_FAC;
  static const double _RAND_mP_FAC;
  static const double _RAND_uP_FAC;
  
  
  
  // Internal functions
  void init();	// Setup  (called by ResetSeed(in_seed);
  
  // Basic Random number
  // Returns a random number [0,_RAND_MBIG)
  virtual unsigned int Get();
  
public:
  /**
   * Set up the random generator object.
   * @param in_seed The seed of the random number generator. 
   * A negative seed means that the random number generator gets its
   * seed from the actual system time.
   **/
  cRandom(const int in_seed = -1);
  virtual ~cRandom() { ; }

  
#ifdef DEBUG_CRANDOM
  inline long GetCallCount() { return m_call_count; }
#endif
  
  /**
   * @return The seed that was actually used to start the random sequence.
   **/
  inline int GetSeed() { return seed; }
  
  /**
   * @return The seed that was originally provided by the user.
   **/
  inline int GetOriginalSeed() { return original_seed; }
  
  /**
   * Starts a new sequence of pseudo random numbers.
   *
   * @param new_seed The seed for the new sequence.
   * A negative seed means that the random number generator gets its
   * seed from the actual system time.
   **/
  virtual void ResetSeed(const int new_seed);
  
  
  // Random Number Generation /////////////////////////////////////////////////
  
  /**
   * Generate a double between 0 and 1.
   *
   * @return The pseudo random number.
   **/
  double GetDouble() { return Get() * _RAND_FAC; }
  
  /**
   * Generate a double between 0 and a given number.
   *
   * @return The pseudo random number.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  double GetDouble(const double max) { return GetDouble() * max; }
  
  /**
   * Generate a double out of a given interval.
   *
   * @return The pseudo random number.
   * @param min The lower bound for the random numbers.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  double GetDouble(const double min, const double max) { return GetDouble() * (max - min) + min; }
  
  /**
   * Generate an unsigned int.
   *
   * @return The pseudo random number.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  unsigned int GetUInt(const unsigned int max) { return static_cast<int>(GetDouble() * static_cast<double>(max)); }
  
  /**
   * Generate an unsigned int out of an interval.
   *
   * @return The pseudo random number.
   * @param min The lower bound for the random numbers.
   * @param max The upper bound for the random numbers (will never be returned).
     **/
  unsigned int GetUInt(const unsigned int min, const unsigned int max) { return GetUInt(max - min) + min; }
  
  /**
   * Generate an int out of an interval.
   *
   * @return The pseudo random number.
   * @param min The lower bound for the random numbers.
   * @param max The upper bound for the random numbers (will never be returned).
   **/
  int GetInt(const int max) { return static_cast<int>(GetUInt(max)); }
  int GetInt(const int min, const int max) { return static_cast<int>(GetUInt(max - min)) + min; }
  
  
  // Random Event Generation //////////////////////////////////////////////////
  
  // P(p) => if p < [0,1) random variable
  bool P(const double _p) { return (Get() < (_p * _RAND_MBIG));}
  bool mP(const double _p) { return (Get() < _RAND_mP_FAC && Get() < (_p * _RAND_MBIG)); } // p = _p*10^-3
  bool uP(const double _p) { return (Get() < _RAND_uP_FAC && Get() < (_p * _RAND_MBIG)); } // p = _p*10^-6
  bool pP(const double _p) { return (Get() < _RAND_uP_FAC && Get() < _RAND_uP_FAC && Get() < (_p * _RAND_MBIG)); } // p = _p*10^-6


  // Other neat stuff /////////////////////////////////////////////////////////
  inline unsigned int MutateByte(unsigned int value);
  inline unsigned int ClearByte(unsigned int value);
  inline unsigned int MutateBit(unsigned int value);
  inline unsigned int MutateBit(unsigned int value, int in_byte);

  bool Choose(int num_in, tArray<int> & out_array);


  // Statistical functions ////////////////////////////////////////////////////

  // Distributions //

  /**
   * Generate a random variable drawn from a unit normal distribution.
   **/
  virtual double GetRandNormal();
  /**
   * Generate a random variable drawn from a distribution with given
   * mean and variance.
   **/
  double GetRandNormal(const double mean, const double variance) { return mean + GetRandNormal() * sqrt(variance); }
  
  /**
   * Generate a random variable drawn from a Poisson distribution.
   **/
  unsigned int GetRandPoisson(const double n, double p) {
    // Optimizes for speed and calculability using symetry of the distribution
    if (p > .5) return (unsigned int)n - GetRandPoisson(n * (1 - p));
    else return GetRandPoisson(n * p);
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
};


/*! This is an adaptor to make cRandom behave like a proper STL random number
 generator.
 */
struct cRandomStdAdaptor {
	typedef int argument_type;
	typedef int result_type;
	
	cRandomStdAdaptor(cRandom& rng) : _rng(rng) { }
	int operator()(int n) { return _rng.GetInt(n); }
	
	cRandom& _rng;
};


/*! Draw a sample (with replacement) from an input range, copying to the output range.
 */
template <typename ForwardIterator, typename OutputIterator, typename RNG>
void sample_with_replacement(ForwardIterator first, ForwardIterator last, OutputIterator ofirst, OutputIterator olast, RNG rng) {
	std::size_t range = std::distance(first, last);
	while(ofirst != olast) {
		*ofirst = *(first+rng(range));
		++ofirst;
	}
}


/*! Convenience function to assign increasing values to a range.
 */
template <typename ForwardIterator, typename T>
void iota(ForwardIterator first, ForwardIterator last, T value) {
	while(first != last) {
		*first = value;
		++first;
		++value;
	}
}


/*! Draw a sample (without replacement) from an input range, copying to the output range.
 */
template <typename ForwardIterator, typename OutputIterator, typename RNG>
void sample_without_replacement(ForwardIterator first, ForwardIterator last, OutputIterator ofirst, OutputIterator olast, RNG rng) {
	std::size_t range = std::distance(first, last);
	std::size_t output_range = std::distance(ofirst, olast);
	
	// if our output range is greater in size than our input range, copy the whole thing.
	if(output_range >= range) {
		std::copy(first, last, ofirst);
		return;
	}
	
	std::vector<std::size_t> rmap(range);
	iota(rmap.begin(), rmap.end(), 0);
	std::random_shuffle(rmap.begin(), rmap.end());
	
	while(ofirst != olast) {
		*ofirst = *(first + rmap.back());
		++ofirst;
		rmap.pop_back();
	}
}

/*! Convenience function to draw samples (without replacement) from a range of values.
 */
template <typename T, typename OutputIterator, typename RNG>
void sample_range_without_replacement(T min, T max, OutputIterator ofirst, OutputIterator olast, RNG rng) {
	std::size_t range = static_cast<std::size_t>(max - min);
	std::vector<T> input(range);
	iota(input.begin(), input.end(), min);
	sample_without_replacement(input.begin(), input.end(), ofirst, olast, rng);
}

	
/*! Choose one element at random from the given range.
 */
template <typename ForwardIterator, typename RNG>
ForwardIterator choose(ForwardIterator first, ForwardIterator last, RNG rng) {
	std::size_t range = std::distance(first, last);
	return first+rng(range);
}


class cRandomMT : public cRandom
{
private:
  cMutex m_mutex;
  
  unsigned int Get();

public:
  cRandomMT(const int in_seed = -1) : cRandom(in_seed) { ; }
  ~cRandomMT() { ; }

  void ResetSeed(const int in_seed);

  double GetRandNormal();
};



inline unsigned int cRandom::MutateByte(unsigned int value)
{
  int byte_pos = 8 * GetUInt(4);
  int new_byte = GetUInt(256);
  value &= ~(255 << byte_pos);
  value |= new_byte << byte_pos;
  return value;
}

inline unsigned int cRandom::ClearByte(unsigned int value)
{
  int byte_pos = 8 * GetUInt(4);
  value &= ~(255 << byte_pos);
  return value;
}

inline unsigned int cRandom::MutateBit(unsigned int value)
{
  int bit_pos = GetUInt(32);
  value ^= (1 << bit_pos);
  return value;
}

inline unsigned int cRandom::MutateBit(unsigned int value, int in_byte)
{
  int bit_pos = (in_byte) * 8 + GetUInt(8);
  value ^= (1 << bit_pos);
  return value;
}


#endif
