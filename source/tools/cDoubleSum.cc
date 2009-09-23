/*
 *  cDoubleSum.cc
 *  Avida
 *
 *  Called "double_sum.cc" prior to 12/7/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cDoubleSum.h"


const double cDoubleSum::INF_ERR = 0;

double cDoubleSum::Skewness() const
{
  return (n > 2.0) ? (n * s3 - (3.0 * s2) * s1 + ((2.0 * s1) * s1) * s1 / n) / ((n - 1.0) * (n - 2.0)) : INF_ERR;
}
//n*n*(s3/n - 3*s2/n*s1/n + 2*s1/n*s1/n*s1/n)/((n-1)*(n-2)) : INF_ERR; }


double cDoubleSum::Kurtosis() const {
  return (n > 3.0) ? 
  (n + 1.0) * (n * s4 - (4.0 * s3) * s1 + (((6.0 * s2) * s1) * s1) / n - (((((3.0 * s1) * s1) * s1) / n) * s1) / n) /
//(n + 1.0) * (n * s4 - (4.0 * s3) * s1 + (((6.0 * s2) * s1) * s1) / n - ((((3.0 * s1) * s1) * s1) / (n * s1)) / n) /
  ((n - 1.0) * (n - 2.0) * (n - 3.0))
//((n - 1.0) * (n - 2.0) * (n - 3.0))
  : INF_ERR;
}
//n*n*(n+1)*(s4/n - 4*s3/n*s1/n + 6*s2/n*s1/n*s1/n -
//3*s1/n*s1/n*s1/n*s1/n)/((n-1)*(n-2)*(n-3)) :
