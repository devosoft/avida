/*
 *  cDynamicCount.cc
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
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

#include "cDynamicCount.h"
#include <iostream>

cDynamicCount::cDynamicCount()
{
 cSpatialResCount(); 
}

cDynamicCount::cDynamicCount(int peaks, double in_min_height, double in_radius_range, double in_min_radius, double in_ah, double in_ar,
			    double in_acx, double in_acy, double in_hstepscale, double in_rstepscale, double in_cstepscalex, double in_cstepscaley, double in_hstep, double in_rstep,
			    double in_cstepx, double in_cstepy, int in_worldx, int in_worldy, int in_geometry, int in_updatestep) : H(NULL), R(NULL), x(NULL), 
			    IUPH(NULL), IUPR(NULL), IUPC(NULL), NoPeaks(peaks), NoDim(2), Hbase(in_min_height), Hrange(in_radius_range), Hmax(0), Hminpct(0),
			    Rbase(in_min_radius), Rrange(in_radius_range), Rmax(0), Rminpct(0), Ah(in_ah), Ar(in_ar), Hstepscale(in_hstepscale), Rstepscale(in_rstepscale), 
			    Hstep(in_hstep), Rstep(in_rstep)
{
  ResizeClear(in_worldx, in_worldy, in_geometry);
  SetXdiffuse(0);
  SetXgravity(0);
  SetYdiffuse(0);
  SetYgravity(0);
  
  Rbase = Rbase*((double(in_worldx)+(double(in_worldy))))/2.0;
  Rrange = Rrange*((double(in_worldx)+(double(in_worldy))))/2.0;


  m_updatestep = in_updatestep;
  m_counter = in_updatestep;
  
  R = new double[NoPeaks];
  H = new double[NoPeaks];
  x = new double*[NoPeaks];
  
  Ac[0] = in_acx;
  Ac[1] = in_acy;
  
  cstepscale[0] = in_cstepscalex;
  cstepscale[1] = in_cstepscaley;
  
  cstep[0] = in_cstepx; 
  cstep[1] = in_cstepy;
  
  for(int i = 0; i < NoPeaks; i++){
    x[i] = new double[2];
  }
  
  IUPH = new int[NoPeaks];
  IUPR = new int[NoPeaks];
  
  IUPC = new int*[NoPeaks];
  
  ChangedPeaks = new int[NoPeaks];
  
  for(int i = 0; i < NoPeaks; i++){
   ChangedPeaks[i] = i;
  }
  
  for(int i = 0; i < NoPeaks; i++){
    IUPC[i] = new int[2];
  }
  
  raw_grid = new double*[GetX()];
  
    
  for(int i = 0; i < GetX(); i++){
    raw_grid[i] = new double[GetY()];
  }
  
  df1_init();
  UpdateCount();
}

void cDynamicCount::FlowAll()
{
  cSpatialResCount::FlowAll();
}

void cDynamicCount::StateAll()
{
  cSpatialResCount::StateAll();
}

cDynamicCount::~cDynamicCount()
{
  
  delete[] R;
  delete[] H;
  delete[] IUPH;
  delete[] IUPR;
  delete ChangedPeaks;
    
    for(int i = 0; i < NoPeaks; i++){
    delete[] x[i];
  }
  
  delete[] x;
  
    for(int i = 0; i < NoPeaks; i++){
    delete[] IUPC[i];
  }
  
  delete[] IUPC;
      
  for(int i = 0; i < GetX(); i++){
    delete raw_grid[i];
  }
  
  delete [] raw_grid;
}


void cDynamicCount::UpdateCount() 
{
  m_counter++;
  if(m_counter < m_updatestep) return;
  double coordinates[2];
  int Dimension1 = 0;
  int Dimension2 = 1;
  
  df1_chg_H(NoPeaks, ChangedPeaks);
  df1_chg_R(NoPeaks, ChangedPeaks);
  df1_chg_c(Dimension1, NoPeaks, ChangedPeaks);
  df1_chg_c(Dimension2, NoPeaks, ChangedPeaks);
  
  
  for (double ix = double(1.0)/double(GetX()); ix < 1.0; ix=ix+double(1.)/double(GetX())){
    for (double iy = double(1.0)/double(GetY()); iy < 1.0; iy=iy+double(1.0)/double(GetY())){
      coordinates[0]=double(ix);
      coordinates[1]=double(iy);
      int x = int(.5+(ix*GetX()));
      int y = int(.5+(iy*GetY()));
      raw_grid[x][y] = df1(coordinates);
    }
  }

  for(int x = 0; x < GetX(); x++){
    for(int y = 0; y < GetY(); y++){
      if(raw_grid[x][y] < 0) (raw_grid[x][y]) = 0;
      if(x == 0 || y == 0)  (raw_grid[x][y]) = -0.05;
      if(x == (GetX()-1) || y == (GetY()-1))  (raw_grid[x][y]) = -0.05;
      Element(y*GetX()+x).SetInitial((raw_grid[x][y]));
      Element(y*GetX()+x).SetAmount((raw_grid[x][y]));
    }
  }
  ResetResourceCounts();
  m_counter = 0;
}

  


/*JW Below is code adapted from DeJong and 
Morrison's Dynamic Function Generator
*/

 /************************************************************
   This is the source file for the df1 Dynamic Test Problem
   Generator.
   
   This file contains the source for the functions:
   
     def1_init:  The initialization routine.
     df1:  This evaluates the function value, given a 
           set of coordinates.
     df1_chg_H:  This applies dynamics to the peak heights.
     df1_chg_R:  This applies dynamics to the peak slopes.
     df1_chg_C:  This moves the peaks along the n dimensional
                  coordinates.
     df1_flip:  Randomly returns a 1 or a 0.

  This file uses the initialization values that are set in df1.h,
  which define the dynamic behavior of test function.

  Details regarding this test function generator can be found
  in "A Test Problem Generator for Non-Stationary Environments"
  by Ronald Morrison and Kenneth De Jong in the Proceeding of
  the Congress on Evolutionary Computation (CEC-99).       

   
   To Use:

   (1)  Set up the dynamic behavior you want by setting
        values for the variables in df1.h.  Remember to
        re-compile the programs using the new df1.h info.
   (2)  Call df1_init to initialize the environment.
   (3)  Call df1 to get the fitness value of any set of
        coordinates.
   (4)  Apply dynamics when desired:
        Call df1_chg_R  to modify R values (peak slopes)
        Call df1_chg_H  to modify H values (peak heights)             
        Call df1_chg_c  to modifies coordinate values (locations)
     All functions take an array which specifies which peaks 
     to modify.  The peaks are numbered from 0 to NoPeaks -1.
     No tests are done to check if the values in array Peaks 
     are valid. 
   
  Permission is hereby granted to copy all or any part of  
  this program for free distribution.               
                                                           
 File: df1.c     version 1.3        September 4, 1999 
 
 Direct comments and suggestions to 
 Ron Morrison at ronald.morrison@mitretek.org
*************************************************************/

