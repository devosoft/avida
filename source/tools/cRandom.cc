/*
 *  cRandom.cc
 *  Avida
 *
 *  Called "random.cc" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2000 California Institute of Technology
 *
 */

#include "cRandom.h"

#include "tArray.h"

#ifdef WIN32
#include <process.h>
#else
#include <unistd.h>
#endif


// Constants //////////////////////////////////////////////////////////////////

// Statistical Approximation
const unsigned int cRandom::_BINOMIAL_TO_NORMAL = 50;    //if < n*p*(1-p)
const unsigned int cRandom::_BINOMIAL_TO_POISSON = 1000; //if < n

// Engine
const unsigned int cRandom::_RAND_MBIG = 1000000000;
const unsigned int cRandom::_RAND_MSEED = 161803398;

// Number Generation
const double cRandom::_RAND_FAC = (1.0/_RAND_MBIG);
const double cRandom::_RAND_mP_FAC = (_RAND_MBIG/1000);
const double cRandom::_RAND_uP_FAC = (_RAND_MBIG/1000000);


// Constructor and setup //////////////////////////////////////////////////////

cRandom::cRandom(const int in_seed)
: seed(0), original_seed(0), inext(0), inextp(0), expRV(0)
{
  for (int i = 0; i < 56; ++i) {
    ma[i] = 0;
  }
  ResetSeed(in_seed);  // Calls init()
}


void cRandom::ResetSeed(const int in_seed)
{
  original_seed = in_seed;
  
  if (in_seed <= 0) {
    int seed_time = (int) time(NULL);
#ifdef MSVC_COMPILER
    int seed_pid = (int) _getpid(); 
#else
    int seed_pid = (int) getpid(); 
#endif
    seed = seed_time ^ (seed_pid << 8);
  } else {
    seed = in_seed;
  }
  
  if (seed < 0) seed *= -1;
  seed %= _RAND_MSEED;

  init();
}

void cRandomMT::ResetSeed(const int in_seed)
{
  pthread_mutex_lock(&m_mutex);
  cRandom::ResetSeed(in_seed);
  pthread_mutex_unlock(&m_mutex);
}


void cRandom::init()
{
  int mj, mk, ii, i;

  // Clear variables
  inext = 0;
  inextp = 0;
  expRV = 0;
  for( i=0; i<56; ++i ){
    ma[i] = 0;
  }

  mj = _RAND_MSEED - seed;
  mj %= _RAND_MBIG;
  ma[55] = mj;
  mk = 1;

  for (i = 1; i < 55; ++i) {
    ii = (21 * i) % 55;
    ma[ii] = mk;
    mk = mj - mk;
    if (mk < 0) mk += _RAND_MBIG;
    mj = ma[ii];
  }

  for (int k = 0; k < 4; ++k) {
    for (int j = 1; j < 55; ++j) {
      ma[j] -= ma[1 + (j + 30) % 55];
      if (ma[j] < 0) ma[j] += _RAND_MBIG;
    }
  }

  inext = 0;
  inextp = 31;

  // Setup variables used by Statistical Distribution functions
  expRV = -log(cRandom::Get() * _RAND_FAC);
}

unsigned int cRandom::Get()
{
  if (++inext == 56) inext = 0;
  if (++inextp == 56) inextp = 0;
  assert(inext < 56);
  assert(inextp < 56);
  int mj = ma[inext] - ma[inextp];
  if (mj < 0) mj += _RAND_MBIG;
  ma[inext] = mj;
  return mj;
}

unsigned int cRandomMT::Get()
{
  pthread_mutex_lock(&m_mutex);
  unsigned int value = cRandom::Get();
  pthread_mutex_unlock(&m_mutex);
  return value;
}

// Statistical functions //////////////////////////////////////////////////////

double cRandom::GetRandNormal()
{
  // Draw from a Unit Normal Dist
  // Using Rejection Method and saving of initial exponential random variable
  double expRV2;
  while (1) {
    expRV2 = -log(GetDouble());
    expRV -= (expRV2-1)*(expRV2-1)/2;
    if (expRV > 0) break;  
    expRV = -log(GetDouble());
  }
  if (P(.5)) 
    return expRV2;
  else
    return -expRV2;
}

