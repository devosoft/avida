/*
 *  cHistogram.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cHistogram.h"

#include "functions.h"

extern "C" {
#include <stdio.h>
}

#include <iostream>

using namespace std;


cHistogram::cHistogram(int in_max, int in_min)
{
  max_bin = in_max;
  min_bin = in_min;
  entry_count = 0;
  entry_total = 0;

  int num_bins = max_bin - min_bin + 1;
  bins = new int[num_bins];
  for (int i = 0; i < num_bins; i++)   bins[i] = 0;
}

void cHistogram::Resize(int new_max, int new_min)
{
#ifdef DEBUG
  if (new_max < new_min) {
    cerr << "Error: Trying to resize histogram to [" << new_min << "," << new_max << "]" << endl;
    return;
  }
#endif

  // Calculate new num bins.
  int new_num_bins = new_max - new_min + 1;

  // Setup new bins, copying over information...
  int cur_bin = 0;
  int overlap_min = Max(min_bin, new_min);
  int overlap_max = Min(max_bin, new_max);

  int * new_bins = new int[new_num_bins];
  for (cur_bin = new_min; cur_bin < min_bin; cur_bin++)
    new_bins[cur_bin - new_min] = 0;
  for (cur_bin = max_bin; cur_bin <= new_max; cur_bin++)
    new_bins[cur_bin - new_min] = 0;
  for (cur_bin = overlap_min; cur_bin <= overlap_max; cur_bin++)
    new_bins[cur_bin - new_min] = bins[cur_bin - min_bin];

  // Re-count bins...
  int new_count = 0;
  int new_total = 0;
  for (int i = 0; i < new_num_bins; i++) {
    new_count += new_bins[i];
    new_total += new_bins[i] * (i + new_min);
  }
  entry_count = new_count;
  entry_total = new_total;

  delete [] bins;
  bins = new_bins;
  max_bin = new_max;
  min_bin = new_min;
}

void cHistogram::Print()
{
  FILE * fp = fopen("test.dat", "w");
  fprintf(fp, "Min = %d, Max = %d, Count = %d, Total = %d, Ave = %f\n",
	  min_bin, max_bin, entry_count, entry_total, GetAverage());
  for (int i = min_bin; i <= max_bin; i++) {
    fprintf(fp, "%d : %d\n", i, bins[i - min_bin]);
  }
  fflush(fp);
  fclose(fp);
}