//#include "df1.h" 
#include <stdlib.h>
#include <time.h>
#include <cmath>
#define max(a, b) (a > b) ? a : b
/*
int df1_flip (void);                    prototype for df1_flip 
double df1 (double coord[2]);  prototype for df1 
void  df1_init ();        prototype for df1_init 
void df1_chg_R (int NoToChg, int Peaks[NUMBER_PEAKS]);
                                     prototype for df1_chg_R 
void df1_chg_H (int NoToChg, int Peaks[NUMBER_PEAKS]);
                                 prototype for df1_chg_H 
void df1_chg_c (int CoordNo, int NoToChg, int Peaks[NUMBER_PEAKS]);
                                 prototype for df1_chg_c 
*/

/**********************************************************
* df1 (coord) evlauates fitness function at the location 
  specified by the n-dimensional vector coord        
************************************************************/

double cDynamicCount::df1(double coord[2])
{
  double z, zi, sum;
  int i, j;

  sum = 0.0;
  for (i = 0; i < NoDim; i++)
    {
      sum += (x[0][i] - coord[i]) * (x[0][i] - coord[i]);
    }
  z = H[0] - R[0] * sqrt (sum);

  if (NoPeaks > 1)
    {
      for (j = 1; j < NoPeaks; j++)
      {
        sum = 0.0;
        for (i = 0; i < NoDim; i++)
        {
            sum += (x[j][i] - coord[i]) * (x[j][i] - coord[i]);
        }
        zi = H[j] - R[j] * sqrt (sum);
        z = max (z, zi);
      }
    }
  return z;
}