double cRandomMT::GetRandNormal()
{
  // Draw from a Unit Normal Dist
  // Using Rejection Method and saving of initial exponential random variable
  double expRV2;
  
  pthread_mutex_lock(&m_mutex);
  while (1) {
    expRV2 = -log(cRandom::Get() * _RAND_FAC);
    expRV -= (expRV2-1)*(expRV2-1)/2;
    if (expRV > 0) break;
    expRV = -log(cRandom::Get() * _RAND_FAC);
  }
  pthread_mutex_unlock(&m_mutex);
  
  if (P(.5)) 
    return expRV2;
  else
    return -expRV2;
}

unsigned int cRandom::GetRandPoisson(const double mean)
{
  // Draw from a Poisson Dist with mean
  // if cannot calculate, returns UINT_MAX
  // Uses Rejection Method
  unsigned int k = 0;
  double a = exp(-mean);
  double u = GetDouble();
  if (a <= 0) return UINT_MAX; // cannot calculate, so return UINT_MAX
  while (u >= a) {
    u *= GetDouble();
    ++k;
  }
  return k;
}

unsigned int cRandom::GetFullRandBinomial(const double n, const double p)
{
  // Actually try n Bernoulli events with probability p
  unsigned int k = 0;
  for (unsigned int i = 0; i < n; ++i) if (P(p)) k++;
  return k;
}

unsigned int cRandom::GetRandBinomial(const double n, const double p)
{
  // Approximate Binomial if appropriate
  // if np(1-p) is large, use a Normal approx
  if (n * p * (1 - p) >= _BINOMIAL_TO_NORMAL) {
    return static_cast<unsigned int>(GetRandNormal(n * p, n * p * (1 - p)) + 0.5);
  }
  // elseif n is large, use a Poisson approx
  if (n >= _BINOMIAL_TO_POISSON) {
    unsigned int k = GetRandPoisson(n, p);
    if (k < UINT_MAX) // if approx worked
      return k;
  }
  // otherwise, actually generate the randBinomial
  return GetFullRandBinomial(n, p);
}


bool cRandom::Choose(int num_in, tArray<int>& out_array)
{
  // If you ask for more than you pass in...
  assert(num_in >= out_array.GetSize());

  if (num_in == out_array.GetSize()) {
    // init array to 0's
    for (int i = 0; i < out_array.GetSize(); i++)  out_array[i] = i;
    return true;
  }

  int choice_num = 0;
  // @CAO this could be done a lot faster when choose size is close to
  // full size.  However we need to rememebr to watch out for larger num_in
  while (choice_num < out_array.GetSize()) {
    int next = static_cast<int>(GetUInt(num_in));

    // See if this choice is ok to use...
    bool ok = true;
    for (int i = 0; i < choice_num; i++) {
      if (out_array[i] == next) {
        ok = false;
        break;
      }
    }

    // And if its good, record it.
    if (ok == true) {
      out_array[choice_num] = next;
      choice_num++;
    }
  }

  return true;
}



#ifdef ENABLE_UNIT_TESTS

/*
Unit tests
*/
#include "cXMLArchive.h"

#include "lightweight_test.h"

#include <cstdio>    // for std::remove() to remove temporary files.
#include <iomanip>
#include <iostream>
#include <fstream> 
#include <string>

