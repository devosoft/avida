/*
 *  nHardware.h
 *  Avida
 *
 *  Created by David on 8/29/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef nHardware_h
#define nHardware_h

namespace nHardware {
  static const double FITNESS_NEUTRAL_RANGE = 0.0;
  static const double FITNESS_NEUTRAL_MIN = 1.0 - FITNESS_NEUTRAL_RANGE;
  static const double FITNESS_NEUTRAL_MAX = 1.0 + FITNESS_NEUTRAL_RANGE;
  
  static const int TEST_CPU_GENERATIONS = 3;  // Default generations tested for viability.
  
  static const int MAX_NOPS = 6;
  static const int MAX_LABEL_SIZE = 10;
  
  enum tHeads { HEAD_IP = 0, HEAD_READ, HEAD_WRITE, HEAD_FLOW, NUM_HEADS };
  
  static const int STACK_SIZE = 10;
}

enum tFaultTypes {
  FAULT_TYPE_WARNING = 0,
  FAULT_TYPE_FORK_TH = 0,
  FAULT_TYPE_KILL_TH = 0,
  FAULT_TYPE_ERROR
};

// Fault locations in CPU executions...
enum tFaultLocations {
  FAULT_LOC_DEFAULT = 0,
  FAULT_LOC_INSTRUCTION,
  FAULT_LOC_JUMP,
  FAULT_LOC_MATH,
  FAULT_LOC_INJECT,
  FAULT_LOC_THREAD_FORK,
  FAULT_LOC_THREAD_KILL,
  FAULT_LOC_ALLOC,
  FAULT_LOC_DIVIDE,
  FAULT_LOC_BCOPY
};

#endif
