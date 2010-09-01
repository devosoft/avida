/*
 *  cHistogram.h
 *  Avida
 *
 *  Called "histogram.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cHistogram_h
#define cHistogram_h

#include <cmath>
#include <iostream>


class cHistogram {
private:
  int * bins;
  int min_bin;
  int max_bin;
  int entry_count;
  int entry_total;
public:
  cHistogram(int in_max=1, int in_min=0);
  inline ~cHistogram() { delete [] bins; }

  void Resize(int new_max, int new_min=0);
  void Print();
  inline void Clear();
  inline void Insert(int value, int count=1);
  inline void Remove(int value);
  inline void RemoveBin(int value);

  inline double GetAverage()
    { return ((double)entry_total) / (double)entry_count; }
  inline double GetCountAverage()
    { return (double) entry_count / (double) GetNumBins(); }
  inline int GetMode();
  inline double GetVariance();
  inline double GetCountVariance();
  inline double GetStdDev();
  inline double GetCountStdDev();
  inline double GetEntropy();
  inline double GetNormEntropy();

  inline int GetCount() { return entry_count; }
  inline int GetCount(int value) { return bins[value - min_bin]; }
  inline int GetTotal() { return entry_total; }
  inline int GetMinBin() { return min_bin; }
  inline int GetMaxBin() { return max_bin; }
  inline int GetNumBins() { return max_bin - min_bin + 1; }
};


inline void cHistogram::Clear()
{
  int num_bins = GetNumBins();
  for (int i = 0; i < num_bins; i++) {
    bins[i] = 0;
  }
  entry_count = 0;
  entry_total = 0;
}


inline void cHistogram::Insert(int value, int count)
{
  if (value > max_bin || value < min_bin) {
    std::cerr << "Trying to insert " << value << " into Histogram of range ["
	 << min_bin << "," << max_bin << "]" << std::endl;
  }

  bins[value - min_bin] += count;
  entry_count += count;
  entry_total += value * count;
}

inline void cHistogram::Remove(int value)
{
#ifdef DEBUG
  if (value > max_bin || value < min_bin) {
    std::cerr << "Trying to remove " << value << " into Histogram of range ["
	 << min_bin << "," << max_bin << "]" << std::endl;
    return;
  }
  if (bins[value] <= 1) {
    std::cerr << "Warning: Trying to remove " << value << " from bin with "
	 << bins[value] << " entries" << std::endl;
    return;
  }
#endif

  bins[value - min_bin]--;
  entry_count--;
  entry_total -= value;
}

inline void cHistogram::RemoveBin(int value)
{
#ifdef DEBUG
  if (value > max_bin || value < min_bin) {
    std::cerr << "Warning: Trying to remove " << value
	 << " in histogram of range [" << min_bin << "," <<  max_bin
	 << "]" << std::endl;
    return;
  }
#endif

  int old_size = bins[value - min_bin];
  bins[value - min_bin] = 0;

  entry_count -= old_size;
  entry_total -= value * old_size;
}

inline int cHistogram::GetMode()
{
  int num_bins = GetNumBins();
  int mode = 0;
  for (int i = 1; i < num_bins; i++) {
    if (bins[i] > bins[mode]) mode = i;
  }

  return mode + min_bin;
}

inline double cHistogram::GetVariance()
{
  if (entry_count < 2) return 0;

  double mean = GetAverage();
  double var = 0;
  double value = 0;

  int num_bins = GetNumBins();
  for (int i = 0; i < num_bins; i++) {
    value = (double) (i + min_bin) - mean;
    var += bins[i] * value * value;
  }
  var /= entry_count - 1;

  return var;
}

inline double cHistogram::GetCountVariance()
{
  int num_bins = GetNumBins();
  if (num_bins < 2) return 0;

  double mean = GetCountAverage();
  double var = 0;
  double value = 0;

  for (int i = 0; i < num_bins; i++) {
    value = (double) bins[i] - mean;
    var += value * value;
  }
  var /= num_bins - 1;

  return var;
}

inline double cHistogram::GetStdDev()
{
  return sqrt(GetVariance());
}

inline double cHistogram::GetCountStdDev()
{
  return sqrt(GetCountVariance());
}

inline double cHistogram::GetEntropy()
{
  int num_bins = GetNumBins();

  double entropy = 0.0;
  double prob = 0.0;

  for (int i = 0; i < num_bins; i++) {
    prob = (double) bins[i] / (double) entry_count;
    entropy -= prob * log(prob);
  }

  return entropy;
}

inline double cHistogram::GetNormEntropy()
{
  int num_bins = GetNumBins();

  double entropy = 0.0;
  double prob = 0.0;

  for (int i = 0; i < num_bins; i++) {
    prob = ((double) bins[i]) / (double) entry_count;
    if (prob != 0.0) entropy -= prob * log(prob);
  }

  return entropy / log((double) num_bins);
}

#endif