/*    end of function df1(coord[2])         */
/**************************************************/

/* Start of function df1_init 
  This initializes the variables for df1          

*****************************************************/

void cDynamicCount::df1_init ()


/*  This uses the values set in df1.h for NoPeaks,
    NoDim, Ah, Ar, and Ac[2]
*/

{
  int i, j;
  srand (time (0));     /* initalize random number generator */
 
  

/* Work past the inital transients in the step values  */
/* This changes the Hstep value from the initialized value
    to the value(s) driven by the Ah value.            */

  for (j = 0; j < 100; j++)

    Hstep = Ah * Hstep * (1.0 - Hstep);

/* This changes the Rstep value from the initialized value
    to the value(s) driven by the Ar value.            */

  for (j = 0; j < 100; j++)
    Rstep = Ar * Rstep * (1.0 - Rstep);

/* This changes the cstep[] value from the initialized value
    to the value(s) driven by the Ac[] values.            */

  for (i = 0; i < NoDim; i++)
    {
      for (j = 0; j < 100; j++)
    cstep[i] = Ac[i] * cstep[i] * (1.0 - cstep[i]);
    }

/*  This section randomizes the cone values. 
     To set specific cone intialtion values, 
        do so after these loops */

  for (i = 0; i < NoPeaks; i++)
    {
      R[i] = Rbase + Rrange * (float) rand () / (float) RAND_MAX;
      H[i] = Hbase + Hrange * (float) rand () / (float) RAND_MAX;
      for (j = 0; j < NoDim; j++)
    x[i][j] = 2 * (float) rand () / (float) RAND_MAX - 1.0;
    }

/* Include following lines setting R[0] = H[0] = 0.0   */
/* if a non-negative landscape is wanted.              */
/*
  R[0] = 0.0;
  H[0] = 0.0;
*/

/*If you have any specific cones that you want in the landscape, 
  put specific values of R[i], H[i] and x[i][j] here to create 
  the desired landscape.                                   */



/* compute other initalization values  */
  Rmax = Rbase + Rrange;
  Rminpct = Rbase / Rmax;
  Hmax = Hbase + Hrange;
  Hminpct = Hbase / Hmax;

/* This section randomly sets whether the dyanmic varaible
    start to move by increasing or decreasing.
   This is done through a set of up/down flags.               
   These are randomly set.  The user can reset any of them to 
    any desired specific values after the loop.   */

  for (i = 0; i < NoPeaks; i++)
    {
      IUPH[i] = df1_flip ();
      IUPR[i] = df1_flip ();

      for (j = 0; j < NoDim; j++)
      {
        IUPC[i][j] = df1_flip ();
      }
    }

/* put specific values of IUPH[i], IUPR[i], and IUPC[i][j] here to  */
/* to set specific inital values for the up/down flags              */

}
/* end of function df1_init
**************************************************************/

/* This is the start of the routines to modify the landscape.
    df1_chg_R   modifies R values              
    df1_chg_H   modifies H values              
    df1_chg_c   modifies coordinate values     
    All functions take an array wich specifies 
    which peaks to modify.                      
    Peaks are numbered from 0 to NoPeaks -1.    
   No tests are done to see if the values in  
   array Peaks are valid.                      
*************************************************************/

//void df1_chg_R (int NoToChg, int Peaks[NUMBER_PEAKS])
void cDynamicCount::df1_chg_R(int NoToChg, int* Peaks)
/* NoToChg is the number of peaks to change the R 
   value of. Peaks is an array of peak numbers to 
   change.                                        */

