/*
 *  cDoubleSum.h
 *  Avida
 *
 *  Called "double_sum.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cDoubleSum_h
#define cDoubleSum_h

#include <math.h>
#include <limits.h>

class cDoubleSum {
private:
  double s1;  // Sum (x)
  double s2;  // Sum of squares (x^2)
  double s3;  // Sum of cubes (x^3)
  double s4;  // Sum of x^4
  double n;

public:
  static const double INF_ERR;  // Value Returned by StdError if Infinate

  cDoubleSum() { Clear(); }

  void Clear() { s1 = s2 = s3 = s4 = n = 0; }

  double Count()        const { return n; }
  double N()            const { return n; }
  double Sum()          const { return s1; }
  double S1()           const { return s1; }
  double SumOfSquares() const { return s2; }
  double S2()           const { return s2; }
  double SumOfCubes()   const { return s3; }
  double S3()           const { return s3; }
  double S4()           const { return s4; }

  double Average() const { return ( n >1 ) ? (s1/n) : 0; }

  double Variance() const { return ( n > 1 ) ?
      (s2 - s1*s1/n) / (n-1) : INF_ERR; }
    //n*(s2/n - s1/n*s1/n) / (n-1) : INF_ERR; }
  double StdDeviation() const { return sqrt(Variance()); }
  double StdError()  const { return (n > 1) ?
         sqrt(Variance()/n) : INF_ERR; }

//      old formula, implemented by TCC, not sure how it relates to 
//      the real Standard Error
//       sqrt(s2 / (n * (n-1))) : INF_ERR; }

  double Skewness() const { return ( n > 2 ) ?
       (n*s3 - 3*s2*s1 + 2*s1*s1*s1/n) / ((n-1)*(n-2)) : INF_ERR; }
     //n*n*(s3/n - 3*s2/n*s1/n + 2*s1/n*s1/n*s1/n)/((n-1)*(n-2)) : INF_ERR; }

  double Kurtosis() const { return ( n > 3 ) ?
     //n*n*(n+1)*(s4/n - 4*s3/n*s1/n + 6*s2/n*s1/n*s1/n -
     //3*s1/n*s1/n*s1/n*s1/n)/((n-1)*(n-2)*(n-3)) :
       (n+1)*(n*s4 - 4*s3*s1 + 6*s2*s1*s1/n - 3*s1*s1*s1/n*s1/n) /
       ((n-1)*(n-2)*(n-3)) :
       INF_ERR; }

  // Notation Shortcuts
  double Ave() const { return Average(); }
  double Var() const { return Variance(); }
  double Kur() const { return Kurtosis(); }
  double Skw() const { return Skewness(); }


  void Add(double value, double weight=1){
    double w_val = value * weight;
    n += weight;
    s1 += w_val;
    s2 += w_val * w_val;
    s3 += w_val * w_val * w_val;
    s4 += w_val * w_val * w_val * w_val;
  }

  void Subtract(double value, double weight=1){
    double w_val = value * weight;
    n -= weight;
    s1 -= w_val;
    s2 -= w_val * w_val;
    s3 -= w_val * w_val * w_val;
    s4 -= w_val * w_val * w_val * w_val;
  }
};


#ifdef ENABLE_UNIT_TESTS
namespace nDoubleSum {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