namespace nRandom {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cRandom_Archive", s);
  }
  
  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cRandom_Archive", s);
  }
  

  namespace utRandom_hello_world {
    void test(){
      std::cout << CURRENT_FUNCTION << std::endl;
      TEST(true);
      TEST(false);
    }
  }
  
  namespace utRandom_archiving {
    void test() {
#   ifdef ENABLE_SERIALIZATION
      std::cout << CURRENT_FUNCTION << std::endl;
      { 
        std::string filename("./cRandom_basic_serialization.xml");
  
        // Using seed drawn from date and time.
        // No, using a problematic seed found.
        cRandom r(13396544);
  
        /*
        Exercise the random number generator a little bit before saving
        and restoring.
        */
        int seed = r.GetSeed();
        int original_seed = r.GetOriginalSeed();
        double d = r.GetDouble();
        double dm = r.GetDouble(1.0);
        double dr = r.GetDouble(1.0, 2.0);
        int i = r.GetInt(100);
        int ir = r.GetInt(100, 200);
        bool p = r.P(0.5);
        bool up = r.uP(0.0000005);
        double rn = r.GetRandNormal();
        double rnr = r.GetRandNormal(10.0, 2.0);
        unsigned int rpnp = r.GetRandPoisson(10.0, 0.5);
        unsigned int rpm = r.GetRandPoisson(0.5);
        unsigned int frb = r.GetFullRandBinomial(10.0, 3.0);
        unsigned int rb = r.GetRandBinomial(10.0, 3.0);
  
        /* Save random number generator state.  */
        save_stuff<>(r, filename.c_str());
  
        /* Get some random numbers...  */
        seed = r.GetSeed();
        original_seed = r.GetOriginalSeed();
        d = r.GetDouble();
        dm = r.GetDouble(1.0);
        dr = r.GetDouble(1.0, 2.0);
        i = r.GetInt(100);
        ir = r.GetInt(100, 200);
        p = r.P(0.5);
        up = r.uP(0.0000005);
        tArray<int> choose(10);
        r.Choose(100, choose);
        rn = r.GetRandNormal();
        rnr = r.GetRandNormal(10.0, 2.0);
        rpnp = r.GetRandPoisson(10.0, 0.5);
        rpm = r.GetRandPoisson(0.5);
        frb = r.GetFullRandBinomial(10.0, 3.0);
        rb = r.GetRandBinomial(10.0, 3.0);
  
        /* Reload saved random number generator state.  */
        cRandom r2(0);
        restore_stuff<>(r2, filename.c_str());
  
        /*
        Get some random numbers... Should be the same as those read above.
        */
        int seed2 = r2.GetSeed();
        int original_seed2 = r2.GetOriginalSeed();
        double d2 = r2.GetDouble();
        double dm2 = r2.GetDouble(1.0);
        double dr2 = r2.GetDouble(1.0, 2.0);
        int i2 = r2.GetInt(100);
        int ir2 = r2.GetInt(100, 200);
        bool p2 = r2.P(0.5);
        bool up2 = r2.uP(0.0000005);
        tArray<int> choose2(10);
        r2.Choose(100, choose2);
        double rn2 = r2.GetRandNormal();
        double rnr2 = r2.GetRandNormal(10.0, 2.0);
        unsigned int rpnp2 = r2.GetRandPoisson(10.0, 0.5);
        unsigned int rpm2 = r2.GetRandPoisson(0.5);
        unsigned int frb2 = r2.GetFullRandBinomial(10.0, 3.0);
        unsigned int rb2 = r2.GetRandBinomial(10.0, 3.0);
  
        /* Compare results.  */
        TEST(seed2 == seed);
        TEST(original_seed2 == original_seed);
        TEST(d2 == d);
        TEST(dm2 == dm);
        TEST(dr2 == dr);
        TEST(i2 == i);
        TEST(ir2 == ir);
        TEST(p2 == p);
        TEST(up2 == up);
        for(int i = 0; i < choose.GetSize(); i++){
          TEST(choose2[i] == choose[i]);
        }
        TEST(rn2 == rn);
        TEST(rnr2 == rnr);
        TEST(rpnp2 == rpnp);
        TEST(rpm2 == rpm);
        //std::cout<<"rnr2: "<<rnr2<<", rnr: "<<rnr<<std::endl;
        //std::cout<<"rpnp2: "<<rpnp2<<", rpnp: "<<rpnp<<std::endl;
        //std::cout<<"rpm2: "<<rpm2<<", rpm: "<<rpm<<std::endl;
        TEST(frb2 == frb);
        TEST(rb2 == rb);
  
        /*
        Print random number seeds to stdout, in case we run across a seed
        that breaks things in weird ways.
        */
        std::cout << "utRandom_archiving info: seed " << seed << ", seed2 " << seed2
        << ", original_seed " << original_seed << ", original_seed2 "
        << original_seed2 << std::endl;
  
        std::remove(filename.c_str());
      }
#   endif // ENABLE_SERIALIZATION
    }
  }

  void UnitTests(bool full)
  {
    //if(full) utRandom_hello_world::test();
    if(full) utRandom_archiving::test();
  }
} // nRandom

#endif // ENABLE_UNIT_TESTS
