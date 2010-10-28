/*
 *  cColor.cc
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "cColor.h"

const cColor cColor::BLACK   (0.0, 0.0, 0.0);
const cColor cColor::WHITE   (1.0, 1.0, 1.0);
const cColor cColor::RED     (1.0, 0.0, 0.0);
const cColor cColor::GREEN   (0.0, 1.0, 0.0);
const cColor cColor::BLUE    (0.0, 0.0, 1.0);
const cColor cColor::YELLOW  (1.0, 1.0, 0.0);
const cColor cColor::CYAN    (0.0, 1.0, 1.0);
const cColor cColor::MAGENTA (1.0, 0.0, 1.0);

const cColor cColor::GRAY       (0.7, 0.7, 0.7);
const cColor cColor::LT_RED     (1.0, 0.3, 0.3);
const cColor cColor::LT_GREEN   (0.3, 1.0, 0.3);
const cColor cColor::LT_BLUE    (0.3, 0.3, 1.0);
const cColor cColor::LT_YELLOW  (1.0, 1.0, 0.3);
const cColor cColor::LT_CYAN    (0.3, 1.0, 1.0);
const cColor cColor::LT_MAGENTA (1.0, 0.3, 1.0);

const cColor cColor::DARK_GRAY    (0.3, 0.3, 0.3);
const cColor cColor::DARK_RED     (0.5, 0.0, 0.0);
const cColor cColor::DARK_GREEN   (0.0, 0.5, 0.0);
const cColor cColor::DARK_BLUE    (0.0, 0.0, 0.5);
const cColor cColor::DARK_YELLOW  (0.3, 0.3, 0.0);
const cColor cColor::DARK_CYAN    (0.0, 0.3, 0.3);
const cColor cColor::DARK_MAGENTA (0.3, 0.0, 0.3);

tArray<cColor> cColor::SCALE_FULL(0);
tArray<cColor> cColor::SCALE_BRIGHT(0);
tArray<cColor> cColor::SCALE_DARK(0);
tArray<cColor> cColor::SCALE_PASTEL(0);
tArray<cColor> cColor::SCALE_HOT(0);
tArray<cColor> cColor::SCALE_COLD(0);

void cColor::Setup()
{
  if (SCALE_BRIGHT.GetSize() != 0) return;

//  int index = 0;

  SCALE_BRIGHT.Push(cColor(0, 0, 0));
  // SCALE_BRIGHT.Push(cColor(0.2 , 0.2 , 0.2 ));

  SCALE_BRIGHT.Push(cColor(1.0 , 1.0 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.95, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.9 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.85, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.8 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.75, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.7 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.65, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.6 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.55, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.5 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.45, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.4 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.35, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.3 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.25, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.2 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.15, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.1 , 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.05, 0.0 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.0 , 0.0 ));

//   SCALE_BRIGHT.Push(cColor(1.0 , 0.0 , 0.05));
//   SCALE_BRIGHT.Push(cColor(0.95, 0.0 , 0.1 ));
//   SCALE_BRIGHT.Push(cColor(0.9 , 0.0 , 0.15));
//   SCALE_BRIGHT.Push(cColor(0.85, 0.0 , 0.2 ));
//   SCALE_BRIGHT.Push(cColor(0.8 , 0.0 , 0.25));
//   SCALE_BRIGHT.Push(cColor(0.75, 0.0 , 0.3 ));
//   SCALE_BRIGHT.Push(cColor(0.7 , 0.0 , 0.35));
//   SCALE_BRIGHT.Push(cColor(0.65, 0.0 , 0.4 ));
//   SCALE_BRIGHT.Push(cColor(0.6 , 0.0 , 0.45));
//   SCALE_BRIGHT.Push(cColor(0.55, 0.0 , 0.5 ));
//   SCALE_BRIGHT.Push(cColor(0.5 , 0.0 , 0.55));
//   SCALE_BRIGHT.Push(cColor(0.45, 0.0 , 0.6 ));
//   SCALE_BRIGHT.Push(cColor(0.4 , 0.0 , 0.65));
//   SCALE_BRIGHT.Push(cColor(0.35, 0.0 , 0.7 ));
//   SCALE_BRIGHT.Push(cColor(0.3 , 0.0 , 0.75));
//   SCALE_BRIGHT.Push(cColor(0.25, 0.0 , 0.8 ));
//   SCALE_BRIGHT.Push(cColor(0.2 , 0.0 , 0.85));
//   SCALE_BRIGHT.Push(cColor(0.15, 0.0 , 0.9 ));
//   SCALE_BRIGHT.Push(cColor(0.1 , 0.0 , 0.95));
//   SCALE_BRIGHT.Push(cColor(0.05, 0.0 , 1.0 ));
//   SCALE_BRIGHT.Push(cColor(0.0 , 0.0 , 1.0 ));

  SCALE_BRIGHT.Push(cColor(1.0 , 0.02 , 0.05));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.04 , 0.1 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 0.06 , 0.15));
  SCALE_BRIGHT.Push(cColor(0.95, 0.08 , 0.2 ));
  SCALE_BRIGHT.Push(cColor(0.9 , 0.10 , 0.25));
  SCALE_BRIGHT.Push(cColor(0.85, 0.12 , 0.3 ));
  SCALE_BRIGHT.Push(cColor(0.8 , 0.14 , 0.35));
  SCALE_BRIGHT.Push(cColor(0.75, 0.16 , 0.4 ));
  SCALE_BRIGHT.Push(cColor(0.7 , 0.18 , 0.45));
  SCALE_BRIGHT.Push(cColor(0.65, 0.20 , 0.5 ));
  SCALE_BRIGHT.Push(cColor(0.6 , 0.20 , 0.55));
  SCALE_BRIGHT.Push(cColor(0.55, 0.20 , 0.6 ));
  SCALE_BRIGHT.Push(cColor(0.5 , 0.20 , 0.65));
  SCALE_BRIGHT.Push(cColor(0.45, 0.20 , 0.7 ));
  SCALE_BRIGHT.Push(cColor(0.4 , 0.20 , 0.75));
  SCALE_BRIGHT.Push(cColor(0.35, 0.20 , 0.8 ));

  SCALE_BRIGHT.Push(cColor(0.3 , 0.25 , 0.85));
  SCALE_BRIGHT.Push(cColor(0.25, 0.25 , 0.9 ));
  SCALE_BRIGHT.Push(cColor(0.2 , 0.20 , 0.95));

  SCALE_BRIGHT.Push(cColor(0.15, 0.15 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.1 , 0.10 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.05, 0.05 , 1.0 ));

//   SCALE_BRIGHT.Push(cColor(0.0 , 0.0 , 1.0 ));
//   SCALE_BRIGHT.Push(cColor(0.0 , 0.0 , 0.0 ));

//   SCALE_BRIGHT.Push(cColor(0.0 , 0.05, 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.1 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.175, 1.0 ));
  //  SCALE_BRIGHT.Push(cColor(0.0 , 0.2 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.25, 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.325 , 1.0 ));
  //  SCALE_BRIGHT.Push(cColor(0.0 , 0.35, 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.4 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.475, 1.0 ));
  //  SCALE_BRIGHT.Push(cColor(0.0 , 0.5 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.55, 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.625 , 1.0 ));
  //  SCALE_BRIGHT.Push(cColor(0.0 , 0.65, 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.7 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.775, 1.0 ));
  // SCALE_BRIGHT.Push(cColor(0.0 , 0.8 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.85, 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.0 , 0.925 , 1.0 ));
  //  SCALE_BRIGHT.Push(cColor(0.0 , 0.95, 1.0 ));

  SCALE_BRIGHT.Push(cColor(0.0 , 1.0 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.05, 1.0 , 1.0 ));
  SCALE_BRIGHT.Push(cColor(0.1 , 1.0 , 0.95));
  SCALE_BRIGHT.Push(cColor(0.15, 1.0 , 0.9 ));
  SCALE_BRIGHT.Push(cColor(0.2 , 1.0 , 0.85));
  SCALE_BRIGHT.Push(cColor(0.25, 1.0 , 0.8 ));
  SCALE_BRIGHT.Push(cColor(0.3 , 1.0 , 0.75));
  SCALE_BRIGHT.Push(cColor(0.35, 1.0 , 0.7 ));
  SCALE_BRIGHT.Push(cColor(0.4 , 1.0 , 0.65));
  SCALE_BRIGHT.Push(cColor(0.45, 1.0 , 0.6 ));
  SCALE_BRIGHT.Push(cColor(0.5 , 1.0 , 0.55));
  SCALE_BRIGHT.Push(cColor(0.55, 1.0 , 0.5 ));
  SCALE_BRIGHT.Push(cColor(0.6 , 1.0 , 0.45));
  SCALE_BRIGHT.Push(cColor(0.65, 1.0 , 0.4 ));
  SCALE_BRIGHT.Push(cColor(0.7 , 1.0 , 0.35));
  SCALE_BRIGHT.Push(cColor(0.75, 1.0 , 0.3 ));
  SCALE_BRIGHT.Push(cColor(0.8 , 1.0 , 0.25));
  SCALE_BRIGHT.Push(cColor(0.85, 1.0 , 0.2 ));
  SCALE_BRIGHT.Push(cColor(0.9 , 1.0 , 0.15));
  SCALE_BRIGHT.Push(cColor(0.95, 1.0 , 0.1 ));
  SCALE_BRIGHT.Push(cColor(1.0 , 1.0 , 0.05));


//   SCALE_BRIGHT.Push(cColor(0.95, 0.0 , 0.0 ));
//   SCALE_BRIGHT.Push(cColor(0.9 , 0.0 , 0.0 ));
//   SCALE_BRIGHT.Push(cColor(0.85, 0.0 , 0.0 ));
//   SCALE_BRIGHT.Push(cColor(0.8 , 0.0 , 0.0 ));
//   SCALE_BRIGHT.Push(cColor(0.75, 0.0 , 0.0 ));
//   SCALE_BRIGHT.Push(cColor(0.7 , 0.0 , 0.0 ));
//   SCALE_BRIGHT.Push(cColor(0.65, 0.0 , 0.0 ));
//   SCALE_BRIGHT.Push(cColor(0.6 , 0.0 , 0.0 ));
//  SCALE_BRIGHT.Push(cColor(0.55, 0.0 , 0.0 ));
//  SCALE_BRIGHT.Push(cColor(0.5 , 0.0 , 0.0 ));

}