{
  int j;
  double Rpct, Rtemp;
  if (NoToChg > 0)
    {
      for (j = 0; j < NoToChg ; j++)
	  {
	    Rpct = R[Peaks[j]] / Rmax;
	    Rstep = Ar * Rstep * ((double) (1.0) - Rstep);
	    Rtemp = Rstep * Rstepscale;
	    if (IUPR[Peaks[j]] == 1)
	    {
	      Rpct = Rpct + Rtemp;
	      if (Rpct > 1.0)
		  {
		  IUPR[Peaks[j]] = 0;
		  Rpct = 1.0 - (Rpct - 1.0);
		  }
	    }
	    else
	    {
	      Rpct = Rpct - Rtemp;
	      if (Rpct < Rminpct)
		  {
		    IUPR[Peaks[j]] = 1;
			if (Rpct >= 0) {
				Rpct = Rminpct + (Rminpct - Rpct);
			}
			else{
				Rpct = Rminpct - Rpct;
			}
		  }
	    }
	    R[Peaks[j]] = Rpct * Rmax;
	  }
    }
}
/*  end of function df1_chg_R                    */

/**************************************************
This section changes the peak heights
**************************************************/

//void df1_chg_H (int NoToChg, int Peaks[NUMBER_PEAKS])
void cDynamicCount::df1_chg_H (int NoToChg, int* Peaks)
/* changes peaks H value   (height)               */
/* same arguments as df1_chg-R                    */

{
  int j;
  double Hpct, Htemp;

  if (NoToChg > 0)
    {
      for (j = 0; j < NoToChg; j++)
	{
	  Hpct = H[Peaks[j]] / Hmax;
	  Hstep = Ah * Hstep * ((double) 1.0 - Hstep);
	  Htemp = Hstep * Hstepscale;

	  if (IUPH[Peaks[j]] == 1)
	  {
	      Hpct = Hpct + Htemp;
	      if (Hpct > 1.0)
		  {
		    IUPH[Peaks[j]] = 0;
		    Hpct = 1.0 - (Hpct - 1.0);
		  }
	  }
	  else
	  {
	      Hpct = Hpct - Htemp;
	      if (Hpct < Hminpct)
		  {
		    IUPH[Peaks[j]] = 1;
			if (Hpct >= 0) {
			   Hpct = Hminpct + (Hminpct - Hpct);
			}
			else {
				Hpct = Hminpct - Hpct;
			}
		  }
	    }

	  H[Peaks[j]] = Hpct * Hmax;

	}
    }
}
/*  end of function df1_chg_H                         
************************************************************8*/
//void df1_chg_c (int CoordNo, int NoToChg, int Peaks[NUMBER_PEAKS])
void cDynamicCount::df1_chg_c(int CoordNo, int NoToChg, int* Peaks)
/* This change the peak location.                           
   CoordNo is the number of which dimension to change   
   CoordNo ranges from 0 to DimNo-1                     */
{
  int j;
  double xpct, xtemp;
  
  if (NoToChg > 0)
    {
      for (j = 0; j< NoToChg; j++)
	  {
	  xpct = (x[Peaks[j]][CoordNo] + 1.0 ) / 2.0;
	  cstep[CoordNo] = Ac[CoordNo] * cstep[CoordNo] * ((double) 1.0 - cstep[CoordNo]);
	  xtemp = cstep[CoordNo] * cstepscale[CoordNo];
	if (IUPC[Peaks[j]][CoordNo] == 1)
	    {
	      xpct = xpct + xtemp;
	      if (xpct > 1.0)
		  {
		  IUPC[Peaks[j]][CoordNo] = 0;
		  xpct = 1.0 - (xpct - 1.0);
		  }
	    }
	  else
	    {
	      xpct = xpct - xtemp;
	      if (xpct < 0.0)
		  {
		  IUPC[Peaks[j]][CoordNo] = 1;
		  xpct = -xpct;
		  }
	    }
	  x[Peaks[j]][CoordNo] = (xpct * 2.0) - 1.0;
	   }
    }
}
/* end of function df1_chg_c                          
**********************************************************/
/* function df1_flip()                                    */
/* function returns a 1 or 0 depending upon random()   */

int cDynamicCount::df1_flip()
{
  int i;
  i = 0;
  if (rand () > RAND_MAX / 2)
    i = 1;
  return i;
}
/* end of funtion df1_flip()     */
