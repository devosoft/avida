/*
 *  nSpecies.h
 *  Avida
 *
 *  Created by David on 10/5/05.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef nSpecies_h
#define nSpecies_h

namespace nSpecies {
  enum tRecording { RECORD_OFF = 0, RECORD_FULL = 1, RECORD_LIMITED = 2 };
  enum { QUEUE_NONE = 0, QUEUE_ACTIVE, QUEUE_INACTIVE, QUEUE_GARBAGE };
  static const int MAX_DISTANCE = 20;
}

#endif
