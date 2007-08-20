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

#define HEX_COLOR(RED,GREEN,BLUE) (0x ## RED, 0x ## GREEN, 0x ## BLUE)

const cColor cColor::BLACK   HEX_COLOR(00, 00, 00);
const cColor cColor::WHITE   HEX_COLOR(FF, FF, FF);
const cColor cColor::RED     HEX_COLOR(FF, 00, 00);
const cColor cColor::GREEN   HEX_COLOR(00, FF, 00);
const cColor cColor::BLUE    HEX_COLOR(00, 00, FF);
const cColor cColor::YELLOW  HEX_COLOR(FF, FF, 00);
const cColor cColor::CYAN    HEX_COLOR(00, FF, FF);
const cColor cColor::MAGENTA HEX_COLOR(FF, 00, FF);

const cColor cColor::GRAY       HEX_COLOR(A0, A0, A0);
const cColor cColor::LT_RED     HEX_COLOR(FF, 50, 50);
const cColor cColor::LT_GREEN   HEX_COLOR(50, FF, 50);
const cColor cColor::LT_BLUE    HEX_COLOR(50, 50, FF);
const cColor cColor::LT_YELLOW  HEX_COLOR(FF, FF, 50);
const cColor cColor::LT_CYAN    HEX_COLOR(50, FF, FF);
const cColor cColor::LT_MAGENTA HEX_COLOR(FF, 50, FF);

const cColor cColor::DARK_GRAY    HEX_COLOR(50, 50, 50);
const cColor cColor::DARK_RED     HEX_COLOR(A0, 00, 00);
const cColor cColor::DARK_GREEN   HEX_COLOR(00, A0, 00);
const cColor cColor::DARK_BLUE    HEX_COLOR(00, 00, A0);
const cColor cColor::DARK_YELLOW  HEX_COLOR(A0, A0, 00);
const cColor cColor::DARK_CYAN    HEX_COLOR(00, A0, A0);
const cColor cColor::DARK_MAGENTA HEX_COLOR(A0, 00, A0);

tArray<cColor> cColor::SET_BRIGHT(100);
tArray<cColor> cColor::SET_DARK(100);

void cColor::Setup()
{
  int index = 0;
  //   SET_BRIGHT[index++].Set(0.5 , 0.0 , 0.0 );
  //   SET_BRIGHT[index++].Set(0.55, 0.0 , 0.0 );

  SET_BRIGHT[index++].Set(0.2 , 0.2 , 0.2 );

  SET_BRIGHT[index++].Set(0.6 , 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.65, 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.7 , 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.75, 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.8 , 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.85, 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.9 , 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.95, 0.0 , 0.0 );

  SET_BRIGHT[index++].Set(1.0 , 0.0 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.05, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.1 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.15, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.2 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.25, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.3 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.35, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.4 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.45, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.5 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.55, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.6 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.65, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.7 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.75, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.8 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.85, 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.9 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 0.95, 0.0 );

  SET_BRIGHT[index++].Set(1.0 , 1.0 , 0.0 );
  SET_BRIGHT[index++].Set(1.0 , 1.0 , 0.05);
  SET_BRIGHT[index++].Set(0.95, 1.0 , 0.1 );
  SET_BRIGHT[index++].Set(0.9 , 1.0 , 0.15);
  SET_BRIGHT[index++].Set(0.85, 1.0 , 0.2 );
  SET_BRIGHT[index++].Set(0.8 , 1.0 , 0.25);
  SET_BRIGHT[index++].Set(0.75, 1.0 , 0.3 );
  SET_BRIGHT[index++].Set(0.7 , 1.0 , 0.35);
  SET_BRIGHT[index++].Set(0.65, 1.0 , 0.4 );
  SET_BRIGHT[index++].Set(0.6 , 1.0 , 0.45);
  SET_BRIGHT[index++].Set(0.55, 1.0 , 0.5 );
  SET_BRIGHT[index++].Set(0.5 , 1.0 , 0.55);
  SET_BRIGHT[index++].Set(0.45, 1.0 , 0.6 );
  SET_BRIGHT[index++].Set(0.4 , 1.0 , 0.65);
  SET_BRIGHT[index++].Set(0.35, 1.0 , 0.7 );
  SET_BRIGHT[index++].Set(0.3 , 1.0 , 0.75);
  SET_BRIGHT[index++].Set(0.25, 1.0 , 0.8 );
  SET_BRIGHT[index++].Set(0.2 , 1.0 , 0.85);
  SET_BRIGHT[index++].Set(0.15, 1.0 , 0.9 );
  SET_BRIGHT[index++].Set(0.1 , 1.0 , 0.95);
  SET_BRIGHT[index++].Set(0.05, 1.0 , 1.0 );

  SET_BRIGHT[index++].Set(0.0 , 1.0 , 1.0 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.95);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.9 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.85);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.8 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.75);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.7 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.65);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.6 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.55);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.5 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.45);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.4 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.35);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.3 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.25);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.2 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.15);
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.1 );
  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.05);

  SET_BRIGHT[index++].Set(0.0 , 1.0 , 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.95, 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.9 , 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.85, 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.8 , 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.75, 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.7 , 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.65, 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.6 , 0.0 );
  SET_BRIGHT[index++].Set(0.0 , 0.55, 0.0 );
//  SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );

//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
//   SET_BRIGHT[index++].Set(0.0 , 0.5 , 0.0 );
}
